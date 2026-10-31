#pragma once

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
#include "SoupBinSession.hpp"

namespace sim::orderentry::soupbin {

// The client side of a session: log in, send requests, read the venue's answers.
// Send:    callable(const char*, std::size_t) writing to the venue
// Handler: void accepted(session_id, sequence_number);
// void rejected(status);
// void message(std::string_view packet) for each data packet the venue
// sends, header included, as the model's structs lay it out;
// void ended()
template <typename Send, typename Handler>
class Client {
public:
    Client(Send send, Handler& handler) : send_(send), handler_(handler) {}

    bool logged_in() const { return logged_in_; }
    std::uint64_t next_sequence() const { return next_sequence_; }

    // Ask to join a session, presenting what the venue reads out of the login.
    void login(std::string_view username = {}, std::string_view password = {}, std::string_view session_id = {}, std::uint64_t requested_sequence_number = 0) {
        char requested_sequence_number_text[24];
        const auto requested_sequence_number_written = std::to_chars(requested_sequence_number_text, requested_sequence_number_text + sizeof requested_sequence_number_text, requested_sequence_number);
        protocol::login_request_packet out{};
        out.fields.username.set(username);
        out.fields.password.set(password);
        out.fields.requested_session.set(session_id);
        out.fields.requested_sequence_number.set(std::string_view(requested_sequence_number_text, static_cast<std::size_t>(requested_sequence_number_written.ptr - requested_sequence_number_text)));
        send_(reinterpret_cast<const char*>(&out), sizeof out);
    }

    // A request to the venue, as everything the client sends is.
    void unsequenced(const char* message, std::size_t length) { frame(Code::payload_application, message, length); }

    void heartbeat() { frame(Code::session_heartbeat_received, nullptr, 0); }
    void logout() { frame(Code::session_logout, nullptr, 0); logged_in_ = false; }

    // Bytes read from the venue: each whole packet is handled as it completes.
    void on_bytes(const char* data, std::size_t length) {
        buffer_.append(data, length);
        std::size_t at = 0;
        while (buffer_.size() - at >= sizeof(protocol::server_packet_header)) {
            const auto* header = protocol::server_packet_header::parse(reinterpret_cast<const std::byte*>(buffer_.data() + at));
            const std::size_t whole = header->packet_length.get().value() + 2;
            if (buffer_.size() - at < whole) break;
            on_packet(static_cast<code>(header->server_packet_type.get().value()), buffer_.data() + at, whole);
            at += whole;
        }
        buffer_.erase(0, at);
    }

private:
    void on_packet(code value, const char* packet, std::size_t length) {
        switch (value) {
            case Code::login_accepted: {
                if (length < sizeof(protocol::login_accepted_packet)) return;
                const auto& reply = *protocol::login_accepted_packet::parse(reinterpret_cast<const std::byte*>(packet));
                const auto number = reply.fields.accepted_sequence_number.get_trimmed().value();
                std::uint64_t given = 0;
                std::from_chars(number.data(), number.data() + number.size(), given);
                if (given != 0) next_sequence_ = given;
                logged_in_ = true;
                handler_.accepted(reply.fields.accepted_session.get_trimmed().value(), next_sequence_);
                return;
            }
            case Code::login_rejected: {
                if (length < sizeof(protocol::login_rejected_packet)) return;
                const auto& reply = *protocol::login_rejected_packet::parse(reinterpret_cast<const std::byte*>(packet));
                handler_.rejected(reply.fields.reject_reason_code.get().value());
                return;
            }
            case Code::payload_sequenced:
                if (length > sizeof(protocol::server_packet_header)) {
                    handler_.message(std::string_view(packet, length));
                    ++next_sequence_;
                }
                return;
            case Code::session_end:
                logged_in_ = false;
                handler_.ended();
                return;
            default:
                return;   // heartbeats and anything else the client need not answer
        }
    }

    // One packet to the venue: its header, then its payload.
    void frame(code value, const char* payload, std::size_t length) {
        protocol::client_packet_header header{};
        header.packet_length.set(static_cast<std::uint16_t>(sizeof(protocol::client_packet_header) + length - 2));
        header.client_packet_type.set(static_cast<std::remove_cvref_t<decltype(header.client_packet_type.get().value())>>(value));
        std::string packet(reinterpret_cast<const char*>(&header), sizeof header);
        if (length > 0) packet.append(payload, length);
        send_(packet.data(), packet.size());
    }

    Send send_;
    Handler& handler_;
    std::string buffer_;
    bool logged_in_ = false;
    std::uint64_t next_sequence_ = 1;
};

}
