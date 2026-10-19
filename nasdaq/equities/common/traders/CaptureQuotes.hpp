#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>
#include "Capture.hpp"
#include "../protocols/marketdata/totalview/structs/PacketHeader.hpp"
#include "../protocols/marketdata/totalview/structs/MessageHeader.hpp"
#include "../protocols/marketdata/totalview/messages/AddOrderNoMpidAttributionMessage.hpp"

namespace sim::traders {

namespace captured_totalview = ::nasdaq::nsmequities::totalview::itch::v5_0_2026;

// One quote a capture showed: what a participant would have to send to stand in its place.
struct Quote {
    char          side     = 'B';
    std::int64_t  price    = 0;
    std::uint64_t quantity = 0;
};

// The quotes a capture of Nasdaq.NsmEquities.TotalView.Itch.v5.0.2026 showed: each packet's messages, walked as
// 'Packet' lays them out, and the fields NsmEquities.Ipc.Transforms.xml says carry a quote.
// The traits of both sides were checked when the feed's own transforms were generated, so
// reading them back needs no conversion beyond the width.
class CaptureQuotes {
public:
    explicit CaptureQuotes(const Capture& capture) {
        for (const auto& packet : capture.payloads()) walk(packet);
    }

    const std::vector<Quote>& quotes() const { return quotes_; }

private:
    void walk(std::string_view packet) {
        if (packet.size() < sizeof(captured_totalview::packet_header)) return;
        const auto* bytes = reinterpret_cast<const std::byte*>(packet.data());
        const auto* head = captured_totalview::packet_header::parse(bytes);
        const std::size_t messages = head->message_count.get().value();
        std::size_t at = sizeof(captured_totalview::packet_header);
        for (std::size_t i = 0; i < messages; ++i) {
            if (at + sizeof(captured_totalview::message_header) > packet.size()) return;
            const auto* head = captured_totalview::message_header::parse(bytes + at);
            const std::size_t block = 0 + sizeof(head->message_length) + head->message_length.get().value();
            if (at + block > packet.size()) return;

            switch (static_cast<char>(head->message_type.get().value())) {
                case 'A': {
                    const auto& m = *captured_totalview::add_order_no_mpid_attribution_message::parse(bytes + at);
                    quotes_.push_back(Quote{
                        /*side*/ static_cast<char>(m.fields.buy_sell_indicator.get().value()),
                        /*price*/ static_cast<std::int64_t>(m.fields.price.get().value()),
                        /*quantity*/ static_cast<std::uint64_t>(m.fields.shares.get().value()),
                    });
                    break;
                }
                default: break;   // a message no transform reads a quote from
            }
            at += block;
        }
    }

    std::vector<Quote> quotes_;
};

// Makes markets from a capture: each step gives up what it posted last and posts the next quotes
// the captured market showed. A capture that runs out starts again, so a short one still makes a
// market for as long as the venue runs; an empty one leaves the trader quiet.
template <typename Orders>
class CaptureMaker {
public:
    CaptureMaker(const std::vector<Quote>& quotes, int per_step)
        : quotes_(quotes), per_step_(per_step) {}

    void step(Orders& orders) {
        for (std::uint64_t reference : posted_) orders.cancel(reference);
        posted_.clear();
        if (quotes_.empty()) return;

        for (int i = 0; i < per_step_; ++i) {
            const Quote& quote = quotes_[at_++ % quotes_.size()];
            posted_.push_back(quote.side == 'B' ? orders.buy (quote.price, quote.quantity)
                                                : orders.sell(quote.price, quote.quantity));
        }
    }

    std::size_t available() const { return quotes_.size(); }

private:
    const std::vector<Quote>& quotes_;
    int                       per_step_;
    std::size_t               at_ = 0;
    std::vector<std::uint64_t> posted_;
};
}
