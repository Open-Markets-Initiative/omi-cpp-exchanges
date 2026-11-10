#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include "./messages/Negotiate.hpp"
#include "./messages/Terminate.hpp"
#include "./messages/NegotiationResponse.hpp"
#include "./messages/NegotiationReject.hpp"
#include "./messages/Establish.hpp"
#include "./messages/EstablishmentAck.hpp"
#include "./messages/Sequence.hpp"
#include "./messages/RetransmitRequest.hpp"
#include "./messages/RetransmitReject.hpp"
#include "./messages/NewOrderSingle.hpp"
#include "./messages/OrderCancelReplaceRequest.hpp"
#include "./messages/OrderCancelRequest.hpp"
#include "./messages/MassQuote.hpp"
#include "./messages/PartyDetailsDefinitionRequest.hpp"
#include "./messages/QuoteCancel.hpp"
#include "./messages/OrderMassActionRequest.hpp"
#include "./messages/OrderMassStatusRequest.hpp"
#include "./messages/OrderStatusRequest.hpp"
#include "./messages/PartyDetailsListRequest.hpp"
#include "./messages/ExecutionAck.hpp"
#include "./messages/RequestForQuote.hpp"
#include "./messages/NewOrderCross.hpp"
#include "./messages/SecurityDefinitionRequest.hpp"
#include "./messages/RequestForCross.hpp"
#include "./messages/MassQuoteRequest.hpp"
#include "./messages/QuoteCancelBySet.hpp"
#include "./messages/QuoteCancelByGroup.hpp"
#include "./messages/QuoteCancelByInstrument.hpp"
#include "./messages/PartyDetailsDefinitionRequestAck.hpp"
#include "./messages/BusinessReject.hpp"
#include "./messages/ExecutionReportNew.hpp"
#include "./messages/ExecutionReportReject.hpp"
#include "./messages/ExecutionReportElimination.hpp"
#include "./messages/ExecutionReportTradeOutright.hpp"
#include "./messages/ExecutionReportTradeSpread.hpp"
#include "./messages/ExecutionReportTradeSpreadLeg.hpp"
#include "./messages/ExecutionReportModify.hpp"
#include "./messages/ExecutionReportStatus.hpp"
#include "./messages/ExecutionReportCancel.hpp"
#include "./messages/OrderCancelReject.hpp"
#include "./messages/OrderCancelReplaceReject.hpp"
#include "./messages/PartyDetailsListReport.hpp"
#include "./messages/MassQuoteAck.hpp"
#include "./messages/RequestForQuoteAck.hpp"
#include "./messages/ExecutionReportTradeAddendumOutright.hpp"
#include "./messages/ExecutionReportTradeAddendumSpread.hpp"
#include "./messages/ExecutionReportTradeAddendumSpreadLeg.hpp"
#include "./messages/SecurityDefinitionResponse.hpp"
#include "./messages/OrderMassActionReport.hpp"
#include "./messages/QuoteCancelAck.hpp"
#include "./messages/MassQuoteResponse.hpp"
#include "./structs/SimpleOpenFramingHeader.hpp"
#include "./structs/MessageHeader.hpp"

