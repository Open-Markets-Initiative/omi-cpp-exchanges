#pragma once

#include <array>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <string>
#include <string_view>
#include <vector>

#include "FixText.hpp"

namespace iexiexequities::order_entry {

// What an acceptor is configured with:
// the begin string its frames open with, and every field the
// model requires that neither the chart nor the framing fills.
struct AcceptorConfig {
    std::string_view begin_string = "FIX.4.2";
    std::string_view sender_comp_id{};
};

// Where a session is: the leaf states of Fix's chart, as it lists them.
enum class AcceptorState { NotLoggedOn, Authenticating, Talking, LoggingOut, Ended };

// The acceptor side of the Fix session, as its chart has it. Transport is
// any callable taking (const char*, std::size_t) that writes one whole frame.
// Handler is asked what the session cannot decide and handed what it does
// not interpret. Both are held by value: a caller keeping its own names them
// by reference. MaxBody is the longest body an application message may
// have; the session's buffers and the messages it keeps are sized by it.
//
// Time is the session's: every call that can send or start a timer is told
// the time, in nanoseconds since the epoch, and tick() fires what is due.
template <typename Transport, typename Handler, std::size_t MaxBody = 1024>
class Acceptor {
public:
    Acceptor(AcceptorConfig config, Transport transport, Handler handler)
        : config_(config), transport_(transport), handler_(handler), sent_(4096) {}

    AcceptorState state() const { return state_; }
    // In Connected, whichever of its states.
    bool in_connected() const { return state_ == AcceptorState::NotLoggedOn || state_ == AcceptorState::Authenticating || state_ == AcceptorState::Talking || state_ == AcceptorState::LoggingOut; }
    // In LoggedOn, whichever of its states.
    bool in_logged_on() const { return state_ == AcceptorState::Talking || state_ == AcceptorState::LoggingOut; }
    // Over: the transport closes the connection.
    bool ended() const { return state_ == AcceptorState::Ended; }
    std::int64_t next_sequence() const { return next_sequence_; }
    std::int64_t expected_sequence() const { return expected_sequence_; }
    std::int64_t heartbeat_interval() const { return heartbeat_interval_; }

    // One whole FIX message from the transport, framed: begin string
    // to checksum. Returns false when it carries no MsgType, which is
    // not a FIX message at all; what the chart does not answer in the
    // state it arrives in is dropped.
    bool on_message(std::string_view received, const std::int64_t now_ns) {
        now_ns_ = now_ns;
        const Arrived arrived{field(received, Tag::MsgType), received};

        // Whoever logs on is answered under the
        // SenderCompID they log on with: the first
        // message is the Logon, or the session ends.
        if (counterparty_.empty()) {
            counterparty_ = field(received, Tag::SenderCompID);
        }

        if (arrived.type.empty()) {
            return false;
        }

        switch (state_) {
            case AcceptorState::NotLoggedOn: return arriving_not_logged_on(arrived);
            case AcceptorState::Authenticating: return true;
            case AcceptorState::Talking: return arriving_talking(arrived);
            case AcceptorState::LoggingOut: return arriving_logging_out(arrived);
            case AcceptorState::Ended: return true;
        }

        return true;
    }

    // Bytes off the stream, in whatever pieces they come: each whole
    // message in them is taken in turn, and what is left waits for the
    // rest. A message is cut as the model's Reassemble rule has it -
    // its BeginString, its BodyLength, that many bytes and the trailer
    // after them. One whose checksum is wrong is dropped, as FIX drops
    // a garbled message; a stream that is not FIX at all returns
    // false, and the transport closes it.
    bool on_bytes(const char* data, const std::size_t length, const std::int64_t now_ns) {
        stream_.insert(stream_.end(), data, data + length);
        std::size_t at = 0;

        for (;;) {
            const std::string_view rest{stream_.data() + at, stream_.size() - at};
            const auto cut = framed(rest);

            if (cut.is == Cuts::Partial) {
                break;
            }

            if (cut.is == Cuts::Foreign || (cut.is == Cuts::Whole && !on_message(rest.substr(0, cut.length), now_ns))) {
                stream_.clear();
                return false;
            }

            at += cut.length;
        }

        stream_.erase(stream_.begin(), stream_.begin() + static_cast<std::ptrdiff_t>(at));
        return true;
    }

    // Request.ResumeFrom, in Connected.
    void resume_from(const std::int64_t sequence_number) {
        if (in_connected()) {
            next_sequence_ = sequence_number;
        }
    }

