#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
#include "ILink3Session.hpp"
#include "./messages/Terminate.hpp"
#include "./messages/Negotiate.hpp"
#include "./messages/NegotiationResponse.hpp"
#include "./messages/NegotiationReject.hpp"
#include "./messages/Establish.hpp"
#include "./messages/EstablishmentAck.hpp"
#include "./messages/EstablishmentReject.hpp"
#include "./messages/Sequence.hpp"
#include "./messages/RetransmitReject.hpp"

namespace sim::orderentry::ilink3 {

// The states the client rests in, as its chart names them.
enum class ClientState { NotLoggedIn, Negotiating, Establishing, Established, Ended };

// The customer's side of a session: negotiate, establish, send requests, read Cme's answers.
// Send:    callable(const char*, std::size_t) writing to Cme
// Handler: what the client tells the service:
// ended()
// rejected(status)
// accepted()
// message(packet)
template <typename Send, typename Handler>
class Client {
public:
    Client(Send send, Handler& handler) : send_(send), handler_(handler) {}

    bool not_logged_in() const { return state_ == ClientState::NotLoggedIn; }
    bool negotiating() const { return state_ == ClientState::Negotiating; }
    bool establishing() const { return state_ == ClientState::Establishing; }
    bool established() const { return state_ == ClientState::Established; }
    bool ended() const { return state_ == ClientState::Ended; }
    std::uint64_t uuid() const { return uuid_; }
    std::uint64_t next_sequence() const { return next_sequence_; }
    std::uint64_t keep_alive_interval() const { return keep_alive_interval_; }