namespace sim::orderentry::ilink3 {

namespace protocol = ::cme::ilink3::v9;

// The bytes a message's fixed part takes: all of it, less the tail a variable-length one reserves.
template <typename Message>
constexpr std::size_t fixed_size() {
    if constexpr (requires { Message::tail_capacity; }) return sizeof(Message) - Message::tail_capacity;
    else return sizeof(Message);
}

// Nanoseconds since the Unix epoch, which is how Cme recommends a RequestTimestamp is stated.
inline std::uint64_t now_nanos() {
    return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}

// The type of a template id, taken down to what it is on the wire where the model enumerates it:
// each direction has its own enumeration, and these codes are read from both ends.
template <typename Code, bool = std::is_enum_v<Code>> struct wire_code { using type = Code; };
template <typename Code> struct wire_code<Code, true> { using type = std::underlying_type_t<Code>; };
using code = typename wire_code<std::remove_cvref_t<decltype(std::declval<const protocol::message_header&>().template_id.get().value())>>::type;

// Template ids, as the branch rules of Cme.Globex.iLink3.Sbe.v8.9 select them.
struct Code {
    static constexpr code login_request = static_cast<code>(500);
    static constexpr code session_logout_sent = static_cast<code>(507);
    static constexpr code login_accepted = static_cast<code>(501);
    static constexpr code login_rejected = static_cast<code>(502);
    static constexpr code establish_request = static_cast<code>(503);
    static constexpr code establish_accepted = static_cast<code>(504);
    static constexpr code session_heartbeat_received = static_cast<code>(506);
    static constexpr code session_logout_received = static_cast<code>(507);
    static constexpr code session_resend = static_cast<code>(508);
    static constexpr code session_resend_rejected = static_cast<code>(510);
    static constexpr code session_heartbeat_sent = static_cast<code>(506);
};

// The outcome a refused request reports, as the model types it.
using status = std::remove_cvref_t<decltype(std::declval<const protocol::negotiation_reject&>().fields.error_codes.get().value())>;

// The states this connection rests in, as the chart names them.
enum class State { NotNegotiated, Negotiating, Negotiated, Established, Ended };

// One connection's session, server side.
// Send:    callable(const char*, std::size_t) writing to the other end
// Handler: answers what the session cannot decide itself, and receives what it does not interpret:
// login(username, password)
// logout()
// message(packet)
template <typename Send, typename Handler>
class Session {
public:
    Session(Send send, Handler& handler) : send_(send), handler_(handler) {}

    bool established() const { return state_ == State::Established; }
    std::uint64_t uuid() const { return uuid_; }
    std::uint64_t next_sequence() const { return next_sequence_; }
    std::uint64_t expected_sequence() const { return expected_sequence_; }

    void message(const char* message, std::size_t length) {
        if (state_ != State::Established) return;
        {
            std::string out(message, length);
            stamp(out, next_sequence_, uuid_);
            send_(out.data(), out.size());
        }
        next_sequence_ = next_sequence_ + 1;
    }
    void heartbeat() {
        if (state_ != State::Established) return;
        {
            protocol::sequence reply{};
            reply.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(reply.fields.uuid.get().value())>>(uuid_));
            reply.fields.next_seq_no.set(static_cast<std::remove_cvref_t<decltype(reply.fields.next_seq_no.get().value())>>(next_sequence_));
            reply.fields.fault_tolerance_indicator.set(static_cast<std::remove_cvref_t<decltype(reply.fields.fault_tolerance_indicator.get().value())>>(1));
            send_(reinterpret_cast<const char*>(&reply), reply.header.message_length.get().value());
        }
    }
    void logout() {
        if (state_ != State::Established) return;
        {
            protocol::terminate reply{};
            reply.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(reply.fields.uuid.get().value())>>(uuid_));
            reply.fields.request_timestamp.set(static_cast<std::remove_cvref_t<decltype(reply.fields.request_timestamp.get().value())>>(now_nanos()));
            reply.fields.error_codes.set(static_cast<std::remove_cvref_t<decltype(reply.fields.error_codes.get().value())>>(0));
            send_(reinterpret_cast<const char*>(&reply), reply.header.message_length.get().value());
        }
        state_ = State::Ended;
    }

    // Bytes read from the other end: each whole frame is handled as it completes.
    void on_bytes(const char* data, std::size_t length) {
        buffer_.append(data, length);
        std::size_t at = 0;
        while (buffer_.size() - at >= sizeof(protocol::simple_open_framing_header) + sizeof(protocol::message_header)) {
            const auto* bytes = reinterpret_cast<const std::byte*>(buffer_.data() + at);
            const auto& header = *protocol::simple_open_framing_header::parse(bytes);
            const std::size_t whole = header.message_length.get().value();
            // A frame shorter than its own headers says nothing a reader can step past.
            if (whole < sizeof(protocol::simple_open_framing_header) + sizeof(protocol::message_header)) { buffer_.clear(); return; }
            if (buffer_.size() - at < whole) break;
            on_packet(static_cast<code>(protocol::message_header::parse(bytes + sizeof(protocol::simple_open_framing_header))->template_id.get().value()), buffer_.data() + at, whole);
            at += whole;
        }
        buffer_.erase(0, at);
    }


