#pragma once

#include <charconv>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
#include "SoupBinSession.hpp"
#include "./messages/ClientHeartbeat.hpp"
#include "./messages/LogoutRequest.hpp"
#include "./messages/LoginRequestPacket.hpp"
#include "./messages/LoginAcceptedPacket.hpp"
#include "./messages/LoginRejectedPacket.hpp"
#include "./messages/SequencedDataPacket.hpp"
#include "./messages/EndOfSession.hpp"

namespace sim::orderentry::soupbin {

// The states the client rests in, as its chart names them.
enum class ClientState { NotLoggedIn, LoggingIn, LoggedIn, Ended };

// The time a client's timers wait by, in nanoseconds: a steady clock, since a timer waits
// an interval and no interval should move when the wall clock does.
struct ClientClock {
    std::int64_t operator()() const {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }
};

// The client side of a session: log in, send requests, read the venue's answers.
// Send:    callable(const char*, std::size_t) writing to the venue
// Clock:   callable() telling the time in nanoseconds, which its timers wait by
// Handler: what the client tells the service:
// accepted(session, sequence_number)
// rejected(status)
// message(packet)
// ended()
// silent()
template <typename Send, typename Handler, typename Clock = ClientClock>
class Client {
public:
    Client(Send send, Handler& handler, Clock clock = {}) : send_(send), handler_(handler), clock_(clock) {}

    bool not_logged_in() const { return state_ == ClientState::NotLoggedIn; }
    bool logging_in() const { return state_ == ClientState::LoggingIn; }
    bool logged_in() const { return state_ == ClientState::LoggedIn; }
    bool ended() const { return state_ == ClientState::Ended; }
    std::uint64_t next_sequence() const { return next_sequence_; }