    void login(std::string_view username = {}, std::string_view password = {}) {
        const bool outer = busy_;
        busy_ = true;
        session_ = std::string(username);
        firm_ = std::string(password);
        uuid_ = now_nanos();
        next_sequence_ = 1;
        {
            protocol::negotiate out{};
            out.fields.session.set(session_);
            out.fields.firm.set(firm_);
            out.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(out.fields.uuid.get().value())>>(uuid_));
            out.fields.request_timestamp.set(static_cast<std::remove_cvref_t<decltype(out.fields.request_timestamp.get().value())>>(now_nanos()));
            // Credentials is sent empty, its length zero.
            const auto written = protocol::start_group_write(out).credentials_and_finish({});
            send_(reinterpret_cast<const char*>(written.data()), written.size());
        }
        state_ = ClientState::Negotiating;
        if (!outer) drain();
    }
    void logout() {
        const bool outer = busy_;
        busy_ = true;
        {
            protocol::terminate out{};
            out.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(out.fields.uuid.get().value())>>(uuid_));
            out.fields.request_timestamp.set(static_cast<std::remove_cvref_t<decltype(out.fields.request_timestamp.get().value())>>(now_nanos()));
            out.fields.error_codes.set(static_cast<std::remove_cvref_t<decltype(out.fields.error_codes.get().value())>>(0));
            send_(reinterpret_cast<const char*>(&out), out.header.message_length.get().value());
        }
        state_ = ClientState::NotLoggedIn;
        if (!outer) drain();
    }
    void unsequenced(const char* message, std::size_t length) {
        if (state_ != ClientState::Established) return;
        const bool outer = busy_;
        busy_ = true;
        {
            std::string out(message, length);
            number(out, next_sequence_);
            send_(out.data(), out.size());
        }
        next_sequence_ = next_sequence_ + 1;
        if (!outer) drain();
    }
    void heartbeat() {
        if (state_ != ClientState::Established) return;
        const bool outer = busy_;
        busy_ = true;
        {
            protocol::sequence out{};
            out.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(out.fields.uuid.get().value())>>(uuid_));
            out.fields.next_seq_no.set(static_cast<std::remove_cvref_t<decltype(out.fields.next_seq_no.get().value())>>(next_sequence_));
            out.fields.fault_tolerance_indicator.set(static_cast<std::remove_cvref_t<decltype(out.fields.fault_tolerance_indicator.get().value())>>(1));
            send_(reinterpret_cast<const char*>(&out), out.header.message_length.get().value());
        }
        if (!outer) drain();
    }

    // Bytes read from Cme. Each is handled once the client has finished the transition
    // it is in: Cme answering on the same thread - a replay read in process - answers before
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
            while (buffer_.size() - at >= sizeof(protocol::simple_open_framing_header) + sizeof(protocol::message_header)) {
                const auto* bytes = reinterpret_cast<const std::byte*>(buffer_.data() + at);
                const auto& header = *protocol::simple_open_framing_header::parse(bytes);
                const std::size_t whole = header.message_length.get().value();
                if (whole < sizeof(protocol::simple_open_framing_header) + sizeof(protocol::message_header)) { buffer_.clear(); return; }
                if (buffer_.size() - at < whole) break;
                on_packet(static_cast<code>(protocol::message_header::parse(bytes + sizeof(protocol::simple_open_framing_header))->template_id.get().value()), buffer_.data() + at, whole);
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
                switch (value) {
                    case static_cast<code>(507): {
                        {
                            handler_.ended();
                            state_ = ClientState::Ended;
                            break;
                        }
                    }
                    default: {
                        break;   // the chart answers nothing else here
                    }
                }
                break;
            case ClientState::Negotiating:
                switch (value) {
                    case static_cast<code>(501): {
                        {
                            {
                                protocol::establish out{};
                                out.fields.session.set(session_);
                                out.fields.firm.set(firm_);
                                out.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(out.fields.uuid.get().value())>>(uuid_));
                                out.fields.request_timestamp.set(static_cast<std::remove_cvref_t<decltype(out.fields.request_timestamp.get().value())>>(now_nanos()));
                                out.fields.next_seq_no.set(static_cast<std::remove_cvref_t<decltype(out.fields.next_seq_no.get().value())>>(next_sequence_));
                                out.fields.keep_alive_interval.set(static_cast<std::remove_cvref_t<decltype(out.fields.keep_alive_interval.get().value())>>(keep_alive_interval_));
                                // Credentials is sent empty, its length zero.
                                const auto written = protocol::start_group_write(out).credentials_and_finish({});
                                send_(reinterpret_cast<const char*>(written.data()), written.size());
                            }
                            state_ = ClientState::Establishing;
                            break;
                        }
                    }
                    case static_cast<code>(502): {
                        if (length < fixed_size<protocol::negotiation_reject>()) break;
                        const auto& reply = *protocol::negotiation_reject::parse(reinterpret_cast<const std::byte*>(packet));
                        {
                            handler_.rejected(reply.fields.error_codes.get().value());
                            state_ = ClientState::NotLoggedIn;
                            break;
                        }
                    }
                    case static_cast<code>(507): {
                        {
                            handler_.ended();
                            state_ = ClientState::Ended;
                            break;
                        }
                    }
                    default: {
                        break;   // the chart answers nothing else here
                    }
                }
                break;
            case ClientState::Establishing:
                switch (value) {
                    case static_cast<code>(504): {
                        {
                            handler_.accepted();
                            state_ = ClientState::Established;
                            break;
                        }
                    }
                    case static_cast<code>(505): {
                        if (length < fixed_size<protocol::establishment_reject>()) break;
                        const auto& reply = *protocol::establishment_reject::parse(reinterpret_cast<const std::byte*>(packet));
                        {
                            handler_.rejected(reply.fields.error_codes.get().value());
                            state_ = ClientState::NotLoggedIn;
                            break;
                        }
                    }
                    case static_cast<code>(507): {
                        {
                            handler_.ended();
                            state_ = ClientState::Ended;
                            break;
                        }
                    }
                    default: {
                        break;   // the chart answers nothing else here
                    }
                }
                break;
            case ClientState::Established:
                switch (value) {
                    case static_cast<code>(506): {
                        {
                            break;
                        }
                    }
                    case static_cast<code>(510): {
                        {
                            break;
                        }
                    }
                    case static_cast<code>(507): {
                        {
                            handler_.ended();
                            state_ = ClientState::Ended;
                            break;
                        }
                    }
                    default: {
                        if (length <= sizeof(protocol::simple_open_framing_header) + sizeof(protocol::message_header)) break;   // a frame with no message of its own says nothing
                        {
                            handler_.message(std::string_view(packet, length));
                            break;
                        }
                    }
                }
                break;
            case ClientState::Ended:
                switch (value) {
                    case static_cast<code>(507): {
                        {
                            handler_.ended();
                            state_ = ClientState::Ended;
                            break;
                        }
                    }
                    default: {
                        break;   // the chart answers nothing else here
                    }
                }
                break;
        }
    }

    // This client's SequenceNumber, written into a business message's own fields.
    static void number(std::string& packet, std::uint64_t sequence_number) {
        if (packet.size() < sizeof(protocol::simple_open_framing_header) + sizeof(protocol::message_header)) return;
        auto* bytes = reinterpret_cast<std::byte*>(packet.data());
        switch (static_cast<code>(protocol::message_header::parse(bytes + sizeof(protocol::simple_open_framing_header))->template_id.get().value())) {
            case static_cast<code>(514): {
                if (packet.size() < fixed_size<protocol::new_order_single>()) return;
                auto& message = *protocol::new_order_single::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                return;
            }
            case static_cast<code>(515): {
                if (packet.size() < fixed_size<protocol::order_cancel_replace_request>()) return;
                auto& message = *protocol::order_cancel_replace_request::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                return;
            }
            case static_cast<code>(516): {
                if (packet.size() < fixed_size<protocol::order_cancel_request>()) return;
                auto& message = *protocol::order_cancel_request::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                return;
            }
            case static_cast<code>(517): {
                if (packet.size() < fixed_size<protocol::mass_quote>()) return;
                auto& message = *protocol::mass_quote::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                return;
            }
            case static_cast<code>(518): {
                if (packet.size() < fixed_size<protocol::party_details_definition_request>()) return;
                auto& message = *protocol::party_details_definition_request::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                return;
            }
            case static_cast<code>(528): {
                if (packet.size() < fixed_size<protocol::quote_cancel>()) return;
                auto& message = *protocol::quote_cancel::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                return;
            }
            case static_cast<code>(529): {
                if (packet.size() < fixed_size<protocol::order_mass_action_request>()) return;
                auto& message = *protocol::order_mass_action_request::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                return;
            }
            case static_cast<code>(530): {
                if (packet.size() < fixed_size<protocol::order_mass_status_request>()) return;
                auto& message = *protocol::order_mass_status_request::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                return;
            }
            case static_cast<code>(533): {
                if (packet.size() < fixed_size<protocol::order_status_request>()) return;
                auto& message = *protocol::order_status_request::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                return;
            }
            case static_cast<code>(537): {
                if (packet.size() < fixed_size<protocol::party_details_list_request>()) return;
                auto& message = *protocol::party_details_list_request::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                return;
            }
            case static_cast<code>(539): {
                if (packet.size() < fixed_size<protocol::execution_ack>()) return;
                auto& message = *protocol::execution_ack::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                return;
            }
            case static_cast<code>(543): {
                if (packet.size() < fixed_size<protocol::request_for_quote>()) return;
                auto& message = *protocol::request_for_quote::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                return;
            }
            case static_cast<code>(544): {
                if (packet.size() < fixed_size<protocol::new_order_cross>()) return;
                auto& message = *protocol::new_order_cross::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                return;
            }
            case static_cast<code>(560): {
                if (packet.size() < fixed_size<protocol::security_definition_request>()) return;
                auto& message = *protocol::security_definition_request::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                return;
            }
            case static_cast<code>(566): {
                if (packet.size() < fixed_size<protocol::request_for_cross>()) return;
                auto& message = *protocol::request_for_cross::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                return;
            }
            case static_cast<code>(567): {
                if (packet.size() < fixed_size<protocol::mass_quote_request>()) return;
                auto& message = *protocol::mass_quote_request::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                return;
            }
            case static_cast<code>(568): {
                if (packet.size() < fixed_size<protocol::quote_cancel_by_set>()) return;
                auto& message = *protocol::quote_cancel_by_set::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                return;
            }
            case static_cast<code>(569): {
                if (packet.size() < fixed_size<protocol::quote_cancel_by_group>()) return;
                auto& message = *protocol::quote_cancel_by_group::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                return;
            }
            case static_cast<code>(570): {
                if (packet.size() < fixed_size<protocol::quote_cancel_by_instrument>()) return;
                auto& message = *protocol::quote_cancel_by_instrument::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                return;
            }
            default:
                return;
        }
    }

    Send send_;
    Handler& handler_;
    std::string buffer_;
    std::string pending_;
    bool busy_ = false;
    std::uint64_t uuid_ = 0;
    std::string session_;
    std::string firm_;
    std::uint64_t next_sequence_ = 1;
    std::uint64_t keep_alive_interval_ = 30000;
    ClientState state_ = ClientState::NotLoggedIn;
};

}