private:
    // A frame from the other end, answered by what the chart allows in this state.
    void on_packet(code value, const char* packet, std::size_t length) {
        switch (state_) {
            case State::NotNegotiated:
                switch (value) {
                    case Code::login_request: {
                        if (length < fixed_size<protocol::negotiate>()) break;
                        const auto& request = *protocol::negotiate::parse(reinterpret_cast<const std::byte*>(packet));
                        {
                            negotiating(request);
                            break;
                        }
                    }
                    default: {
                        {
                            {
                                protocol::terminate reply{};
                                reply.fields.request_timestamp.set(static_cast<std::remove_cvref_t<decltype(reply.fields.request_timestamp.get().value())>>(now_nanos()));
                                reply.fields.error_codes.set(static_cast<std::remove_cvref_t<decltype(reply.fields.error_codes.get().value())>>(2));
                                send_(reinterpret_cast<const char*>(&reply), reply.header.message_length.get().value());
                            }
                            state_ = State::Ended;
                            break;
                        }
                    }
                }
                break;
            case State::Negotiating:
                break;   // nothing the chart answers here
            case State::Negotiated:
                switch (value) {
                    case Code::establish_request: {
                        if (length < fixed_size<protocol::establish>()) break;
                        const auto& request = *protocol::establish::parse(reinterpret_cast<const std::byte*>(packet));
                        if (request.fields.uuid.get().value() != uuid_) {
                            {
                                protocol::terminate reply{};
                                reply.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(reply.fields.uuid.get().value())>>(uuid_));
                                reply.fields.request_timestamp.set(static_cast<std::remove_cvref_t<decltype(reply.fields.request_timestamp.get().value())>>(request.fields.request_timestamp.get().value()));
                                reply.fields.error_codes.set(static_cast<std::remove_cvref_t<decltype(reply.fields.error_codes.get().value())>>(13));
                                send_(reinterpret_cast<const char*>(&reply), reply.header.message_length.get().value());
                            }
                            state_ = State::Ended;
                            break;
                        }
                        {
                            expected_sequence_ = request.fields.next_seq_no.get().value();
                            {
                                protocol::establishment_ack reply{};
                                reply.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(reply.fields.uuid.get().value())>>(uuid_));
                                reply.fields.request_timestamp.set(static_cast<std::remove_cvref_t<decltype(reply.fields.request_timestamp.get().value())>>(request.fields.request_timestamp.get().value()));
                                reply.fields.next_seq_no.set(static_cast<std::remove_cvref_t<decltype(reply.fields.next_seq_no.get().value())>>(next_sequence_));
                                reply.fields.previous_seq_no.set(static_cast<std::remove_cvref_t<decltype(reply.fields.previous_seq_no.get().value())>>(0));
                                reply.fields.previous_uuid.set(static_cast<std::remove_cvref_t<decltype(reply.fields.previous_uuid.get().value())>>(0));
                                reply.fields.keep_alive_interval.set(static_cast<std::remove_cvref_t<decltype(reply.fields.keep_alive_interval.get().value())>>(request.fields.keep_alive_interval.get().value()));
                                reply.fields.fault_tolerance_indicator.set(static_cast<std::remove_cvref_t<decltype(reply.fields.fault_tolerance_indicator.get().value())>>(1));
                                send_(reinterpret_cast<const char*>(&reply), reply.header.message_length.get().value());
                            }
                            state_ = State::Established;
                            break;
                        }
                    }
                    case Code::login_request: {
                        if (length < fixed_size<protocol::negotiate>()) break;
                        const auto& request = *protocol::negotiate::parse(reinterpret_cast<const std::byte*>(packet));
                        {
                            {
                                protocol::terminate reply{};
                                reply.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(reply.fields.uuid.get().value())>>(uuid_));
                                reply.fields.request_timestamp.set(static_cast<std::remove_cvref_t<decltype(reply.fields.request_timestamp.get().value())>>(request.fields.request_timestamp.get().value()));
                                reply.fields.error_codes.set(static_cast<std::remove_cvref_t<decltype(reply.fields.error_codes.get().value())>>(4));
                                send_(reinterpret_cast<const char*>(&reply), reply.header.message_length.get().value());
                            }
                            state_ = State::Ended;
                            break;
                        }
                    }
                    default: {
                        {
                            {
                                protocol::terminate reply{};
                                reply.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(reply.fields.uuid.get().value())>>(uuid_));
                                reply.fields.request_timestamp.set(static_cast<std::remove_cvref_t<decltype(reply.fields.request_timestamp.get().value())>>(now_nanos()));
                                reply.fields.error_codes.set(static_cast<std::remove_cvref_t<decltype(reply.fields.error_codes.get().value())>>(3));
                                send_(reinterpret_cast<const char*>(&reply), reply.header.message_length.get().value());
                            }
                            state_ = State::Ended;
                            break;
                        }
                    }
                }
                break;
            case State::Established:
                switch (value) {
                    case Code::session_heartbeat_received: {
                        if (length < fixed_size<protocol::sequence>()) break;
                        const auto& request = *protocol::sequence::parse(reinterpret_cast<const std::byte*>(packet));
                        {
                            expected_sequence_ = request.fields.next_seq_no.get().value();
                            break;
                        }
                    }
                    case Code::session_logout_received: {
                        if (length < fixed_size<protocol::terminate>()) break;
                        const auto& request = *protocol::terminate::parse(reinterpret_cast<const std::byte*>(packet));
                        {
                            {
                                protocol::terminate reply{};
                                reply.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(reply.fields.uuid.get().value())>>(uuid_));
                                reply.fields.request_timestamp.set(static_cast<std::remove_cvref_t<decltype(reply.fields.request_timestamp.get().value())>>(request.fields.request_timestamp.get().value()));
                                reply.fields.error_codes.set(static_cast<std::remove_cvref_t<decltype(reply.fields.error_codes.get().value())>>(0));
                                send_(reinterpret_cast<const char*>(&reply), reply.header.message_length.get().value());
                            }
                            handler_.logout();
                            state_ = State::Ended;
                            break;
                        }
                    }
                    case Code::session_resend: {
                        if (length < fixed_size<protocol::retransmit_request>()) break;
                        const auto& request = *protocol::retransmit_request::parse(reinterpret_cast<const std::byte*>(packet));
                        {
                            {
                                protocol::retransmit_reject reply{};
                                reply.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(reply.fields.uuid.get().value())>>(request.fields.uuid.get().value()));
                                reply.fields.request_timestamp.set(static_cast<std::remove_cvref_t<decltype(reply.fields.request_timestamp.get().value())>>(request.fields.request_timestamp.get().value()));
                                reply.fields.error_codes.set(static_cast<std::remove_cvref_t<decltype(reply.fields.error_codes.get().value())>>(0));
                                send_(reinterpret_cast<const char*>(&reply), reply.header.message_length.get().value());
                            }
                            break;
                        }
                    }
                    case Code::login_request: {
                        if (length < fixed_size<protocol::negotiate>()) break;
                        const auto& request = *protocol::negotiate::parse(reinterpret_cast<const std::byte*>(packet));
                        {
                            {
                                protocol::terminate reply{};
                                reply.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(reply.fields.uuid.get().value())>>(uuid_));
                                reply.fields.request_timestamp.set(static_cast<std::remove_cvref_t<decltype(reply.fields.request_timestamp.get().value())>>(request.fields.request_timestamp.get().value()));
                                reply.fields.error_codes.set(static_cast<std::remove_cvref_t<decltype(reply.fields.error_codes.get().value())>>(4));
                                send_(reinterpret_cast<const char*>(&reply), reply.header.message_length.get().value());
                            }
                            state_ = State::Ended;
                            break;
                        }
                    }
                    case Code::establish_request: {
                        if (length < fixed_size<protocol::establish>()) break;
                        const auto& request = *protocol::establish::parse(reinterpret_cast<const std::byte*>(packet));
                        {
                            {
                                protocol::terminate reply{};
                                reply.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(reply.fields.uuid.get().value())>>(uuid_));
                                reply.fields.request_timestamp.set(static_cast<std::remove_cvref_t<decltype(reply.fields.request_timestamp.get().value())>>(request.fields.request_timestamp.get().value()));
                                reply.fields.error_codes.set(static_cast<std::remove_cvref_t<decltype(reply.fields.error_codes.get().value())>>(6));
                                send_(reinterpret_cast<const char*>(&reply), reply.header.message_length.get().value());
                            }
                            state_ = State::Ended;
                            break;
                        }
                    }
                    default: {
                        if (length <= sizeof(protocol::simple_open_framing_header) + sizeof(protocol::message_header)) break;   // a frame with no message of its own says nothing
                        const auto read_sequence_number = read_sequence_number_of(value, packet, length);
                        if (!read_sequence_number) break;   // not a message the session numbers
                        const std::uint64_t sequence_number = *read_sequence_number;
                        if (sequence_number < expected_sequence_) {
                            {
                                protocol::terminate reply{};
                                reply.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(reply.fields.uuid.get().value())>>(uuid_));
                                reply.fields.request_timestamp.set(static_cast<std::remove_cvref_t<decltype(reply.fields.request_timestamp.get().value())>>(now_nanos()));
                                reply.fields.error_codes.set(static_cast<std::remove_cvref_t<decltype(reply.fields.error_codes.get().value())>>(11));
                                send_(reinterpret_cast<const char*>(&reply), reply.header.message_length.get().value());
                            }
                            state_ = State::Ended;
                            break;
                        }
                        {
                            expected_sequence_ = sequence_number;
                            expected_sequence_ = expected_sequence_ + 1;
                            handler_.message(std::string_view(packet, length));
                            break;
                        }
                    }
                }
                break;
            case State::Ended:
                break;   // nothing the chart answers here
        }
    }

    void negotiating(const protocol::negotiate& request) {
        const auto refused = handler_.login(request.fields.session.get_trimmed().value(), request.fields.firm.get_trimmed().value());
        if (!refused) {
            {
                uuid_ = request.fields.uuid.get().value();
                {
                    protocol::negotiation_response reply{};
                    reply.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(reply.fields.uuid.get().value())>>(request.fields.uuid.get().value()));
                    reply.fields.request_timestamp.set(static_cast<std::remove_cvref_t<decltype(reply.fields.request_timestamp.get().value())>>(request.fields.request_timestamp.get().value()));
                    reply.fields.fault_tolerance_indicator.set(static_cast<std::remove_cvref_t<decltype(reply.fields.fault_tolerance_indicator.get().value())>>(1));
                    reply.fields.previous_seq_no.set(static_cast<std::remove_cvref_t<decltype(reply.fields.previous_seq_no.get().value())>>(0));
                    reply.fields.previous_uuid.set(static_cast<std::remove_cvref_t<decltype(reply.fields.previous_uuid.get().value())>>(0));
                    // Credentials is sent empty, its length zero.
                    const auto written = protocol::start_group_write(reply).credentials_and_finish({});
                    send_(reinterpret_cast<const char*>(written.data()), written.size());
                }
                state_ = State::Negotiated;
                return;
            }
        }
        if (refused) {
            {
                {
                    protocol::negotiation_reject reply{};
                    reply.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(reply.fields.uuid.get().value())>>(request.fields.uuid.get().value()));
                    reply.fields.request_timestamp.set(static_cast<std::remove_cvref_t<decltype(reply.fields.request_timestamp.get().value())>>(request.fields.request_timestamp.get().value()));
                    reply.fields.fault_tolerance_indicator.set(static_cast<std::remove_cvref_t<decltype(reply.fields.fault_tolerance_indicator.get().value())>>(1));
                    reply.fields.error_codes.set(static_cast<std::remove_cvref_t<decltype(reply.fields.error_codes.get().value())>>(*refused));
                    send_(reinterpret_cast<const char*>(&reply), reply.header.message_length.get().value());
                }
                state_ = State::NotNegotiated;
                return;
            }
        }
    }

    // The SequenceNumber a business message carries in its own body, by the template id that selects it.
    static std::optional<std::uint64_t> read_sequence_number_of(code value, const char* packet, std::size_t length) {
        const auto* bytes = reinterpret_cast<const std::byte*>(packet);
        switch (value) {
            case static_cast<code>(514):
                if (length < fixed_size<protocol::new_order_single>()) return std::nullopt;
                return static_cast<std::uint64_t>(protocol::new_order_single::parse(bytes)->fields.seq_num.get().value());
            case static_cast<code>(515):
                if (length < fixed_size<protocol::order_cancel_replace_request>()) return std::nullopt;
                return static_cast<std::uint64_t>(protocol::order_cancel_replace_request::parse(bytes)->fields.seq_num.get().value());
            case static_cast<code>(516):
                if (length < fixed_size<protocol::order_cancel_request>()) return std::nullopt;
                return static_cast<std::uint64_t>(protocol::order_cancel_request::parse(bytes)->fields.seq_num.get().value());
            case static_cast<code>(517):
                if (length < fixed_size<protocol::mass_quote>()) return std::nullopt;
                return static_cast<std::uint64_t>(protocol::mass_quote::parse(bytes)->fields.seq_num.get().value());
            case static_cast<code>(518):
                if (length < fixed_size<protocol::party_details_definition_request>()) return std::nullopt;
                return static_cast<std::uint64_t>(protocol::party_details_definition_request::parse(bytes)->fields.seq_num.get().value());
            case static_cast<code>(528):
                if (length < fixed_size<protocol::quote_cancel>()) return std::nullopt;
                return static_cast<std::uint64_t>(protocol::quote_cancel::parse(bytes)->fields.seq_num.get().value());
            case static_cast<code>(529):
                if (length < fixed_size<protocol::order_mass_action_request>()) return std::nullopt;
                return static_cast<std::uint64_t>(protocol::order_mass_action_request::parse(bytes)->fields.seq_num.get().value());
            case static_cast<code>(530):
                if (length < fixed_size<protocol::order_mass_status_request>()) return std::nullopt;
                return static_cast<std::uint64_t>(protocol::order_mass_status_request::parse(bytes)->fields.seq_num.get().value());
            case static_cast<code>(533):
                if (length < fixed_size<protocol::order_status_request>()) return std::nullopt;
                return static_cast<std::uint64_t>(protocol::order_status_request::parse(bytes)->fields.seq_num.get().value());
            case static_cast<code>(537):
                if (length < fixed_size<protocol::party_details_list_request>()) return std::nullopt;
                return static_cast<std::uint64_t>(protocol::party_details_list_request::parse(bytes)->fields.seq_num.get().value());
            case static_cast<code>(539):
                if (length < fixed_size<protocol::execution_ack>()) return std::nullopt;
                return static_cast<std::uint64_t>(protocol::execution_ack::parse(bytes)->fields.seq_num.get().value());
            case static_cast<code>(543):
                if (length < fixed_size<protocol::request_for_quote>()) return std::nullopt;
                return static_cast<std::uint64_t>(protocol::request_for_quote::parse(bytes)->fields.seq_num.get().value());
            case static_cast<code>(544):
                if (length < fixed_size<protocol::new_order_cross>()) return std::nullopt;
                return static_cast<std::uint64_t>(protocol::new_order_cross::parse(bytes)->fields.seq_num.get().value());
            case static_cast<code>(560):
                if (length < fixed_size<protocol::security_definition_request>()) return std::nullopt;
                return static_cast<std::uint64_t>(protocol::security_definition_request::parse(bytes)->fields.seq_num.get().value());
            case static_cast<code>(566):
                if (length < fixed_size<protocol::request_for_cross>()) return std::nullopt;
                return static_cast<std::uint64_t>(protocol::request_for_cross::parse(bytes)->fields.seq_num.get().value());
            case static_cast<code>(567):
                if (length < fixed_size<protocol::mass_quote_request>()) return std::nullopt;
                return static_cast<std::uint64_t>(protocol::mass_quote_request::parse(bytes)->fields.seq_num.get().value());
            case static_cast<code>(568):
                if (length < fixed_size<protocol::quote_cancel_by_set>()) return std::nullopt;
                return static_cast<std::uint64_t>(protocol::quote_cancel_by_set::parse(bytes)->fields.seq_num.get().value());
            case static_cast<code>(569):
                if (length < fixed_size<protocol::quote_cancel_by_group>()) return std::nullopt;
                return static_cast<std::uint64_t>(protocol::quote_cancel_by_group::parse(bytes)->fields.seq_num.get().value());
            case static_cast<code>(570):
                if (length < fixed_size<protocol::quote_cancel_by_instrument>()) return std::nullopt;
                return static_cast<std::uint64_t>(protocol::quote_cancel_by_instrument::parse(bytes)->fields.seq_num.get().value());
            default:
                return std::nullopt;
        }
    }

    // This side's SequenceNumber and Uuid, written into a business message's own fields.
    static void stamp(std::string& packet, std::uint64_t sequence_number, std::uint64_t uuid) {
        if (packet.size() < sizeof(protocol::simple_open_framing_header) + sizeof(protocol::message_header)) return;
        auto* bytes = reinterpret_cast<std::byte*>(packet.data());
        switch (static_cast<code>(protocol::message_header::parse(bytes + sizeof(protocol::simple_open_framing_header))->template_id.get().value())) {
            case static_cast<code>(519): {
                if (packet.size() < fixed_size<protocol::party_details_definition_request_ack>()) return;
                auto& message = *protocol::party_details_definition_request_ack::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                message.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(message.fields.uuid.get().value())>>(uuid));
                return;
            }
            case static_cast<code>(521): {
                if (packet.size() < fixed_size<protocol::business_reject>()) return;
                auto& message = *protocol::business_reject::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                message.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(message.fields.uuid.get().value())>>(uuid));
                return;
            }
            case static_cast<code>(522): {
                if (packet.size() < fixed_size<protocol::execution_report_new>()) return;
                auto& message = *protocol::execution_report_new::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                message.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(message.fields.uuid.get().value())>>(uuid));
                return;
            }
            case static_cast<code>(523): {
                if (packet.size() < fixed_size<protocol::execution_report_reject>()) return;
                auto& message = *protocol::execution_report_reject::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                message.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(message.fields.uuid.get().value())>>(uuid));
                return;
            }
            case static_cast<code>(524): {
                if (packet.size() < fixed_size<protocol::execution_report_elimination>()) return;
                auto& message = *protocol::execution_report_elimination::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                message.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(message.fields.uuid.get().value())>>(uuid));
                return;
            }
            case static_cast<code>(525): {
                if (packet.size() < fixed_size<protocol::execution_report_trade_outright>()) return;
                auto& message = *protocol::execution_report_trade_outright::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                message.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(message.fields.uuid.get().value())>>(uuid));
                return;
            }
            case static_cast<code>(526): {
                if (packet.size() < fixed_size<protocol::execution_report_trade_spread>()) return;
                auto& message = *protocol::execution_report_trade_spread::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                message.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(message.fields.uuid.get().value())>>(uuid));
                return;
            }
            case static_cast<code>(527): {
                if (packet.size() < fixed_size<protocol::execution_report_trade_spread_leg>()) return;
                auto& message = *protocol::execution_report_trade_spread_leg::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                message.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(message.fields.uuid.get().value())>>(uuid));
                return;
            }
            case static_cast<code>(531): {
                if (packet.size() < fixed_size<protocol::execution_report_modify>()) return;
                auto& message = *protocol::execution_report_modify::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                message.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(message.fields.uuid.get().value())>>(uuid));
                return;
            }
            case static_cast<code>(532): {
                if (packet.size() < fixed_size<protocol::execution_report_status>()) return;
                auto& message = *protocol::execution_report_status::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                message.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(message.fields.uuid.get().value())>>(uuid));
                return;
            }
            case static_cast<code>(534): {
                if (packet.size() < fixed_size<protocol::execution_report_cancel>()) return;
                auto& message = *protocol::execution_report_cancel::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                message.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(message.fields.uuid.get().value())>>(uuid));
                return;
            }
            case static_cast<code>(535): {
                if (packet.size() < fixed_size<protocol::order_cancel_reject>()) return;
                auto& message = *protocol::order_cancel_reject::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                message.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(message.fields.uuid.get().value())>>(uuid));
                return;
            }
            case static_cast<code>(536): {
                if (packet.size() < fixed_size<protocol::order_cancel_replace_reject>()) return;
                auto& message = *protocol::order_cancel_replace_reject::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                message.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(message.fields.uuid.get().value())>>(uuid));
                return;
            }
            case static_cast<code>(538): {
                if (packet.size() < fixed_size<protocol::party_details_list_report>()) return;
                auto& message = *protocol::party_details_list_report::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                message.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(message.fields.uuid.get().value())>>(uuid));
                return;
            }
            case static_cast<code>(545): {
                if (packet.size() < fixed_size<protocol::mass_quote_ack>()) return;
                auto& message = *protocol::mass_quote_ack::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                message.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(message.fields.uuid.get().value())>>(uuid));
                return;
            }
            case static_cast<code>(546): {
                if (packet.size() < fixed_size<protocol::request_for_quote_ack>()) return;
                auto& message = *protocol::request_for_quote_ack::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                message.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(message.fields.uuid.get().value())>>(uuid));
                return;
            }
            case static_cast<code>(548): {
                if (packet.size() < fixed_size<protocol::execution_report_trade_addendum_outright>()) return;
                auto& message = *protocol::execution_report_trade_addendum_outright::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                message.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(message.fields.uuid.get().value())>>(uuid));
                return;
            }
            case static_cast<code>(549): {
                if (packet.size() < fixed_size<protocol::execution_report_trade_addendum_spread>()) return;
                auto& message = *protocol::execution_report_trade_addendum_spread::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                message.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(message.fields.uuid.get().value())>>(uuid));
                return;
            }
            case static_cast<code>(550): {
                if (packet.size() < fixed_size<protocol::execution_report_trade_addendum_spread_leg>()) return;
                auto& message = *protocol::execution_report_trade_addendum_spread_leg::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                message.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(message.fields.uuid.get().value())>>(uuid));
                return;
            }
            case static_cast<code>(561): {
                if (packet.size() < fixed_size<protocol::security_definition_response>()) return;
                auto& message = *protocol::security_definition_response::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                message.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(message.fields.uuid.get().value())>>(uuid));
                return;
            }
            case static_cast<code>(562): {
                if (packet.size() < fixed_size<protocol::order_mass_action_report>()) return;
                auto& message = *protocol::order_mass_action_report::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                message.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(message.fields.uuid.get().value())>>(uuid));
                return;
            }
            case static_cast<code>(563): {
                if (packet.size() < fixed_size<protocol::quote_cancel_ack>()) return;
                auto& message = *protocol::quote_cancel_ack::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                message.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(message.fields.uuid.get().value())>>(uuid));
                return;
            }
            case static_cast<code>(571): {
                if (packet.size() < fixed_size<protocol::mass_quote_response>()) return;
                auto& message = *protocol::mass_quote_response::parse(bytes);
                message.fields.seq_num.set(static_cast<std::remove_cvref_t<decltype(message.fields.seq_num.get().value())>>(sequence_number));
                message.fields.uuid.set(static_cast<std::remove_cvref_t<decltype(message.fields.uuid.get().value())>>(uuid));
                return;
            }
            default:
                return;
        }
    }

    std::uint64_t uuid_ = 0;
    std::uint64_t next_sequence_ = 1;
    std::uint64_t expected_sequence_ = 1;
    Send send_;
    Handler& handler_;
    std::string buffer_;
    State state_ = State::NotNegotiated;
};

}