    void unsequenced(const char* message, std::size_t length) {
        now_ns_ = clock_();
        const bool outer = busy_;
        busy_ = true;
        frame(static_cast<code>('U'), message, length);
        beat_due_ = due(heartbeat_interval_ * 1'000'000'000);
        if (!outer) drain();
    }
    void heartbeat() {
        now_ns_ = clock_();
        const bool outer = busy_;
        busy_ = true;
        frame(static_cast<code>('R'), nullptr, 0);
        beat_due_ = due(heartbeat_interval_ * 1'000'000'000);
        if (!outer) drain();
    }
    void logout() {
        now_ns_ = clock_();
        const bool outer = busy_;
        busy_ = true;
        switch (state_) {
            case ClientState::LoggedIn:
                beat_due_ = 0;
                silence_due_ = 0;
                break;
            default:
                break;
        }
        frame(static_cast<code>('O'), nullptr, 0);
        state_ = ClientState::NotLoggedIn;
        if (!outer) drain();
    }
    void login(std::string_view username = {}, std::string_view password = {}, std::string_view session_id = {}, std::uint64_t requested_sequence_number = 0) {
        now_ns_ = clock_();
        const bool outer = busy_;
        busy_ = true;
        switch (state_) {
            case ClientState::LoggedIn:
                beat_due_ = 0;
                silence_due_ = 0;
                break;
            default:
                break;
        }
        {
            char requested_sequence_number_text[24];
            const auto requested_sequence_number_written = std::to_chars(requested_sequence_number_text, requested_sequence_number_text + sizeof requested_sequence_number_text, requested_sequence_number);
            protocol::login_request_packet out{};
            out.fields.username.set(username);
            out.fields.password.set(password);
            out.fields.requested_session.set(session_id);
            out.fields.requested_sequence_number.set(std::string_view(requested_sequence_number_text, static_cast<std::size_t>(requested_sequence_number_written.ptr - requested_sequence_number_text)));
            send_(reinterpret_cast<const char*>(&out), sizeof out);
        }
        state_ = ClientState::LoggingIn;
        if (!outer) drain();
    }

    // The time now: every timer due by it fires, the earliest first, and two due together in
    // the order the chart starts them. Called as often as the owner can; a timer fires late by
    // as long as the owner leaves between calls.
    void tick() {
        now_ns_ = clock_();
        const bool outer = busy_;
        busy_ = true;
        for (;;) {
            std::int64_t* due = nullptr;
            for (std::int64_t* timer : {&beat_due_, &silence_due_}) {
                if (*timer != 0 && *timer <= now_ns_ && (due == nullptr || *timer < *due)) due = timer;
            }
            if (due == nullptr) break;
            *due = 0;
            if (due == &beat_due_) fire_heartbeat();
            else if (due == &silence_due_) fire_silence();
        }
        if (!outer) drain();
    }

    // Bytes read from the venue. Each is handled once the client has finished the transition
    // it is in: a venue answering on the same thread - a replay read in process - answers before
    // the client has entered the state its answer belongs to, and a statechart takes one event
    // at a time.
    void on_bytes(const char* data, std::size_t length) {
        now_ns_ = clock_();
        pending_.append(data, length);
        if (!busy_) drain();
    }

private:
    // Every whole packet waiting, in the order it came, and any that came while they were read.
    void drain() {
        busy_ = true;
        while (!pending_.empty()) {
            buffer_.append(pending_);
            pending_.clear();
            std::size_t at = 0;
            while (buffer_.size() - at >= sizeof(protocol::server_packet_header)) {
                const auto* header = protocol::server_packet_header::parse(reinterpret_cast<const std::byte*>(buffer_.data() + at));
                const std::size_t whole = header->packet_length.get().value() + 2;
                if (buffer_.size() - at < whole) break;
                on_packet(static_cast<code>(header->server_packet_type.get().value()), buffer_.data() + at, whole);
                at += whole;
            }
            buffer_.erase(0, at);
        }
        busy_ = false;
    }

private:
    void on_packet(code value, const char* packet, std::size_t length) {
        (void)packet;
        (void)length;
        switch (state_) {
            case ClientState::NotLoggedIn:
                break;   // nothing the chart answers here
            case ClientState::LoggingIn:
                switch (value) {
                    case static_cast<code>('A'): {
                        if (length < sizeof(protocol::login_accepted_packet)) break;
                        const auto& reply = *protocol::login_accepted_packet::parse(reinterpret_cast<const std::byte*>(packet));
                        const auto sequence_number_text = reply.fields.accepted_sequence_number.get_trimmed().value();
                        std::uint64_t sequence_number = 0;
                        std::from_chars(sequence_number_text.data(), sequence_number_text.data() + sequence_number_text.size(), sequence_number);
                        if (sequence_number != static_cast<std::remove_cvref_t<decltype(sequence_number)>>(0)) {
                            next_sequence_ = sequence_number;
                            handler_.accepted(reply.fields.accepted_session.get_trimmed().value(), next_sequence_);
                            beat_due_ = due(heartbeat_interval_ * 1'000'000'000);
                            silence_due_ = due(silence_interval_ * 1'000'000'000);
                            state_ = ClientState::LoggedIn;
                            break;
                        }
                        {
                            handler_.accepted(reply.fields.accepted_session.get_trimmed().value(), next_sequence_);
                            beat_due_ = due(heartbeat_interval_ * 1'000'000'000);
                            silence_due_ = due(silence_interval_ * 1'000'000'000);
                            state_ = ClientState::LoggedIn;
                            break;
                        }
                    }
                    case static_cast<code>('J'): {
                        if (length < sizeof(protocol::login_rejected_packet)) break;
                        const auto& reply = *protocol::login_rejected_packet::parse(reinterpret_cast<const std::byte*>(packet));
                        {
                            handler_.rejected(reply.fields.reject_reason_code.get().value());
                            state_ = ClientState::NotLoggedIn;
                            break;
                        }
                    }
                    default: {
                        break;   // heartbeats and anything else the client need not answer
                    }
                }
                break;
            case ClientState::LoggedIn:
                switch (value) {
                    case static_cast<code>('S'): {
                        if (length <= sizeof(protocol::server_packet_header)) break;   // a packet with no payload of its own says nothing
                        {
                            handler_.message(std::string_view(packet, length));
                            next_sequence_ = next_sequence_ + 1;
                            silence_due_ = due(silence_interval_ * 1'000'000'000);
                            break;
                        }
                    }
                    case static_cast<code>('Z'): {
                        {
                            beat_due_ = 0;
                            silence_due_ = 0;
                            handler_.ended();
                            state_ = ClientState::Ended;
                            break;
                        }
                    }
                    default: {
                        {
                            silence_due_ = due(silence_interval_ * 1'000'000'000);
                            break;
                        }
                    }
                }
                break;
            case ClientState::Ended:
                break;   // nothing the chart answers here
        }
    }

    // Timer.Heartbeat, in whichever state answers it; dropped in any other.
    void fire_heartbeat() {
        switch (state_) {
            case ClientState::LoggedIn: {
                frame(static_cast<code>('R'), nullptr, 0);
                beat_due_ = due(heartbeat_interval_ * 1'000'000'000);
                break;
            }
            default:
                break;
        }
    }

    // Timer.Silence, in whichever state answers it; dropped in any other.
    void fire_silence() {
        switch (state_) {
            case ClientState::LoggedIn: {
                beat_due_ = 0;
                silence_due_ = 0;
                handler_.silent();
                state_ = ClientState::NotLoggedIn;
                break;
            }
            default:
                break;
        }
    }

    std::int64_t due(const std::int64_t wait_ns) const { return wait_ns > 0 ? now_ns_ + wait_ns : 0; }

    // One packet to the venue: its header, then its payload.
    void frame(code value, const char* payload, std::size_t length) {
        protocol::client_packet_header header{};
        header.packet_length.set(static_cast<std::uint16_t>(sizeof(protocol::client_packet_header) + length - 2));
        header.client_packet_type.set(static_cast<std::remove_cvref_t<decltype(header.client_packet_type.get().value())>>(value));
        std::string packet(reinterpret_cast<const char*>(&header), sizeof header);
        if (length > 0) packet.append(payload, length);
        send_(packet.data(), packet.size());
    }

    Send send_;
    Handler& handler_;
    Clock clock_;
    std::int64_t now_ns_ = 0;   // the time the client last read its clock at
    std::string buffer_;
    std::string pending_;
    bool busy_ = false;
    std::uint64_t next_sequence_ = 1;
    std::int64_t heartbeat_interval_ = 1;   // seconds
    std::int64_t silence_interval_ = 15;   // seconds
    std::int64_t beat_due_ = 0;   // when the timer Beat is due; 0 when it is not running
    std::int64_t silence_due_ = 0;   // when the timer Silence is due; 0 when it is not running
    ClientState state_ = ClientState::NotLoggedIn;
};

}
