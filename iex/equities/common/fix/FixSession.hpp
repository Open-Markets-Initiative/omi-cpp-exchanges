#pragma once

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string_view>
#include "FixWire.hpp"

namespace iexiexequities::order_entry {

// Session settings (from the sim config). begin_string e.g. "FIXT.1.1".
struct SessionConfig {
    std::string_view begin_string;
    std::string_view sender_comp_id;
    std::string_view target_comp_id;
    std::uint32_t    heartbeat_seconds = 30;
};

enum class SessionState { Disconnected, LogonSent, LoggedOn, LogoutSent };

// FIX session state machine. Transport-decoupled: `Transport` is any callable
// (const char*, std::size_t) that writes framed bytes to the wire — socket,
// shared-memory ring, or in-process queue — so the threading / IPC model is a
// wiring choice, not baked in. `OnApp` receives decoded application messages
// (msg_type, body view). No allocations on the hot path.
template <typename Transport, typename OnApp>
class Session {
public:
    Session(SessionConfig cfg, Transport transport, OnApp on_app)
        : cfg_(cfg), transport_(transport), on_app_(on_app) {}

    SessionState  state()   const { return state_; }
    std::uint32_t out_seq() const { return out_seq_; }
    std::uint32_t in_seq()  const { return in_seq_; }

    // Initiate the session: send Logon (35=A) with the negotiated heartbeat.
    void logon() {
        char body[128];
        char* p = start(body, 'A');                                 // 35=A 34=<seq>
        p = detail::put_uint(p, 108, cfg_.heartbeat_seconds);       // 108=HeartBtInt
        send(body, static_cast<std::size_t>(p - body));
        state_ = SessionState::LogonSent;
    }

    // Graceful logout (35=5).
    void logout() {
        char body[64];
        char* p = start(body, '5');
        send(body, static_cast<std::size_t>(p - body));
        state_ = SessionState::LogoutSent;
    }

    // Application heartbeat (35=0).
    void heartbeat() {
        char body[64];
        char* p = start(body, '0');
        send(body, static_cast<std::size_t>(p - body));
    }

    // Test request (35=1) with a TestReqID.
    void test_request(std::string_view id) {
        char body[128];
        char* p = start(body, '1');
        p = detail::put_str(p, 112, id.data(), id.size());          // 112=TestReqID
        send(body, static_cast<std::size_t>(p - body));
    }

    // Feed raw inbound bytes (one framed message) into the session.
    void on_data(std::string_view bytes) {
        char          mt  = message_type(bytes);
        std::uint32_t seq = field_uint(bytes, 34);                  // 34=MsgSeqNum

        // Inbound sequence gap: request a resend of everything from the expected seq.
        // Logon (A) and SequenceReset (4) are processed regardless (they re-baseline seq).
        if (seq > in_seq_ && mt != 'A' && mt != '4') {
            request_resend(in_seq_, 0);
            return;
        }

        switch (mt) {
            case '0': break;                                        // Heartbeat
            case '1': heartbeat(); break;                           // TestRequest -> Heartbeat
            case '2': handle_resend_request(bytes); break;          // ResendRequest -> replay
            case '4': handle_sequence_reset(bytes); break;          // SequenceReset / GapFill
            case '5': state_ = SessionState::Disconnected; break;   // Logout
            case 'A': state_ = SessionState::LoggedOn; break;       // Logon ack
            default:  on_app_(mt, bytes); break;                    // Application message
        }
        in_seq_ = (seq >= in_seq_) ? seq + 1 : in_seq_ + 1;
    }

    // --- Replay support (ResendRequest / SequenceReset gap-fill) ---

    // Handle an inbound ResendRequest (35=2): replay stored messages [BeginSeqNo,EndSeqNo]
    // verbatim with PossDupFlag (43=Y); missing seqs become SequenceReset-GapFill.
    void handle_resend_request(std::string_view bytes) {
        std::uint32_t begin = field_uint(bytes, 7);                 // 7=BeginSeqNo
        std::uint32_t end   = field_uint(bytes, 16);                // 16=EndSeqNo (0 = through last)
        std::uint32_t last  = (end == 0) ? (out_seq_ - 1) : end;
        for (std::uint32_t s = begin; s > 0 && s <= last; ++s) {
            const Stored& st = store_[s % kDepth];
            if (st.seq == s && st.len > 0) {
                char body[kMaxMsg + 8];
                std::memcpy(body, st.bytes, st.len);
                char* p = body + st.len;
                p = detail::tag(p, 43); *p++ = 'Y'; *p++ = '\x01';  // 43=Y PossDupFlag
                emit(body, static_cast<std::size_t>(p - body));     // replay: no seq bump, no re-store
            } else {
                gap_fill(s);
            }
        }
    }

