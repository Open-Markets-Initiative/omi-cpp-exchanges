#pragma once

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include "./messages/LoginRequestPacket.hpp"
#include "./messages/LoginRejectedPacket.hpp"
#include "./messages/LoginAcceptedPacket.hpp"
#include "./structs/ClientPacketHeader.hpp"
#include "./structs/ServerPacketHeader.hpp"

namespace sim::orderentry::soupbin {

namespace protocol = ::nasdaq::nsmequities::orders::ouch::v5_0;

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
using code = typename wire_code<std::remove_cvref_t<decltype(std::declval<const protocol::client_packet_header&>().client_packet_type.get().value())>>::type;

// Packet type codes, as the branch rules of Nasdaq.NsmEquities.Orders.Ouch.v5.0 select them.
// One flat set, because the same struct is read from both ends: a client reading a
// code called "received" would be reading the server's word for what it sends.
struct Code {
    static constexpr code login_request = static_cast<code>('L');
    static constexpr code login_rejected = static_cast<code>('J');
    static constexpr code login_accepted = static_cast<code>('A');
    static constexpr code payload_application = static_cast<code>('U');
    static constexpr code session_logout = static_cast<code>('O');
    static constexpr code session_heartbeat_received = static_cast<code>('R');
    static constexpr code payload_sequenced = static_cast<code>('S');
    static constexpr code session_heartbeat_sent = static_cast<code>('H');
    static constexpr code session_end = static_cast<code>('Z');
};

// The outcome a refused request reports, as the model types it.
using status = std::remove_cvref_t<decltype(std::declval<const protocol::login_rejected_packet&>().fields.reject_reason_code.get().value())>;

// The states this connection rests in, as the chart names them.
enum class State { NotLoggedIn, Authenticating, LoggedIn };

// One connection's session, server side.
// Send:    callable(const char*, std::size_t) writing to the other end
// Handler: answers what the session cannot decide itself, and receives what it does not interpret:
// login(username, password, session_id, requested_sequence_number)
// message(packet)
// logout()
template <typename Send, typename Handler>
class Session {
public:
    Session(std::string_view session_id, Send send, Handler& handler)
        : session_id_(session_id), send_(send), handler_(handler) {}

    bool logged_in() const { return state_ == State::LoggedIn; }
    std::uint64_t next_sequence() const { return next_sequence_; }

    void resume_from(std::uint64_t sequence_number) {
        next_sequence_ = sequence_number;
    }
    void sequenced(const char* message, std::size_t length) {
        if (state_ != State::LoggedIn) return;
        frame(Code::payload_sequenced, message, length);
        next_sequence_ = next_sequence_ + 1;
    }
    void heartbeat() {
        if (state_ != State::LoggedIn) return;
        frame(Code::session_heartbeat_sent, nullptr, 0);
    }
    void end_of_session() {
        if (state_ != State::LoggedIn) return;
        frame(Code::session_end, nullptr, 0);
    }

    // Bytes read from the other end: each whole packet is handled as it completes.
    void on_bytes(const char* data, std::size_t length) {
        buffer_.append(data, length);
        std::size_t at = 0;
        while (buffer_.size() - at >= sizeof(protocol::client_packet_header)) {
            const auto* header = protocol::client_packet_header::parse(reinterpret_cast<const std::byte*>(buffer_.data() + at));
            const std::size_t whole = header->packet_length.get().value() + 2;
            if (buffer_.size() - at < whole) break;
            on_packet(static_cast<code>(header->client_packet_type.get().value()), buffer_.data() + at, whole);
            at += whole;
        }
        buffer_.erase(0, at);
    }


private:
    // A packet from the other end, answered by what the chart allows in this state.
    void on_packet(code value, const char* packet, std::size_t length) {
        const std::size_t body = sizeof(protocol::client_packet_header);
        switch (state_) {
            case State::NotLoggedIn:
                switch (value) {
                    case Code::login_request:
                        if (length >= sizeof(protocol::login_request_packet)) authenticating(*protocol::login_request_packet::parse(reinterpret_cast<const std::byte*>(packet)));
                        break;
                    default:
                        break;   // the chart answers nothing else here
                }
                break;
            case State::Authenticating:
                break;   // nothing the chart answers here
            case State::LoggedIn:
                switch (value) {
                    case Code::session_logout:
                        handler_.logout();
                        state_ = State::NotLoggedIn;
                        break;
                    case Code::session_heartbeat_received:
                        break;
                    default:
                        if (length <= body) break;   // a packet with no payload of its own says nothing
                        handler_.message(std::string_view(packet, length));
                        break;
                }
                break;
        }
    }

    void authenticating(const protocol::login_request_packet& request) {
        const auto requested_sequence_number_text = request.fields.requested_sequence_number.get_trimmed().value();
        std::uint64_t requested_sequence_number = 0;
        std::from_chars(requested_sequence_number_text.data(), requested_sequence_number_text.data() + requested_sequence_number_text.size(), requested_sequence_number);
        const auto refused = handler_.login(request.fields.username.get_trimmed().value(), request.fields.password.get_trimmed().value(), request.fields.requested_session.get_trimmed().value(), requested_sequence_number);
        if (!refused) {
            if (requested_sequence_number != 0) {
                next_sequence_ = requested_sequence_number;
            }
            protocol::login_accepted_packet reply{};
            reply.fields.accepted_session.set(session_id_);
            char number[24];
            const auto written = std::to_chars(number, number + sizeof number, next_sequence_);
            reply.fields.accepted_sequence_number.set(std::string_view(number, static_cast<std::size_t>(written.ptr - number)));
            send_(reinterpret_cast<const char*>(&reply), sizeof reply);
            state_ = State::LoggedIn;
            return;
        }
        if (refused) {
            protocol::login_rejected_packet reply{};
            reply.fields.reject_reason_code.set(*refused);
            send_(reinterpret_cast<const char*>(&reply), sizeof reply);
            state_ = State::NotLoggedIn;
            return;
        }
    }

    // One packet out: its header, then its payload.
    void frame(code value, const char* payload, std::size_t length) {
        protocol::server_packet_header header{};
        header.packet_length.set(static_cast<std::uint16_t>(sizeof(protocol::server_packet_header) + length - 2));
        header.server_packet_type.set(static_cast<std::remove_cvref_t<decltype(header.server_packet_type.get().value())>>(value));
        std::string packet(reinterpret_cast<const char*>(&header), sizeof header);
        if (length > 0) packet.append(payload, length);
        send_(packet.data(), packet.size());
    }

    std::string session_id_;
    std::uint64_t next_sequence_ = 1;
    Send send_;
    Handler& handler_;
    std::string buffer_;
    State state_ = State::NotLoggedIn;
};

}
