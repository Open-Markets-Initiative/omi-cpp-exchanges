#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string_view>
#include <vector>

namespace snap {

// Snap message type codes (ascii).
struct MessageType {
    static constexpr char SnapshotRequest = 'r';
    static constexpr char ErrorResponse   = 'e';
    static constexpr char SnapshotStart   = 's';
    static constexpr char SnapshotData    = 'd';
    static constexpr char SnapshotEnd     = 'x';
};

// Reject reason codes for ErrorResponse.
struct RejectReason {
    static constexpr char AuthenticationFailure = 'A';
    static constexpr char IncorrectChannelId    = 'C';
    static constexpr char AlreadyActive         = 'E';
    static constexpr char QuotaExceeded         = 'Q';
    static constexpr char NotYetAvailable       = 'R';
    static constexpr char IncorrectSessionId    = 'S';
    static constexpr char UnknownMessage        = 'U';
};

// Snap snapshot server for one feed. Retains published IexTp segments (bounded) and
// replays them as a snapshot: 's' + one 'd' per segment + 'x'. `Send` writes framed
// bytes back to the requesting client.
class SnapServer {
public:
    explicit SnapServer(std::size_t max_segments = 4096) : max_segments_(max_segments) {}

    // Feed-side hook: retain one published IexTp segment (verbatim bytes).
    void on_segment(const char* segment, std::size_t len) {
        if (segments_.size() >= max_segments_) return;   // bounded; late segments dropped
        segments_.emplace_back(segment, segment + len);
        ++last_sequence_;
    }

    std::size_t   segment_count() const { return segments_.size(); }
    std::uint64_t last_sequence() const { return last_sequence_; }

    // Client-side hook: handle one framed Snap message; respond via `send`.
    // Returns true when the message was a valid SnapshotRequest and a snapshot was served.
    template <typename Send>
    bool on_message(std::string_view bytes, Send send) {
        // [len u16][type u8][data]
        if (bytes.size() < 3) { error(send, RejectReason::UnknownMessage); return false; }
        char type = bytes[2];
        if (type != MessageType::SnapshotRequest) { error(send, RejectReason::UnknownMessage); return false; }
        if (bytes.size() < 3 + 40 + 4 + 4 + 8) { error(send, RejectReason::UnknownMessage); return false; }

        // Minimum Sequence Number (last 8 bytes of the request, little endian).
        std::uint64_t minimum = 0;
        std::memcpy(&minimum, bytes.data() + 3 + 40 + 4 + 4, 8);
        if (minimum > last_sequence_) { error(send, RejectReason::NotYetAvailable); return false; }

        serve(send);
        return true;
    }

private:
    // Send the full snapshot: SnapshotStart + SnapshotData per segment + SnapshotEnd.
    template <typename Send>
    void serve(Send send) {
        // Total response length: start(3+8) + per segment (3 + segment) + end(3+8).
        std::uint64_t total = (3 + 8) + (3 + 8);
        for (const auto& segment : segments_) total += 3 + segment.size();

        char small[16];
        std::size_t n = frame(small, MessageType::SnapshotStart, &total, 8);
        send(small, n);

        for (const auto& segment : segments_) {
            std::vector<char> data(3 + segment.size());
            std::uint16_t len = static_cast<std::uint16_t>(1 + segment.size());
            std::memcpy(data.data(), &len, 2);
            data[2] = MessageType::SnapshotData;
            std::memcpy(data.data() + 3, segment.data(), segment.size());
            send(data.data(), data.size());
        }

        n = frame(small, MessageType::SnapshotEnd, &last_sequence_, 8);
        send(small, n);
    }

    template <typename Send>
    void error(Send send, char reason) {
        char out[4];
        std::uint16_t len = 2;                 // type + reason
        std::memcpy(out, &len, 2);
        out[2] = MessageType::ErrorResponse;
        out[3] = reason;
        send(out, 4);
    }

    // Frame [len u16][type][payload] into out; returns bytes written.
    static std::size_t frame(char* out, char type, const void* payload, std::size_t payload_len) {
        std::uint16_t len = static_cast<std::uint16_t>(1 + payload_len);
        std::memcpy(out, &len, 2);
        out[2] = type;
        std::memcpy(out + 3, payload, payload_len);
        return 3 + payload_len;
    }

    std::size_t                    max_segments_;
    std::vector<std::vector<char>> segments_;
    std::uint64_t                  last_sequence_ = 0;
};

}