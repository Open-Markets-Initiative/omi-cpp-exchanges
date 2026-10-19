#pragma once

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
#include "NnfSession.hpp"

namespace sim::orderentry::nnf {

// The client side of a session: log in, send requests, read the venue's answers.
// Send:    callable(const char*, std::size_t) writing to the venue
// Handler: void accepted(username);
// void message(std::string_view packet) for each data packet the venue
// sends, header included, as the model's structs lay it out;
template <typename Send, typename Handler>
class Client {
public:
    Client(Send send, Handler& handler) : send_(send), handler_(handler) {}

    bool logged_in() const { return logged_in_; }

    // Ask to join a session, presenting what the venue reads out of the login.
    void login(std::uint64_t username = 0, std::string_view password = {}) {
        protocol::sign_on_request_in_message out{};
        out.fields.user_id.set(static_cast<decltype(out.fields.user_id)::result_type::value_type>(username));
        out.fields.password.set(password);
        send_(reinterpret_cast<const char*>(&out), sizeof out);
    }

    // A request to the venue, as everything the client sends is.
    void unsequenced(const char* message, std::size_t length) { send_(message, length); }

    void heartbeat() { frame(Code::session_heartbeat, nullptr, 0); }
    void logout() { frame(Code::session_logout_received, nullptr, 0); logged_in_ = false; }

    // Bytes read from the venue: each whole packet is handled as it completes.
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
    void on_packet(code value, const char* packet, std::size_t length) {
        switch (value) {
            case Code::login_accepted: {
                if (length < sizeof(protocol::sign_on_request_out_message)) return;
                const auto& reply = *protocol::sign_on_request_out_message::parse(reinterpret_cast<const std::byte*>(packet));
                logged_in_ = true;
                handler_.accepted(reply.fields.user_id.get().value());
                return;
            }
            default:
                // No packet of its own carries a message here: each one is its own, so
                // anything the session does not answer itself is for the handler.
                if (length > sizeof(protocol::message_header))
                    handler_.message(std::string_view(packet, length));
                return;
        }
    }

    // One packet to the venue: its header, then its payload.
    void frame(code value, const char* payload, std::size_t length) {
        protocol::message_header header{};
        header.message_length.set(static_cast<std::uint16_t>(sizeof(protocol::message_header) + length));
        header.transaction_code.set(static_cast<std::remove_cvref_t<decltype(header.transaction_code.get().value())>>(value));
        std::string packet(reinterpret_cast<const char*>(&header), sizeof header);
        if (length > 0) packet.append(payload, length);
        send_(packet.data(), packet.size());
    }

    Send send_;
    Handler& handler_;
    std::string buffer_;
    bool logged_in_ = false;
};

}
