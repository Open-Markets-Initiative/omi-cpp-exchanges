#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <charconv>
#include <string_view>
#include "structs/StreamHeader.hpp"

namespace sim::mtbt {

namespace protocol = ::nse::nsefo::mtbt::binary::v7_1;

// Mtbt packets: Nse.NseFo.Mtbt.Binary.v7.1 'Packet', a 'Stream Header' then the messages it counts.
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

    // Publish one encoded message, which is the whole packet.
    void publish_message(const char* message, std::size_t length) {
        if (size_ + length > kMtu) flush();
        std::memcpy(buffer_ + size_, message, length);
        size_ += length;
        ++count_;
        ++seq_;
        flush();
    }

    // Fill the header and hand the packet to the publisher.
    void flush() {
        if (count_ == 0) return;
        auto* header = protocol::stream_header::parse(reinterpret_cast<std::byte*>(buffer_));
        header->message_length.set(static_cast<std::uint16_t>(size_ - sizeof(*header) + 8));
        header->stream_id.set(static_cast<decltype(header->stream_id)::result_type::value_type>(stream_));
        header->sequence_number.set(seq_ - count_);
        publish_(buffer_, size_);
        reset();
    }

private:
    void reset() {
        std::memset(buffer_, 0, sizeof(protocol::stream_header));
        size_ = sizeof(protocol::stream_header);
        count_ = 0;
    }

    static constexpr std::size_t kMtu = 1400;
    std::string_view session_;
    std::uint64_t stream_ = 0;
    Publish publish_;
    std::uint64_t seq_ = 1;
    char buffer_[1500];
    std::size_t size_ = sizeof(protocol::stream_header);
    std::uint16_t count_ = 0;
};

}
