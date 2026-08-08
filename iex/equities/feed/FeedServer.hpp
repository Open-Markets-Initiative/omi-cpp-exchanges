#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace feed {

// IEX-TP segment header (outbound). The precise byte layout comes from the IEX-TP
// header model (Iex.IexEquities.IexTpHeader.Udp.v1.0); this mirrors its fields.
struct SegmentHeader {
    std::uint8_t  version        = 1;
    std::uint8_t  reserved       = 0;
    std::uint16_t protocol_id    = 0;   // per feed: TOPS / DEEP / DEEP+
    std::uint32_t channel_id     = 1;
    std::uint32_t session_id     = 0;
    std::uint16_t payload_length = 0;
    std::uint16_t message_count  = 0;
    std::uint64_t stream_offset  = 0;
    std::uint64_t first_seq      = 0;   // sequence number of the first message
    std::uint64_t send_time      = 0;   // nanoseconds
};

// Market-data feed server: packs already-encoded messages into IEX-TP segments and
// hands full segments to `Publisher` (a callable(const char*, std::size_t) — UDP
// multicast socket, or any transport). One instance per feed.
template <typename Publisher>
class FeedServer {
public:
    FeedServer(std::uint16_t protocol_id, std::uint32_t session_id, Publisher publish)
        : protocol_id_(protocol_id), session_id_(session_id), publish_(publish) { reset(); }

    std::uint64_t next_seq() const { return seq_; }

    // Append one already-encoded feed message; flush the segment when near MTU.
    void publish_message(const char* msg, std::size_t len) {
        if (size_ + len > kMtu) flush();
        std::memcpy(buf_ + size_, msg, len);
        size_ += len;
        ++count_;
        ++seq_;
    }

    // Emit the current segment (fills the header, hands bytes to the publisher).
    void flush() {
        if (count_ == 0) return;
        auto* h = reinterpret_cast<SegmentHeader*>(buf_);
        h->protocol_id    = protocol_id_;
        h->session_id     = session_id_;
        h->message_count  = count_;
        h->payload_length = static_cast<std::uint16_t>(size_ - sizeof(SegmentHeader));
        h->first_seq      = seq_ - count_;
        // TODO(feed): stream_offset + send_time (ns clock).
        publish_(buf_, size_);
        reset();
    }

private:
    void reset() { size_ = sizeof(SegmentHeader); count_ = 0; }

    static constexpr std::size_t kMtu = 1400;
    std::uint16_t protocol_id_;
    std::uint32_t session_id_;
    Publisher     publish_;
    std::uint64_t seq_   = 1;
    char          buf_[1500];
    std::size_t   size_  = sizeof(SegmentHeader);
    std::uint16_t count_ = 0;
};}
