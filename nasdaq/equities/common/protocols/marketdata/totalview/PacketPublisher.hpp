#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <charconv>
#include <string_view>
#include "structs/PacketHeader.hpp"

namespace sim::totalview {

namespace protocol = ::nasdaq::nsmequities::totalview::itch::v5_0_2026;

// TotalView packets: Nasdaq.NsmEquities.TotalView.Itch.v5.0.2026 'Packet', a 'Packet Header' then the messages it counts.
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
        auto* header = protocol::packet_header::parse(reinterpret_cast<std::byte*>(buffer_));
        header->session.set(session_);
        header->sequence_number.set(seq_ - count_);
        header->message_count.set(count_);
        publish_(buffer_, size_);
        reset();
    }

private:
    void reset() {
        std::memset(buffer_, 0, sizeof(protocol::packet_header));
        size_ = sizeof(protocol::packet_header);
        count_ = 0;
    }

    static constexpr std::size_t kMtu = 1400;
    std::string_view session_;
    std::uint64_t stream_ = 0;
    Publish publish_;
    std::uint64_t seq_ = 1;
    char buffer_[1500];
    std::size_t size_ = sizeof(protocol::packet_header);
    std::uint16_t count_ = 0;
};

}
