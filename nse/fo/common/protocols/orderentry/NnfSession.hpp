#pragma once

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include "./messages/SignOnRequestInMessage.hpp"
#include "./messages/SignOnRequestOutMessage.hpp"
#include "./structs/MessageHeader.hpp"

namespace sim::orderentry::nnf {

namespace protocol = ::nse::nsefo::orderentry::nnf::v9_50;

// The type of a packet code, as the model types the field it is read from: a letter
// in SoupBin, a number in Nnf.
// 
// Taken down to what it is on the wire where the model types it as an enumeration. A
// dual-tree protocol has one enum per direction, and the codes below are one flat set
// read from both ends, so holding them in either direction's enum would mean a switch
// over values that are not in the type it switches on - complete, correct and warned
// about. The byte is the thing both ends agree on; the enums are each end's view of it.
template <typename Code, bool = std::is_enum_v<Code>> struct wire_code { using type = Code; };
template <typename Code> struct wire_code<Code, true> { using type = std::underlying_type_t<Code>; };
using code = typename wire_code<std::remove_cvref_t<decltype(std::declval<const protocol::message_header&>().transaction_code.get().value())>>::type;

// Packet type codes, as the branch rules of Nse.NseFo.OrderEntry.Nnf.v9.50 select them.
// One flat set, because the same struct is read from both ends: a client reading a
// code called "received" would be reading the server's word for what it sends.
struct Code {
    static constexpr code login_request = static_cast<code>(2300);
    static constexpr code login_accepted = static_cast<code>(2301);
    static constexpr code session_logout_received = static_cast<code>(2320);
    static constexpr code session_heartbeat = static_cast<code>(23506);
    static constexpr code session_logout_sent = static_cast<code>(2321);
};

// The outcome a refused request reports, as the model types it.
using status = std::remove_cvref_t<decltype(std::declval<const protocol::message_header&>().error_code.get().value())>;

// The states this connection rests in, as the chart names them.
enum class State { NotSignedOn, Authenticating, SignedOn };

// One connection's session, server side.
// Send:    callable(const char*, std::size_t) writing to the other end
// Handler: answers what the session cannot decide itself, and receives what it does not interpret:
// login(username, password)
// message(packet)
// logout()
template <typename Send, typename Handler>
class Session {
public:
    Session(Send send, Handler& handler)
        : send_(send), handler_(handler) {}

    bool signed_on() const { return state_ == State::SignedOn; }

    void message(const char* message, std::size_t length) {
        if (state_ != State::SignedOn) return;
        send_(message, length);
    }
    void logout() {
        if (state_ != State::SignedOn) return;
        frame(Code::session_logout_sent, nullptr, 0);
        state_ = State::NotSignedOn;
    }

    // Bytes read from the other end: each whole packet is handled as it completes.
    void on_bytes(const char* data, std::size_t length) {
        buffer_.append(data, length);
        std::size_t at = 0;
        while (buffer_.size() - at >= sizeof(protocol::message_header)) {
            const auto* header = protocol::message_header::parse(reinterpret_cast<const std::byte*>(buffer_.data() + at));
            const std::size_t whole = header->message_length.get().value();
            if (buffer_.size() - at < whole) break;
            on_packet(static_cast<code>(header->transaction_code.get().value()), buffer_.data() + at, whole);
            at += whole;
        }
        buffer_.erase(0, at);
    }


private:
    // A packet from the other end, answered by what the chart allows in this state.
    void on_packet(code value, const char* packet, std::size_t length) {
        const std::size_t body = sizeof(protocol::message_header);
        switch (state_) {
            case State::NotSignedOn:
                switch (value) {
                    case Code::login_request:
                        if (length >= sizeof(protocol::sign_on_request_in_message)) authenticating(*protocol::sign_on_request_in_message::parse(reinterpret_cast<const std::byte*>(packet)));
                        break;
                    default:
                        break;   // the chart answers nothing else here
                }
                break;
            case State::Authenticating:
                break;   // nothing the chart answers here
            case State::SignedOn:
                switch (value) {
                    case Code::session_logout_received:
                        handler_.logout();
                        state_ = State::NotSignedOn;
                        break;
                    case Code::session_heartbeat:
                        break;
                    default:
                        if (length <= body) break;   // a packet with no payload of its own says nothing
                        handler_.message(std::string_view(packet, length));
                        break;
                }
                break;
        }
    }

    void authenticating(const protocol::sign_on_request_in_message& request) {
        const auto refused = handler_.login(request.fields.user_id.get().value(), request.fields.password.get_trimmed().value());
        if (!refused) {
            protocol::sign_on_request_out_message reply{};
            reply.header.error_code.set(status{});
            reply.fields.user_id.set(request.fields.user_id.get().value());
            send_(reinterpret_cast<const char*>(&reply), sizeof reply);
            state_ = State::SignedOn;
            return;
        }
        if (refused) {
            frame(Code::login_accepted, nullptr, 0, static_cast<status>(*refused));
            state_ = State::NotSignedOn;
            return;
        }
    }

    // One packet out: its header, then its payload.
    void frame(code value, const char* payload, std::size_t length, status outcome = status{}) {
        protocol::message_header header{};
        header.message_length.set(static_cast<std::uint16_t>(sizeof(protocol::message_header) + length));
        header.transaction_code.set(static_cast<std::remove_cvref_t<decltype(header.transaction_code.get().value())>>(value));
        header.error_code.set(outcome);
        std::string packet(reinterpret_cast<const char*>(&header), sizeof header);
        if (length > 0) packet.append(payload, length);
        send_(packet.data(), packet.size());
    }

    Send send_;
    Handler& handler_;
    std::string buffer_;
    State state_ = State::NotSignedOn;
};

}
