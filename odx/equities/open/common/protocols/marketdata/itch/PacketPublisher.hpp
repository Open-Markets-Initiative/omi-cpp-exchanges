#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <charconv>
#include <string_view>
#include <chrono>
#include "structs/PacketHeader.hpp"

namespace sim::itch {

namespace protocol = ::odx::odxequities::pts::itch::v2_2;

// Itch packets: Odx.OdxEquities.Pts.Itch.v2.2 'Packet', a 'Packet Header' then the messages it counts.
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
        sent_ = std::chrono::steady_clock::now();
        reset();
    }

    // Nothing published for a second: the header alone goes out, saying the session is alive
    // and the number its next message will carry - or, once it has ended, that it is over.
    void idle() {
        if (std::chrono::steady_clock::now() - sent_ < std::chrono::seconds(1)) return;
        flush();
        signal(ended_ ? 65535 : 0);
    }

    // The session's last message has gone: say so, now and each second after.
    void end_session() {
        flush();
        ended_ = true;
        signal(65535);
    }

private:
    // A packet of no messages: its header, the count saying what it is.
    void signal(std::uint16_t count) {
        auto* header = protocol::packet_header::parse(reinterpret_cast<std::byte*>(buffer_));
        header->session.set(session_);
        header->sequence_number.set(seq_ - count_);
        header->message_count.set(count);
        publish_(buffer_, sizeof(*header));
        sent_ = std::chrono::steady_clock::now();
        reset();
    }

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
    std::chrono::steady_clock::time_point sent_ = std::chrono::steady_clock::now();   // when a packet last went out
    bool ended_ = false;   // has the session's last message gone?
};

}
