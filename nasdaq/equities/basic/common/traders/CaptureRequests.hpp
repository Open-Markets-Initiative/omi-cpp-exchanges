#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>
#include "Capture.hpp"
#include "../protocols/orderentry/structs/ClientPacketHeader.hpp"
#include "../protocols/orderentry/SoupBinSession.hpp"

namespace sim::traders {

namespace captured_orders = ::nasdaq::nsmequities::orders::ouch::v5_0;
namespace captured_session = ::sim::orderentry::soupbin;

// The requests a capture of Nasdaq.NsmEquities.Orders.Ouch.v5.0 carried: each client packet is walked as
// 'Client Packet Header' frames it, and the ones carrying unsequenced data are the requests a
// client sent. What is inside them is this venue's own order entry, so a trader can send them as
// its own — the gateway reads them exactly as it read the client's.
class CaptureRequests {
public:
    explicit CaptureRequests(const Capture& capture) {
        for (const auto& stream : capture.payloads()) walk(stream);
    }

    const std::vector<std::string>& requests() const { return requests_; }

private:
    // Tcp gives up its bytes in whatever lengths it likes, so packets are reassembled as they
    // complete, the way a session reads them.
    void walk(std::string_view bytes) {
        buffer_.append(bytes);
        std::size_t at = 0;
        while (buffer_.size() - at >= sizeof(captured_orders::client_packet_header)) {
            const auto* header = captured_orders::client_packet_header::parse(reinterpret_cast<const std::byte*>(buffer_.data() + at));
            const std::size_t whole = 0 + sizeof(header->packet_length) + header->packet_length.get().value();
            if (whole <= sizeof(*header) || buffer_.size() - at < whole) break;
            if (static_cast<captured_session::code>(header->client_packet_type.get().value()) == captured_session::Code::payload_application)
                requests_.emplace_back(buffer_, at + sizeof(captured_orders::client_packet_header), whole - sizeof(captured_orders::client_packet_header));
            at += whole;
        }
        buffer_.erase(0, at);
    }

    std::string buffer_;
    std::vector<std::string> requests_;
};

// Replays a capture of this venue's own order entry: each step sends the next requests the
// captured client sent, as this trader's own orders. A capture that runs out starts again, so a short
// one still keeps the venue busy; an empty one leaves the trader quiet.
template <typename Orders>
class CaptureReplay {
public:
    CaptureReplay(const std::vector<std::string>& requests, int per_step)
        : requests_(requests), per_step_(per_step) {}

    void step(Orders& orders) {
        if (requests_.empty()) return;

        for (int i = 0; i < per_step_; ++i) {
            const std::string& request = requests_[at_++ % requests_.size()];
            orders.replay(request.data(), request.size());
        }
    }

    std::size_t available() const { return requests_.size(); }

private:
    const std::vector<std::string>& requests_;
    int                             per_step_;
    std::size_t                     at_ = 0;
};
}
