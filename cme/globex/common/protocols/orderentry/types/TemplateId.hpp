#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace cme::ilink3::v9 {

// template_id
struct template_id {

    enum class enum_type : std::uint16_t {
        negotiate = 500,
        negotiation_response = 501,
        negotiation_reject = 502,
        establish = 503,
        establishment_ack = 504,
        establishment_reject = 505,
        sequence = 506,
        terminate = 507,
        retransmit_request = 508,
        retransmission = 509,
        retransmit_reject = 510,
        not_applied = 513,
        new_order_single = 514,
        order_cancel_replace_request = 515,
        order_cancel_request = 516,
        mass_quote = 517,
        party_details_definition_request = 518,
        party_details_definition_request_ack = 519,
        business_reject = 521,
        execution_report_new = 522,
        execution_report_reject = 523,
        execution_report_elimination = 524,
        execution_report_trade_outright = 525,
        execution_report_trade_spread = 526,
        execution_report_trade_spread_leg = 527,
        quote_cancel = 528,
        order_mass_action_request = 529,
        order_mass_status_request = 530,
        execution_report_modify = 531,
        execution_report_status = 532,
        order_status_request = 533,
        execution_report_cancel = 534,
        order_cancel_reject = 535,
        order_cancel_replace_reject = 536,
        party_details_list_request = 537,
        party_details_list_report = 538,
        execution_ack = 539,
        request_for_quote = 543,
        new_order_cross = 544,
        mass_quote_ack = 545,
        request_for_quote_ack = 546,
        execution_report_trade_addendum_outright = 548,
        execution_report_trade_addendum_spread = 549,
        execution_report_trade_addendum_spread_leg = 550,
        security_definition_request = 560,
        security_definition_response = 561,
        order_mass_action_report = 562,
        quote_cancel_ack = 563,
        execution_report_pending_cancel = 564,
        execution_report_pending_replace = 565,
        request_for_cross = 566,
        mass_quote_request = 567,
        quote_cancel_by_set = 568,
        quote_cancel_by_group = 569,
        quote_cancel_by_instrument = 570,
        mass_quote_response = 571
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 56> from_string_map = {{
        {"Business Reject", enum_type::business_reject},
        {"Establish", enum_type::establish},
        {"Establishment Ack", enum_type::establishment_ack},
        {"Establishment Reject", enum_type::establishment_reject},
        {"Execution Ack", enum_type::execution_ack},
        {"Execution Report Cancel", enum_type::execution_report_cancel},
        {"Execution Report Elimination", enum_type::execution_report_elimination},
        {"Execution Report Modify", enum_type::execution_report_modify},
        {"Execution Report New", enum_type::execution_report_new},
        {"Execution Report Pending Cancel", enum_type::execution_report_pending_cancel},
        {"Execution Report Pending Replace", enum_type::execution_report_pending_replace},
        {"Execution Report Reject", enum_type::execution_report_reject},
        {"Execution Report Status", enum_type::execution_report_status},
        {"Execution Report Trade Addendum Outright", enum_type::execution_report_trade_addendum_outright},
        {"Execution Report Trade Addendum Spread", enum_type::execution_report_trade_addendum_spread},
        {"Execution Report Trade Addendum Spread Leg", enum_type::execution_report_trade_addendum_spread_leg},
        {"Execution Report Trade Outright", enum_type::execution_report_trade_outright},
        {"Execution Report Trade Spread", enum_type::execution_report_trade_spread},
        {"Execution Report Trade Spread Leg", enum_type::execution_report_trade_spread_leg},
        {"Mass Quote", enum_type::mass_quote},
        {"Mass Quote Ack", enum_type::mass_quote_ack},
        {"Mass Quote Request", enum_type::mass_quote_request},
        {"Mass Quote Response", enum_type::mass_quote_response},
        {"Negotiate", enum_type::negotiate},
        {"Negotiation Reject", enum_type::negotiation_reject},
        {"Negotiation Response", enum_type::negotiation_response},
        {"New Order Cross", enum_type::new_order_cross},
        {"New Order Single", enum_type::new_order_single},
        {"Not Applied", enum_type::not_applied},
        {"Order Cancel Reject", enum_type::order_cancel_reject},
        {"Order Cancel Replace Reject", enum_type::order_cancel_replace_reject},
        {"Order Cancel Replace Request", enum_type::order_cancel_replace_request},
        {"Order Cancel Request", enum_type::order_cancel_request},
        {"Order Mass Action Report", enum_type::order_mass_action_report},
        {"Order Mass Action Request", enum_type::order_mass_action_request},
        {"Order Mass Status Request", enum_type::order_mass_status_request},
        {"Order Status Request", enum_type::order_status_request},
        {"Party Details Definition Request", enum_type::party_details_definition_request},
        {"Party Details Definition Request Ack", enum_type::party_details_definition_request_ack},
        {"Party Details List Report", enum_type::party_details_list_report},
        {"Party Details List Request", enum_type::party_details_list_request},
        {"Quote Cancel", enum_type::quote_cancel},
        {"Quote Cancel Ack", enum_type::quote_cancel_ack},
        {"Quote Cancel By Group", enum_type::quote_cancel_by_group},
        {"Quote Cancel By Instrument", enum_type::quote_cancel_by_instrument},
        {"Quote Cancel By Set", enum_type::quote_cancel_by_set},
        {"Request For Cross", enum_type::request_for_cross},
        {"Request For Quote", enum_type::request_for_quote},
        {"Request For Quote Ack", enum_type::request_for_quote_ack},
        {"Retransmission", enum_type::retransmission},
        {"Retransmit Reject", enum_type::retransmit_reject},
        {"Retransmit Request", enum_type::retransmit_request},
        {"Security Definition Request", enum_type::security_definition_request},
        {"Security Definition Response", enum_type::security_definition_response},
        {"Sequence", enum_type::sequence},
        {"Terminate", enum_type::terminate}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::negotiate: return "Negotiate";
            case enum_type::negotiation_response: return "Negotiation Response";
            case enum_type::negotiation_reject: return "Negotiation Reject";
            case enum_type::establish: return "Establish";
            case enum_type::establishment_ack: return "Establishment Ack";
            case enum_type::establishment_reject: return "Establishment Reject";
            case enum_type::sequence: return "Sequence";
            case enum_type::terminate: return "Terminate";
            case enum_type::retransmit_request: return "Retransmit Request";
            case enum_type::retransmission: return "Retransmission";
            case enum_type::retransmit_reject: return "Retransmit Reject";
            case enum_type::not_applied: return "Not Applied";
            case enum_type::new_order_single: return "New Order Single";
            case enum_type::order_cancel_replace_request: return "Order Cancel Replace Request";
            case enum_type::order_cancel_request: return "Order Cancel Request";
            case enum_type::mass_quote: return "Mass Quote";
            case enum_type::party_details_definition_request: return "Party Details Definition Request";
            case enum_type::party_details_definition_request_ack: return "Party Details Definition Request Ack";
            case enum_type::business_reject: return "Business Reject";
            case enum_type::execution_report_new: return "Execution Report New";
            case enum_type::execution_report_reject: return "Execution Report Reject";
            case enum_type::execution_report_elimination: return "Execution Report Elimination";
            case enum_type::execution_report_trade_outright: return "Execution Report Trade Outright";
            case enum_type::execution_report_trade_spread: return "Execution Report Trade Spread";
            case enum_type::execution_report_trade_spread_leg: return "Execution Report Trade Spread Leg";
            case enum_type::quote_cancel: return "Quote Cancel";
            case enum_type::order_mass_action_request: return "Order Mass Action Request";
            case enum_type::order_mass_status_request: return "Order Mass Status Request";
            case enum_type::execution_report_modify: return "Execution Report Modify";
            case enum_type::execution_report_status: return "Execution Report Status";
            case enum_type::order_status_request: return "Order Status Request";
            case enum_type::execution_report_cancel: return "Execution Report Cancel";
            case enum_type::order_cancel_reject: return "Order Cancel Reject";
            case enum_type::order_cancel_replace_reject: return "Order Cancel Replace Reject";
            case enum_type::party_details_list_request: return "Party Details List Request";
            case enum_type::party_details_list_report: return "Party Details List Report";
            case enum_type::execution_ack: return "Execution Ack";
            case enum_type::request_for_quote: return "Request For Quote";
            case enum_type::new_order_cross: return "New Order Cross";
            case enum_type::mass_quote_ack: return "Mass Quote Ack";
            case enum_type::request_for_quote_ack: return "Request For Quote Ack";
            case enum_type::execution_report_trade_addendum_outright: return "Execution Report Trade Addendum Outright";
            case enum_type::execution_report_trade_addendum_spread: return "Execution Report Trade Addendum Spread";
            case enum_type::execution_report_trade_addendum_spread_leg: return "Execution Report Trade Addendum Spread Leg";
            case enum_type::security_definition_request: return "Security Definition Request";
            case enum_type::security_definition_response: return "Security Definition Response";
            case enum_type::order_mass_action_report: return "Order Mass Action Report";
            case enum_type::quote_cancel_ack: return "Quote Cancel Ack";
            case enum_type::execution_report_pending_cancel: return "Execution Report Pending Cancel";
            case enum_type::execution_report_pending_replace: return "Execution Report Pending Replace";
            case enum_type::request_for_cross: return "Request For Cross";
            case enum_type::mass_quote_request: return "Mass Quote Request";
            case enum_type::quote_cancel_by_set: return "Quote Cancel By Set";
            case enum_type::quote_cancel_by_group: return "Quote Cancel By Group";
            case enum_type::quote_cancel_by_instrument: return "Quote Cancel By Instrument";
            case enum_type::mass_quote_response: return "Mass Quote Response";
            default: return "unknown";
        }
    }

    static constexpr std::optional<enum_type> from_string(std::string_view str) {
        auto it = std::lower_bound(
            from_string_map.begin(),
            from_string_map.end(),
            str,
            [](const auto& pair, std::string_view s) { return pair.first < s; }
        );
        if (it != from_string_map.end() && it->first == str) {
            return it->second;
        }
        return std::nullopt;
    }

    static constexpr const char* name = "template_id";
    static constexpr std::size_t size = 2;
    static constexpr bool is_optional = false;

    using result_type = required<template_id::enum_type>;
    using storage_type = result_type;

    constexpr template_id()
     : value{ enum_type{} } {}

    constexpr template_id(enum_type v)
     : value{ v } {}

    [[nodiscard]] constexpr result_type get() const {
        return result_type{value};
    }

    constexpr void set(enum_type v) {
        value = v;
    }

    constexpr void set(result_type value) {
        if (value.has_value())
            set(value.value());
        else
            set(enum_type::mass_quote_response);
    }

  protected:
    enum_type value;
};
}
