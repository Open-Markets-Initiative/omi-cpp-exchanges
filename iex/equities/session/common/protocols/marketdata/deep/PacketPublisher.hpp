#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <charconv>
#include <string_view>
#include <chrono>
#include "../../../run/Clock.hpp"
#include "structs/IextpHeader.hpp"

namespace sim::deep {

namespace protocol = ::iex::iexequities::deep::iextp::v1_08;

// Deep packets: Iex.IexEquities.Deep.IexTp.v1.08 'Packet', a 'Iextp Header' then the messages it counts.
// Messages arrive already encoded, each with the length its model frames it with; the header
// is filled as the packet is flushed. `Publish` is a callable(const char*, std::size_t).
template <typename Publish>
class PacketPublisher {
public:
    PacketPublisher(std::string_view session, Publish publish)
        : session_(session), publish_(publish) {
        // a protocol that numbers its stream rather than naming it reads the number here
        std::from_chars(session_.data(), session_.data() + session_.size(), stream_);
        reset();
    }

    std::uint64_t next_seq() const { return seq_; }

    // Append one encoded message; flush first when it would not fit.
    void publish_message(const char* message, std::size_t length) {
        if (size_ + length > kMtu) flush();
        std::memcpy(buffer_ + size_, message, length);
        size_ += length;
        ++count_;
        ++seq_;
    }

    // Fill the header and hand the packet to the publisher.
    void flush() {
        if (count_ == 0) return;
        auto* header = protocol::iextp_header::parse(reinterpret_cast<std::byte*>(buffer_));
        header->version.set(1);
        // reserved: reserved, zeroed by reset()
        header->message_protocol_id.set(32772);
        header->channel_id.set(1);
        header->session_id.set(static_cast<decltype(header->session_id)::result_type::value_type>(stream_));
        header->payload_length.set(static_cast<std::uint16_t>(size_ - sizeof(*header) + 0));
        header->message_count.set(count_);
        header->stream_offset.set(offset_);
        header->first_message_sequence_number.set(seq_ - count_);
        header->send_time.set(static_cast<std::uint64_t>(sim::clock::now_nanos()));
        publish_(buffer_, size_);
        sent_ = std::chrono::steady_clock::now();
        offset_ += size_ - sizeof(*header);
        reset();
    }

    // Nothing published for a second: the header alone goes out, saying the session is alive
    // and the number its next message will carry - or, once it has ended, that it is over.
    void idle() {
        if (std::chrono::steady_clock::now() - sent_ < std::chrono::seconds(1)) return;
        flush();
        signal(0);
    }

private:
    // A packet of no messages: its header, the count saying what it is.
    void signal(std::uint16_t count) {
        auto* header = protocol::iextp_header::parse(reinterpret_cast<std::byte*>(buffer_));
        header->version.set(1);
        // reserved: reserved, zeroed by reset()
        header->message_protocol_id.set(32772);
        header->channel_id.set(1);
        header->session_id.set(static_cast<decltype(header->session_id)::result_type::value_type>(stream_));
        header->payload_length.set(static_cast<std::uint16_t>(size_ - sizeof(*header) + 0));
        header->message_count.set(count);
        header->stream_offset.set(offset_);
        header->first_message_sequence_number.set(seq_ - count_);
        header->send_time.set(static_cast<std::uint64_t>(sim::clock::now_nanos()));
        publish_(buffer_, sizeof(*header));
        sent_ = std::chrono::steady_clock::now();
        reset();
    }

    void reset() {
        std::memset(buffer_, 0, sizeof(protocol::iextp_header));
        size_ = sizeof(protocol::iextp_header);
        count_ = 0;
    }

    static constexpr std::size_t kMtu = 1400;
    std::string_view session_;
    std::uint64_t stream_ = 0;
    Publish publish_;
    std::uint64_t seq_ = 1;
    char buffer_[1500];
    std::size_t size_ = sizeof(protocol::iextp_header);
    std::uint16_t count_ = 0;
    std::chrono::steady_clock::time_point sent_ = std::chrono::steady_clock::now();   // when a packet last went out
    bool ended_ = false;   // has the session's last message gone?
    std::uint64_t offset_ = 0;
};

}
