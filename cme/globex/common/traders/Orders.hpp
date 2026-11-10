#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <map>
#include <string>
#include <string_view>
#include <type_traits>
#include "../protocols/orderentry/messages/NewOrderSingle.hpp"
#include "../protocols/orderentry/messages/OrderCancelRequest.hpp"
#include "../protocols/orderentry/messages/ExecutionReportNew.hpp"
#include "../protocols/orderentry/messages/ExecutionReportCancel.hpp"
#include "../protocols/orderentry/messages/ExecutionReportTradeOutright.hpp"
#include "../protocols/orderentry/ILink3Client.hpp"
#include "../protocols/ipc/IpcBus.hpp"

namespace sim::traders {

namespace protocol = ::cme::ilink3::v9;
namespace ilink3 = ::sim::orderentry::ilink3;

// The value a field is set with, for a mapped value that must be cast to it.
template <typename Field>
using value_of = std::remove_cvref_t<decltype(std::declval<const Field&>().get().value())>;

// The bytes a message's fixed part takes: all of it, less the tail a variable-length one reserves.
template <typename Message>
constexpr std::size_t fixed_size() {
    if constexpr (requires { Message::tail_capacity; }) return sizeof(Message) - Message::tail_capacity;
    else return sizeof(Message);
}

// What the venue answered, counted by the message each response transform builds (CmeGlobex.Ipc.Transforms.xml).
struct Answers {
    std::uint64_t execution_report_new = 0;
    std::uint64_t execution_report_cancel = 0;
    std::uint64_t execution_report_trade_outright = 0;
};

// A strategy's intent, in the venue's own order-entry messages, on a client session.
template <typename Session>
class Orders {
public:
    explicit Orders(Session& session) : session_(session) {}

    // The instrument this trader trades, of the several the venue does: its first until told.
    void trade(std::int32_t instrument) { instrument_ = instrument; }

    std::uint64_t buy (std::int64_t price, std::uint64_t quantity) { return submit(1, price, quantity); }
    std::uint64_t sell(std::int64_t price, std::uint64_t quantity) { return submit(2, price, quantity); }

    // Give up an order the trader entered, by the reference it entered it under (ilink3_order_cancel).
    void cancel(std::uint64_t reference) {
        protocol::order_cancel_request out{};
        out.fields.clordid.set(std::to_string(reference));
        out.fields.security_id.set(instrument_);
        send(out);
        ++canceled_;
    }


    // A request read from a capture of this venue's own order entry, sent as this trader's own:
    // the reference it is entered under is the trader's, so the venue answers the trader. Returns 0
    // for a message this venue's requests do not map.
    std::uint64_t replay(const char* message, std::size_t length) {
        if (length == 0) return 0;
        auto selector = protocol::message_header::parse(reinterpret_cast<const std::byte*>(message) + 4)->template_id.get().value();
        switch (selector) {
            case static_cast<decltype(selector)>(514): {
                if (length < fixed_size<protocol::new_order_single>() - sizeof(protocol::simple_open_framing_header)) return 0;
                protocol::new_order_single out{};
                // the captured message, then the reference this trader knows its own order by
                std::memcpy(reinterpret_cast<char*>(&out) + sizeof(protocol::simple_open_framing_header), message,
                            std::min(length, sizeof out - sizeof(protocol::simple_open_framing_header)));
                // the capture's own reference, so a request acting on an earlier order still names it
                const std::string captured = std::string(out.fields.clordid.get_trimmed().value());
                const std::uint64_t reference = ++next_;
                replayed_[captured] = reference;
                out.fields.clordid.set(std::to_string(reference));
                send(out);
                ++entered_;
                return reference;
            }
            case static_cast<decltype(selector)>(516): {
                if (length < fixed_size<protocol::order_cancel_request>() - sizeof(protocol::simple_open_framing_header)) return 0;
                protocol::order_cancel_request out{};
                // the captured message, then the reference this trader knows its own order by
                std::memcpy(reinterpret_cast<char*>(&out) + sizeof(protocol::simple_open_framing_header), message,
                            std::min(length, sizeof out - sizeof(protocol::simple_open_framing_header)));
                // the capture's own reference, so a request acting on an earlier order still names it
                const std::string captured = std::string(out.fields.clordid.get_trimmed().value());
                const auto entered = replayed_.find(captured);
                if (entered == replayed_.end()) return 0;   // an order this trader never entered
                const std::uint64_t reference = entered->second;
                out.fields.clordid.set(std::to_string(reference));
                send(out);
                ++canceled_;
                return reference;
            }
            default: return 0;   // a message this venue's order entry does not map
        }
    }