    // Request.Message, in Talking. Returns the bytes written; none outside it or for a body longer than MaxBody.
    // The body is the message's own fields, tag=value each closed by SOH; the
    // header and the frame are the session's.
    std::size_t message(std::string_view type, std::string_view body, const std::int64_t now_ns) {
        if (state_ == AcceptorState::Talking && body.size() <= MaxBody) {
            now_ns_ = now_ns;
            std::size_t written = 0;
            // Send.Payload.Application
            {
                auto out = start(type, next_sequence_);
                out.raw(body);
                written += finish(out);
            }
            keep_sent(type, body);
            next_sequence_ = next_sequence_ + 1;
            // Timer.Heartbeat
            beat_due_ = due(heartbeat_interval_ * 1'000'000'000);
            return written;
        }

        return 0;
    }

    // Request.Logout, in Talking. Returns the bytes written; none outside it.
    std::size_t logout(const std::int64_t now_ns) {
        if (state_ == AcceptorState::Talking) {
            now_ns_ = now_ns;
            std::size_t written = 0;
            // Send.Session.Logout
            {
                auto out = start(MsgType::Logout, next_sequence_);
                written += finish(out);
            }
            next_sequence_ = next_sequence_ + 1;
            // Timer.LogoutUnanswered
            logout_unanswered_due_ = due(heartbeat_interval_ * 1'000'000'000);
            written += enter_logging_out();
            return written;
        }

        return 0;
    }

    // The time now: every timer due by it fires, the earliest first
    // and two due together in the order the chart starts them. Returns
    // the bytes written.
    std::size_t tick(const std::int64_t now_ns) {
        now_ns_ = now_ns;
        std::size_t written = 0;

        for (;;) {
            std::int64_t* due = nullptr;

            for (std::int64_t* timer : {&beat_due_, &silence_due_, &unanswered_due_, &logout_unanswered_due_}) {
                if (*timer != 0 && *timer <= now_ns && (due == nullptr || *timer < *due)) {
                    due = timer;
                }
            }

            if (due == nullptr) {
                return written;
            }

            *due = 0;

            if (due == &beat_due_) {
                written += fire_heartbeat();
            } else if (due == &silence_due_) {
                written += fire_silence();
            } else if (due == &unanswered_due_) {
                written += fire_unanswered();
            } else if (due == &logout_unanswered_due_) {
                written += fire_logout_unanswered();
            }
        }
    }

private:
    // A message that arrived: its MsgType, and the whole of it as framed.
    struct Arrived {
        std::string_view type;
        std::string_view text;
    };

    // NotLoggedOn: what arrives.
    bool arriving_not_logged_on(const Arrived& arrived) {
        // Receive.Login.Request -> Authenticating
        if (arrived.type == MsgType::Logon) {
            enter_authenticating(arrived);
            return true;
        }

        // Receive -> Ended
        enter_ended();
        return true;
    }

    // Talking: what arrives.
    bool arriving_talking(const Arrived& arrived) {
        // Receive.Session.Reset [Field:GapFill == false]
        if (arrived.type == MsgType::SequenceReset && field_flag(arrived.text, Tag::GapFillFlag) == false) {
            expected_sequence_ = field_int(arrived.text, Tag::NewSeqNo);
            // Timer.Silence
            silence_due_ = due(heartbeat_interval_ * 1'000'000'000);
            unanswered_due_ = 0;
            return true;
        }

        // Receive [Field:SequenceNumber > State:ExpectedSequence]
        if (field_int(arrived.text, Tag::MsgSeqNum) > expected_sequence_) {
            // Send.Session.Resend
            {
                auto out = start(MsgType::ResendRequest, next_sequence_);
                out.put(Tag::BeginSeqNo, expected_sequence_);
                out.put(Tag::EndSeqNo, std::int64_t{0});
                finish(out);
            }
            next_sequence_ = next_sequence_ + 1;
            // Timer.Silence
            silence_due_ = due(heartbeat_interval_ * 1'000'000'000);
            unanswered_due_ = 0;
            return true;
        }

        // Receive [Field:SequenceNumber < State:ExpectedSequence and Field:PossibleDuplicate == true]
        if (field_int(arrived.text, Tag::MsgSeqNum) < expected_sequence_ && field_flag(arrived.text, Tag::PossDupFlag) == true) {
            // Timer.Silence
            silence_due_ = due(heartbeat_interval_ * 1'000'000'000);
            unanswered_due_ = 0;
            return true;
        }

        // Receive [Field:SequenceNumber < State:ExpectedSequence] -> Ended
        if (field_int(arrived.text, Tag::MsgSeqNum) < expected_sequence_) {
            // Send.Session.Logout
            {
                auto out = start(MsgType::Logout, next_sequence_);
                finish(out);
            }
            next_sequence_ = next_sequence_ + 1;
            enter_ended();
            return true;
        }

        // Receive.Session.Heartbeat
        if (arrived.type == MsgType::Heartbeat) {
            expected_sequence_ = expected_sequence_ + 1;
            // Timer.Silence
            silence_due_ = due(heartbeat_interval_ * 1'000'000'000);
            unanswered_due_ = 0;
            return true;
        }

        // Receive.Session.TestRequest
        if (arrived.type == MsgType::TestRequest) {
            expected_sequence_ = expected_sequence_ + 1;
            // Timer.Silence
            silence_due_ = due(heartbeat_interval_ * 1'000'000'000);
            unanswered_due_ = 0;
            // Send.Session.Heartbeat
            {
                auto out = start(MsgType::Heartbeat, next_sequence_);
                out.put(Tag::TestReqID, field(arrived.text, Tag::TestReqID));
                finish(out);
            }
            next_sequence_ = next_sequence_ + 1;
            // Timer.Heartbeat
            beat_due_ = due(heartbeat_interval_ * 1'000'000'000);
            return true;
        }

        // Receive.Session.Resend
        if (arrived.type == MsgType::ResendRequest) {
            expected_sequence_ = expected_sequence_ + 1;
            // Timer.Silence
            silence_due_ = due(heartbeat_interval_ * 1'000'000'000);
            unanswered_due_ = 0;
            // Replay Sent
            replay_sent(field_int(arrived.text, Tag::BeginSeqNo), field_int(arrived.text, Tag::EndSeqNo));
            // Timer.Heartbeat
            beat_due_ = due(heartbeat_interval_ * 1'000'000'000);
            return true;
        }

        // Receive.Session.Reset
        if (arrived.type == MsgType::SequenceReset) {
            expected_sequence_ = field_int(arrived.text, Tag::NewSeqNo);
            // Timer.Silence
            silence_due_ = due(heartbeat_interval_ * 1'000'000'000);
            unanswered_due_ = 0;
            return true;
        }

        // Receive.Session.Reject
        if (arrived.type == MsgType::Reject) {
            expected_sequence_ = expected_sequence_ + 1;
            // Timer.Silence
            silence_due_ = due(heartbeat_interval_ * 1'000'000'000);
            unanswered_due_ = 0;
            handler_.message(arrived.type, arrived.text);
            return true;
        }

        // Receive.Session.Logout -> Ended
        if (arrived.type == MsgType::Logout) {
            // Send.Session.Logout
            {
                auto out = start(MsgType::Logout, next_sequence_);
                finish(out);
            }
            next_sequence_ = next_sequence_ + 1;
            handler_.logout();
            enter_ended();
            return true;
        }

        // Receive.Login.Request
        if (arrived.type == MsgType::Logon) {
            expected_sequence_ = expected_sequence_ + 1;
            // Timer.Silence
            silence_due_ = due(heartbeat_interval_ * 1'000'000'000);
            unanswered_due_ = 0;
            // Send.Session.Reject
            {
                auto out = start(MsgType::Reject, next_sequence_);
                out.put(Tag::RefSeqNum, field_int(arrived.text, Tag::MsgSeqNum));
                finish(out);
            }
            next_sequence_ = next_sequence_ + 1;
            return true;
        }

        // Receive.Payload.Application
        if (!named(arrived.type)) {
            expected_sequence_ = expected_sequence_ + 1;
            // Timer.Silence
            silence_due_ = due(heartbeat_interval_ * 1'000'000'000);
            unanswered_due_ = 0;
            handler_.message(arrived.type, arrived.text);
            return true;
        }

        return true;
    }

    // LoggingOut: what arrives.
    bool arriving_logging_out(const Arrived& arrived) {
        // Receive.Session.Logout -> Ended
        if (arrived.type == MsgType::Logout) {
            logout_unanswered_due_ = 0;
            handler_.logout();
            enter_ended();
            return true;
        }

        return true;
    }

    // Timer.Heartbeat, in whichever state answers it; dropped in any other.
    std::size_t fire_heartbeat() {
        std::size_t written = 0;

        switch (state_) {
            case AcceptorState::Talking:
            case AcceptorState::LoggingOut:
                // Timer.Heartbeat
                // Send.Session.Heartbeat
                {
                    auto out = start(MsgType::Heartbeat, next_sequence_);
                    written += finish(out);
                }
                next_sequence_ = next_sequence_ + 1;
                // Timer.Heartbeat
                beat_due_ = due(heartbeat_interval_ * 1'000'000'000);
                break;
            default:
                break;
        }

        return written;
    }

    // Timer.Silence, in whichever state answers it; dropped in any other.
    std::size_t fire_silence() {
        std::size_t written = 0;

        switch (state_) {
            case AcceptorState::Talking:
                // Timer.Silence
                // Send.Session.TestRequest
                {
                    auto out = start(MsgType::TestRequest, next_sequence_);
                    out.put(Tag::TestReqID, next_sequence_);
                    written += finish(out);
                }
                next_sequence_ = next_sequence_ + 1;
                // Timer.Heartbeat
                beat_due_ = due(heartbeat_interval_ * 1'000'000'000);
                // Timer.Unanswered
                unanswered_due_ = due(heartbeat_interval_ * 1'000'000'000);
                break;
            default:
                break;
        }

        return written;
    }

    // Timer.Unanswered, in whichever state answers it; dropped in any other.
    std::size_t fire_unanswered() {
        std::size_t written = 0;

        switch (state_) {
            case AcceptorState::Talking:
                // Timer.Unanswered -> Ended
                // Send.Session.Logout
                {
                    auto out = start(MsgType::Logout, next_sequence_);
                    written += finish(out);
                }
                next_sequence_ = next_sequence_ + 1;
                written += enter_ended();
                break;
            default:
                break;
        }

        return written;
    }

    // Timer.LogoutUnanswered, in whichever state answers it; dropped in any other.
    std::size_t fire_logout_unanswered() {
        std::size_t written = 0;

        switch (state_) {
            case AcceptorState::LoggingOut:
                // Timer.LogoutUnanswered -> Ended
                written += enter_ended();
                break;
            default:
                break;
        }

        return written;
    }

    // Entering Authenticating.
    std::size_t enter_authenticating(const Arrived& arrived) {
        std::size_t written = 0;
        state_ = AcceptorState::Authenticating;
        // Ask.Login
        if (handler_.login(arrived.type, arrived.text)) {
            // Handler.Login.Accepted -> LoggedOn
            expected_sequence_ = expected_sequence_ + 1;
            heartbeat_interval_ = field_int(arrived.text, Tag::HeartBtInt);
            written += enter_logged_on();
        } else {
            // Handler.Login.Refused -> Ended
            // Send.Session.Logout
            {
                auto out = start(MsgType::Logout, next_sequence_);
                written += finish(out);
            }
            next_sequence_ = next_sequence_ + 1;
            written += enter_ended();
        }
        return written;
    }

    // Entering Ended.
    std::size_t enter_ended() {
        state_ = AcceptorState::Ended;
        return 0;
    }

    // Entering LoggedOn.
    std::size_t enter_logged_on() {
        std::size_t written = 0;
        // Send.Login.Accepted
        {
            auto out = start(MsgType::Logon, next_sequence_);
            out.put(Tag::HeartBtInt, heartbeat_interval_);
            written += finish(out);
        }
        next_sequence_ = next_sequence_ + 1;
        // Timer.Heartbeat
        beat_due_ = due(heartbeat_interval_ * 1'000'000'000);
        // Timer.Silence
        silence_due_ = due(heartbeat_interval_ * 1'000'000'000);
        written += enter_talking();
        return written;
    }

    // Entering Talking.
    std::size_t enter_talking() {
        state_ = AcceptorState::Talking;
        return 0;
    }

    // Entering LoggingOut.
    std::size_t enter_logging_out() {
        state_ = AcceptorState::LoggingOut;
        return 0;
    }

    // Replay Sent: what it kept of From to To (To 0
    // meaning through the last sent) again, each under its own
    // number and marked a possible duplicate, and a
    // SequenceReset for each run of numbers it did not keep,
    // moving the stream past them - the session's own messages,
    // which FIX never resends.
    std::size_t replay_sent(std::int64_t from, std::int64_t to) {
        const std::int64_t last = next_sequence_ - 1;
        std::size_t written = 0;

        if (to == 0 || to > last) {
            to = last;
        }

        if (from < 1) {
            from = 1;
        }

        for (std::int64_t number = from; number <= to;) {
            const Kept& kept = sent_[static_cast<std::size_t>(number) % sent_.size()];

            if (kept.number == number) {
                auto out = start_again(std::string_view{kept.type.data(), kept.type_length}, number, kept.sent_ns);
                out.raw(std::string_view{kept.body.data(), kept.length});
                written += finish(out);
                ++number;
                continue;
            }

            std::int64_t past = number + 1;

            while (past <= to && sent_[static_cast<std::size_t>(past) % sent_.size()].number != past) {
                ++past;
            }

            // Send.Session.Reset
            auto out = start_again(MsgType::SequenceReset, number, now_ns_);
            out.put(Tag::NewSeqNo, past);
            out.put(Tag::GapFillFlag, true);
            written += finish(out);
            number = past;
        }

        return written;
    }

    // Keep what was just sent in Sent, under the number it went
    // out with; a type longer than a kept one holds is not kept, and a
    // resend fills its gap.
    void keep_sent(std::string_view type, std::string_view body) {
        Kept& kept = sent_[static_cast<std::size_t>(next_sequence_) % sent_.size()];

        if (type.size() > kept.type.size()) {
            kept.number = 0;
            return;
        }

        kept.number = next_sequence_;
        kept.sent_ns = now_ns_;
        kept.type_length = type.size();
        std::memcpy(kept.type.data(), type.data(), type.size());
        kept.length = body.size();
        std::memcpy(kept.body.data(), body.data(), body.size());
    }

    // Is this a message one of the chart's events names? Anything else
    // is the application's, whatever the model calls it.
    static bool named(std::string_view type) { return type == MsgType::Logon || type == MsgType::SequenceReset || type == MsgType::Heartbeat || type == MsgType::TestRequest || type == MsgType::ResendRequest || type == MsgType::Reject || type == MsgType::Logout; }

    // A message's header, under the number it goes out with.
    Writer start(std::string_view type, const std::int64_t number) {
        Writer out{body_.data(), body_.data() + body_.size()};
        out.put(Tag::MsgType, type);
        out.put(Tag::SenderCompID, config_.sender_comp_id);
        out.put(Tag::TargetCompID, std::string_view{counterparty_});
        out.put(Tag::MsgSeqNum, number);
        out.put_time(Tag::SendingTime, now_ns_);
        return out;
    }

    // The same, for a message sent again: marked a possible duplicate,
    // with the time it was first sent.
    Writer start_again(std::string_view type, const std::int64_t number, const std::int64_t first_sent_ns) {
        Writer out{body_.data(), body_.data() + body_.size()};
        out.put(Tag::MsgType, type);
        out.put(Tag::SenderCompID, config_.sender_comp_id);
        out.put(Tag::TargetCompID, std::string_view{counterparty_});
        out.put(Tag::MsgSeqNum, number);
        out.put_time(Tag::SendingTime, now_ns_);
        out.put(Tag::PossDupFlag, true);
        out.put_time(Tag::OrigSendingTime, first_sent_ns);
        return out;
    }

    // Frame what was written - begin string, body length, checksum -
    // and hand it to the transport. Nothing goes when it did not fit.
    std::size_t finish(const Writer& out) {
        const auto length = out.used(body_.data());

        if (out.overflowed || length + config_.begin_string.size() + Framing > frame_.size()) {
            return 0;
        }

        const auto framed = detail::frame(config_.begin_string, body_.data(), length, frame_.data());
        transport_(frame_.data(), framed);
        return framed;
    }

    // What a cut finds: not yet a whole message, one whole, one whose
    // checksum is wrong, or something that is not FIX.
    enum class Cuts { Partial, Whole, Garbled, Foreign };

    struct Cut {
        Cuts is;
        std::size_t length;
    };

    // How far into a stream its first SOH may be: BeginString and
    // BodyLength are short.
    static constexpr std::size_t Opening = 64;
    // The trailer after the body, which BodyLength does not count.
    static constexpr std::size_t Trailer = 7;

    // Where the next message ends:
    // BeginString (8) opens it, BodyLength (9) follows it,
    // and the body that counts is followed by the trailer.
    static Cut framed(std::string_view rest) {
        constexpr std::string_view opens = "8=";
        constexpr std::string_view sizes = "9=";

        if (rest.size() < opens.size()) {
            return {Cuts::Partial, 0};
        }

        if (rest.substr(0, opens.size()) != opens) {
            return {Cuts::Foreign, 0};
        }

        const auto first = rest.find('\x01');
        const auto second = first == std::string_view::npos ? first : rest.find('\x01', first + 1);

        if (second == std::string_view::npos) {
            return {rest.size() > Opening ? Cuts::Foreign : Cuts::Partial, 0};
        }

        if (rest.substr(first + 1, sizes.size()) != sizes) {
            return {Cuts::Foreign, 0};
        }

        std::size_t body = 0;
        const auto digits = std::from_chars(rest.data() + first + 1 + sizes.size(), rest.data() + second, body);

        if (digits.ec != std::errc{} || digits.ptr != rest.data() + second || body > MaxBody + Room) {
            return {Cuts::Foreign, 0};
        }

        const auto whole = second + 1 + body + Trailer;

        if (rest.size() < whole) {
            return {Cuts::Partial, 0};
        }

        // CheckSum (10): every byte before it
        // summed, modulo 256, in three digits. A trailer that is
        // not one has lost the frame; a sum that is wrong garbles
        // one message.
        constexpr std::string_view sums = "10=";
        const auto trailer = rest.substr(whole - Trailer, Trailer);

        if (trailer.substr(0, sums.size()) != sums || trailer.back() != '\x01') {
            return {Cuts::Foreign, 0};
        }

        std::uint32_t stated = 0;
        std::from_chars(trailer.data() + sums.size(), trailer.data() + trailer.size() - 1, stated);

        if (stated != detail::checksum(rest.data(), rest.data() + whole - Trailer)) {
            return {Cuts::Garbled, whole};
        }

        return {Cuts::Whole, whole};
    }

    // When a timer waiting this long from now is due; a wait of
    // nothing or less starts nothing, as a heartbeat interval of zero
    // asks for no heartbeats.
    std::int64_t due(const std::int64_t wait_ns) const { return wait_ns > 0 ? now_ns_ + wait_ns : 0; }

    // The tags the session reads and writes, as the model numbers them.
    struct Tag {
        static constexpr std::uint32_t BeginSeqNo = 7;
        static constexpr std::uint32_t EndSeqNo = 16;
        static constexpr std::uint32_t GapFillFlag = 123;
        static constexpr std::uint32_t HeartBtInt = 108;
        static constexpr std::uint32_t MsgSeqNum = 34;
        static constexpr std::uint32_t MsgType = 35;
        static constexpr std::uint32_t NewSeqNo = 36;
        static constexpr std::uint32_t OrigSendingTime = 122;
        static constexpr std::uint32_t PossDupFlag = 43;
        static constexpr std::uint32_t RefSeqNum = 45;
        static constexpr std::uint32_t SenderCompID = 49;
        static constexpr std::uint32_t SendingTime = 52;
        static constexpr std::uint32_t TargetCompID = 56;
        static constexpr std::uint32_t TestReqID = 112;
    };

    // The message types the chart's events name, as the model gives them.
    struct MsgType {
        static constexpr std::string_view Heartbeat = "0";
        static constexpr std::string_view Logon = "A";
        static constexpr std::string_view Logout = "5";
        static constexpr std::string_view Reject = "3";
        static constexpr std::string_view ResendRequest = "2";
        static constexpr std::string_view SequenceReset = "4";
        static constexpr std::string_view TestRequest = "1";
    };

    // A message Sent keeps: its number, when it was first sent,
    // and its type and body as handed over.
    struct Kept {
        std::int64_t number = 0;
        std::int64_t sent_ns = 0;
        std::size_t type_length = 0;
        std::array<char, 3> type{};
        std::size_t length = 0;
        std::array<char, MaxBody> body{};
    };

    // What a header, the fields round it and the frame add to a body,
    // at most: the frame's own fields here, the header's in the body
    // buffer's room.
    static constexpr std::size_t Framing = 64;
    static constexpr std::size_t Room = 1024;

    AcceptorConfig config_;
    Transport transport_;
    Handler handler_;
    AcceptorState state_ = AcceptorState::NotLoggedOn;
    std::int64_t now_ns_ = 0;
    std::vector<char> stream_;
    std::string counterparty_;
    std::int64_t next_sequence_ = 1;
    std::int64_t expected_sequence_ = 1;
    std::int64_t heartbeat_interval_ = 30;
    std::vector<Kept> sent_;
    std::int64_t beat_due_ = 0;
    std::int64_t silence_due_ = 0;
    std::int64_t unanswered_due_ = 0;
    std::int64_t logout_unanswered_due_ = 0;
    std::array<char, MaxBody + Room> body_{};
    std::array<char, MaxBody + Room + Framing> frame_{};
};

}  // namespace iexiexequities::order_entry
