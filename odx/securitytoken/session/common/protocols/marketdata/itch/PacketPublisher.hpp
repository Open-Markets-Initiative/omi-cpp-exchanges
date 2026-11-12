#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace sim::itch {

// Itch packets: Odx.OdxSecurityToken.Pts.Itch.v1.2 'Server Soup Bin Tcp Packet', whose 'Server Packet Header' selects the message in
// it, so each message's struct is the whole packet and goes out as it is, one a packet.
// `Publish` is a callable(const char*, std::size_t).
template <typename Publish>
class PacketPublisher {
public:
    PacketPublisher(std::string_view, Publish publish) : publish_(publish) {}

    std::uint64_t next_seq() const { return seq_; }

    // Publish one message, which is the whole packet.
    void publish_message(const char* message, std::size_t length) {
        publish_(message, length);
        ++seq_;
    }

    // Nothing waits to be sent: every message went out as it was published.
    void flush() {}

private:
    Publish publish_;
    std::uint64_t seq_ = 1;
};

}