    // Reset the inbound sequence to NewSeqNo (36) — SequenceReset (35=4), gap-fill or reset.
    void handle_sequence_reset(std::string_view bytes) {
        std::uint32_t new_seq = field_uint(bytes, 36);              // 36=NewSeqNo
        if (new_seq > in_seq_) in_seq_ = new_seq;
    }

    // Ask the peer to resend [begin, end] (end 0 = infinity): ResendRequest (35=2).
    void request_resend(std::uint32_t begin, std::uint32_t end) {
        char body[64];
        char* p = start(body, '2');
        p = detail::put_uint(p, 7, begin);                          // 7=BeginSeqNo
        p = detail::put_uint(p, 16, end);                           // 16=EndSeqNo
        send(body, static_cast<std::size_t>(p - body));
    }

private:
    // SequenceReset-GapFill for a seq that isn't in the store (35=4, 123=Y, 36=NewSeqNo).
    void gap_fill(std::uint32_t seq) {
        char body[64];
        char* p = body;
        p = detail::tag(p, 35); *p++ = '4'; *p++ = '\x01';          // 35=4 SequenceReset
        p = detail::put_uint(p, 34, seq);                           // 34=MsgSeqNum
        p = detail::tag(p, 123); *p++ = 'Y'; *p++ = '\x01';         // 123=Y GapFillFlag
        p = detail::put_uint(p, 36, seq + 1);                       // 36=NewSeqNo
        emit(body, static_cast<std::size_t>(p - body));
    }

    // Begin a message body: "35=<type>\x01 34=<out_seq>\x01". Returns the write cursor.
    char* start(char* body, char msg_type) {
        char* p = body;
        p = detail::tag(p, 35); *p++ = msg_type; *p++ = '\x01';     // 35=<type>
        p = detail::put_uint(p, 34, out_seq_);                      // 34=MsgSeqNum
        return p;
    }

    // Store the body (for resend), frame + transport, bump the outbound sequence.
    void send(const char* body, std::size_t len) {
        Stored& s = store_[out_seq_ % kDepth];
        s.seq = out_seq_;
        s.len = (len < kMaxMsg) ? len : kMaxMsg;
        std::memcpy(s.bytes, body, s.len);
        emit(body, len);
        ++out_seq_;
    }

    // Frame a body (adds 8= / 9= / 10= via the wire layer) and hand it to the transport.
    // No sequence bump / store — used for both first sends (via send) and replays.
    void emit(const char* body, std::size_t len) {
        char out[2048];
        std::size_t n = detail::frame(cfg_.begin_string, body, len, out);
        transport_(out, n);
    }

    // MsgType (tag 35) of the framed message, or '\0'.
    static char message_type(std::string_view bytes) {
        auto pos = bytes.find("\x01" "35=");
        if (pos == std::string_view::npos) return '\0';
        std::size_t v = pos + 4;
        return v < bytes.size() ? bytes[v] : '\0';
    }

    // Value of a FIX field by tag (searches "\x01<tag>="); empty view if absent.
    static std::string_view field(std::string_view bytes, std::uint32_t tag) {
        char key[12];
        char* k = key;
        *k++ = '\x01';
        char digits[10]; int n = 0; std::uint32_t t = tag;
        do { digits[n++] = static_cast<char>('0' + t % 10); t /= 10; } while (t);
        while (n) *k++ = digits[--n];
        *k++ = '=';
        std::string_view needle(key, static_cast<std::size_t>(k - key));
        auto pos = bytes.find(needle);
        if (pos == std::string_view::npos) return {};
        std::size_t vs = pos + needle.size();
        auto soh = bytes.find('\x01', vs);
        if (soh == std::string_view::npos) return {};
        return bytes.substr(vs, soh - vs);
    }

    static std::uint32_t field_uint(std::string_view bytes, std::uint32_t tag) {
        std::string_view v = field(bytes, tag);
        std::uint32_t x = 0;
        std::from_chars(v.data(), v.data() + v.size(), x);
        return x;
    }

    // Bounded resend store: last kDepth outbound messages, indexed by seq % kDepth.
    struct Stored { std::uint32_t seq = 0; std::size_t len = 0; char bytes[512]; };
    static constexpr std::size_t kDepth  = 1024;
    static constexpr std::size_t kMaxMsg = 512;

    SessionConfig cfg_;
    Transport     transport_;
    OnApp         on_app_;
    SessionState  state_   = SessionState::Disconnected;
    std::uint32_t out_seq_ = 1;
    std::uint32_t in_seq_  = 1;
    Stored        store_[kDepth];
};}
