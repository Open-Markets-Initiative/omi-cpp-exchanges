#pragma once

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
#include "NnfSession.hpp"
#include "./messages/HeartbeatMessage.hpp"
#include "./messages/SignOffRequestInMessage.hpp"
#include "./messages/SignOnRequestInMessage.hpp"
#include "./messages/SignOnRequestOutMessage.hpp"
#include "./messages/SignOffRequestOutMessage.hpp"

namespace sim::orderentry::nnf {

// The states the client rests in, as its chart names them.
enum class ClientState { NotLoggedIn, LoggingIn, LoggedIn };

// The client side of a session: log in, send requests, read the venue's answers.
// Send:    callable(const char*, std::size_t) writing to the venue
// Handler: what the client tells the service:
// rejected(status)
// accepted(username)
// message(packet)
// ended()
template <typename Send, typename Handler>
class Client {
public:
    Client(Send send, Handler& handler) : send_(send), handler_(handler) {}

    bool not_logged_in() const { return state_ == ClientState::NotLoggedIn; }
    bool logging_in() const { return state_ == ClientState::LoggingIn; }
    bool logged_in() const { return state_ == ClientState::LoggedIn; }

    void unsequenced(const char* message, std::size_t length) {
        const bool outer = busy_;
        busy_ = true;
        send_(message, length);
        if (!outer) drain();
    }
    void heartbeat() {
        const bool outer = busy_;
        busy_ = true;
        frame(static_cast<code>(23506), nullptr, 0);
        if (!outer) drain();
    }
    void logout() {
        const bool outer = busy_;
        busy_ = true;
        frame(static_cast<code>(2320), nullptr, 0);
        state_ = ClientState::NotLoggedIn;
        if (!outer) drain();
    }
    void login(std::uint64_t username = 0, std::string_view password = {}) {
        const bool outer = busy_;
        busy_ = true;
        {
            protocol::sign_on_request_in_message out{};
            out.fields.user_id.set(static_cast<std::remove_cvref_t<decltype(out.fields.user_id.get().value())>>(username));
            out.fields.password.set(password);
            send_(reinterpret_cast<const char*>(&out), sizeof out);
        }
        state_ = ClientState::LoggingIn;
        if (!outer) drain();
    }

    // Bytes read from the venue. Each is handled once the client has finished the transition
    // it is in: a venue answering on the same thread - a replay read in process - answers before
    // the client has entered the state its answer belongs to, and a statechart takes one event
    // at a time.
    void on_bytes(const char* data, std::size_t length) {
        pending_.append(data, length);
        if (!busy_) drain();
    }

private:
    // Every whole packet waiting, in the order it came, and any that came while they were read.
    void drain() {
        busy_ = true;
        while (!pending_.empty()) {
            buffer_.append(pending_);
            pending_.clear();
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
        busy_ = false;
    }

private:
    void on_packet(code value, const char* packet, std::size_t length) {
        (void)packet;
        (void)length;
        switch (state_) {
            case ClientState::NotLoggedIn:
                break;   // nothing the chart answers here
            case ClientState::LoggingIn:
                switch (value) {
                    case static_cast<code>(2301): {
                        if (length < sizeof(protocol::sign_on_request_out_message)) break;
                        const auto& reply = *protocol::sign_on_request_out_message::parse(reinterpret_cast<const std::byte*>(packet));
                        if (reply.header.error_code.get().value() != static_cast<std::remove_cvref_t<decltype(reply.header.error_code.get().value())>>(0)) {
                            handler_.rejected(reply.header.error_code.get().value());
                            state_ = ClientState::NotLoggedIn;
                            break;
                        }
                        {
                            handler_.accepted(reply.fields.user_id.get().value());
                            state_ = ClientState::LoggedIn;
                            break;
                        }
                    }
                    default: {
                        break;   // heartbeats and anything else the client need not answer
                    }
                }
                break;
            case ClientState::LoggedIn:
                switch (value) {
                    case static_cast<code>(2321): {
                        {
                            handler_.ended();
                            state_ = ClientState::NotLoggedIn;
                            break;
                        }
                    }
                    default: {
                        if (length <= sizeof(protocol::message_header)) break;   // a packet with no payload of its own says nothing
                        {
                            handler_.message(std::string_view(packet, length));
                            break;
                        }
                    }
                }
                break;
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
    std::string pending_;
    bool busy_ = false;
    ClientState state_ = ClientState::NotLoggedIn;
};

}