    std::uint64_t entered()  const { return entered_; }
    std::uint64_t canceled() const { return canceled_; }

private:
    // An order the trader enters, numbered by its own reference (ilink3_new_order).
    std::uint64_t submit(char side, std::int64_t price, std::uint64_t quantity) {
        (void)side; (void)price; (void)quantity;
        const std::uint64_t reference = ++next_;
        protocol::new_order_single out{};
        out.fields.clordid.set(std::to_string(reference));
        out.fields.side.set(static_cast<value_of<decltype(out.fields.side)>>(side));
        out.fields.order_qty.set(quantity);
        out.fields.price_optional.set(price);
        out.fields.security_id.set(instrument_);
        send(out);
        ++entered_;
        return reference;
    }

    // One request onto the session, whole: the session sends it as it stands, and the
    // message's own header is what tells the venue which message it is.
    template <typename Message>
    void send(const Message& message) {
        const auto* bytes = reinterpret_cast<const char*>(&message);
        const std::size_t length = message.header.message_length.get().value();
        session_.unsequenced(bytes, length);
    }

    Session& session_;
    std::int32_t instrument_ = instrument_id;
    // The reference this trader entered each captured order under, for a replayed request that names it.
    std::map<std::string, std::uint64_t> replayed_;
    std::uint64_t next_ = 0;
    std::uint64_t entered_ = 0;
    std::uint64_t canceled_ = 0;
};

// A trader: a client session of this venue, the orders it speaks through, and what came
// back. Its `Send` writes to the venue; the trader is its own session handler. The strategy
// stays outside it, driving `orders()`, so any strategy runs any venue.
template <typename Send>
class Trader {
public:
    using Session = ilink3::Client<Send, Trader>;

    Trader(std::string name, std::string username, std::string password, Send send)
        : name_(std::move(name)), username_(std::move(username)), password_(std::move(password)),
          session_(send, *this), orders_(session_) {}

    Trader(const Trader&)            = delete;
    Trader& operator=(const Trader&) = delete;

    // Join the venue's session. Nothing else is sent until the login is accepted.
    void login() { session_.login(username_, password_); }

    Orders<Session>& orders() { return orders_; }

    void on_bytes(const char* data, std::size_t length) { session_.on_bytes(data, length); }

    // The session's handler: what the venue said back.
    void accepted() {}
    void rejected(ilink3::status reason) { refused_ = static_cast<char>(reason); }
    void ended() {}

    void message(std::string_view packet) {
        if (packet.size() <= sizeof(protocol::simple_open_framing_header)) return;
        auto selector = protocol::message_header::parse(reinterpret_cast<const std::byte*>(packet.data()) + 4)->template_id.get().value();
        switch (selector) {
            case static_cast<decltype(selector)>(522): ++answers_.execution_report_new; return;
            case static_cast<decltype(selector)>(534): ++answers_.execution_report_cancel; return;
            case static_cast<decltype(selector)>(525): ++answers_.execution_report_trade_outright; return;
            default: return;   // a message this trader's venue does not answer orders with
        }
    }

    const std::string& name() const { return name_; }
    const std::string& session_name() const { return session_name_; }
    const Answers& answers() const { return answers_; }
    bool logged_in() const { return session_.established(); }
    char refused() const { return refused_; }
    std::uint64_t entered() const { return orders_.entered(); }
    std::uint64_t canceled() const { return orders_.canceled(); }

private:
    std::string name_;
    std::string username_;
    std::string password_;
    std::string session_name_;
    Session     session_;
    Orders<Session> orders_;
    Answers     answers_{};
    char        refused_ = 0;
};

}
