#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

// transaction_code
struct transaction_code {

    enum class enum_type : std::int16_t {
        sign_on_request_in_message = 2300,
        sign_on_request_out_message = 2301,
        sign_off_request_out_message = 2321,
        system_information_in_message = 1600,
        system_information_out_message = 1601,
        gateway_router_request_message = 2400,
        gateway_router_response_message = 2401,
        sign_off_request_in_message = 2320,
        update_local_database_in_message = 7300,
        update_local_database_header_message = 7307,
        update_local_database_trailer_message = 7308,
        download_request_message = 7000,
        update_local_database_data_message = 7304,
        header_record_message = 7011,
        message_record_message = 7021,
        trailer_record_message = 7031,
        order_entry_request_message = 2000,
        order_modification_request_message = 2040,
        order_cancellation_request_message = 2070,
        order_confirmation_message = 2073,
        order_modification_confirmation_message = 2074,
        order_entry_message_2012 = 2012,
        order_entry_message_2042 = 2042,
        order_entry_message_2062 = 2062,
        order_entry_message_2072 = 2072,
        order_entry_message_2170 = 2170,
        order_entry_message_2231 = 2231,
        order_entry_message_9002 = 9002,
        price_modification_message_2013 = 2013,
        price_modification_message_20406 = 20406,
        trade_inquiry_message_5445 = 5445,
        trade_inquiry_message_5440 = 5440,
        trade_inquiry_message_5441 = 5441,
        trade_inquiry_message_2223 = 2223,
        spread_order_entry_message_2100 = 2100,
        spread_order_entry_message_2102 = 2102,
        spread_order_entry_message_2104 = 2104,
        spread_order_entry_message_2106 = 2106,
        spread_order_entry_message_2118 = 2118,
        spread_order_entry_message_2124 = 2124,
        spread_order_entry_message_2125 = 2125,
        spread_order_entry_message_2126 = 2126,
        spread_order_entry_message_2127 = 2127,
        spread_order_entry_message_2130 = 2130,
        spread_order_entry_message_2131 = 2131,
        spread_order_entry_message_2132 = 2132,
        spread_order_entry_message_2133 = 2133,
        spread_order_entry_message_2136 = 2136,
        spread_order_entry_message_2154 = 2154,
        spread_order_entry_message_2155 = 2155,
        spread_order_entry_message_2156 = 2156,
        spread_order_entry_message_9004 = 9004,
        spread_order_entry_message_20408 = 20408,
        spread_order_entry_message_20410 = 20410,
        spread_order_entry_message_20412 = 20412,
        spread_order_entry_message_20414 = 20414,
        spread_order_entry_message_20416 = 20416,
        trade_confirmation_message_2222 = 2222,
        trade_confirmation_message_2212 = 2212,
        trade_confirmation_message_2282 = 2282,
        trade_confirmation_message_2286 = 2286,
        trade_confirmation_message_2287 = 2287,
        trade_confirmation_message_2288 = 2288,
        user_order_limit_update_message = 5731,
        dealer_limit_update_message = 5733,
        spread_order_limit_update_message = 5772,
        control_message_to_trader_message = 5295,
        market_statistics_report_message = 1833,
        enhanced_market_statistics_report_message = 11833,
        market_index_report_message = 1836,
        industry_index_report_message = 1837,
        sector_index_report_message = 1838,
        spread_bhavcopy_message = 1862,
        global_indices_message = 7732,
        global_contracts_message = 7733,
        secure_box_registration_request_in_message = 23008,
        secure_box_registration_response_out_message = 23009,
        box_sign_on_request_in_message = 23000,
        box_sign_on_request_out_message = 23001,
        heartbeat_message = 23506,
        box_sign_off_message = 20322,
        contingency_broadcast_message = 5294,
        branch_order_value_limit_update_message = 5716,
        user_order_value_limit_update_message = 5730,
        normal_order_limit_update_message = 5732,
        reset_user_password_message = 5740,
        collateral_user_status_change_request_message = 5744,
        collateral_user_status_change_response_message = 5745,
        user_trade_modify_cancel_status_change_request_message = 5738,
        user_trade_modify_cancel_status_change_response_message = 5739,
        user_address_unlock_request_message = 5427,
        user_address_unlock_confirm_message = 5428,
        user_address_unlock_approve_message = 5483,
        giveup_confirmation_message_4506 = 4506,
        giveup_confirmation_message_4507 = 4507,
        order_cancellation_confirmation_message = 2075
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 96> from_string_map = {{
        {"Box Sign Off Message", enum_type::box_sign_off_message},
        {"Box Sign On Request In Message", enum_type::box_sign_on_request_in_message},
        {"Box Sign On Request Out Message", enum_type::box_sign_on_request_out_message},
        {"Branch Order Value Limit Update Message", enum_type::branch_order_value_limit_update_message},
        {"Collateral User Status Change Request Message", enum_type::collateral_user_status_change_request_message},
        {"Collateral User Status Change Response Message", enum_type::collateral_user_status_change_response_message},
        {"Contingency Broadcast Message", enum_type::contingency_broadcast_message},
        {"Control Message To Trader Message", enum_type::control_message_to_trader_message},
        {"Dealer Limit Update Message", enum_type::dealer_limit_update_message},
        {"Download Request Message", enum_type::download_request_message},
        {"Enhanced Market Statistics Report Message", enum_type::enhanced_market_statistics_report_message},
        {"Gateway Router Request Message", enum_type::gateway_router_request_message},
        {"Gateway Router Response Message", enum_type::gateway_router_response_message},
        {"Giveup Confirmation Message", enum_type::giveup_confirmation_message_4506},
        {"Giveup Confirmation Message", enum_type::giveup_confirmation_message_4507},
        {"Global Contracts Message", enum_type::global_contracts_message},
        {"Global Indices Message", enum_type::global_indices_message},
        {"Header Record Message", enum_type::header_record_message},
        {"Heartbeat Message", enum_type::heartbeat_message},
        {"Industry Index Report Message", enum_type::industry_index_report_message},
        {"Market Index Report Message", enum_type::market_index_report_message},
        {"Market Statistics Report Message", enum_type::market_statistics_report_message},
        {"Message Record Message", enum_type::message_record_message},
        {"Normal Order Limit Update Message", enum_type::normal_order_limit_update_message},
        {"Order Cancellation Confirmation Message", enum_type::order_cancellation_confirmation_message},
        {"Order Cancellation Request Message", enum_type::order_cancellation_request_message},
        {"Order Confirmation Message", enum_type::order_confirmation_message},
        {"Order Entry Message", enum_type::order_entry_message_2012},
        {"Order Entry Message", enum_type::order_entry_message_2042},
        {"Order Entry Message", enum_type::order_entry_message_2062},
        {"Order Entry Message", enum_type::order_entry_message_2072},
        {"Order Entry Message", enum_type::order_entry_message_2170},
        {"Order Entry Message", enum_type::order_entry_message_2231},
        {"Order Entry Message", enum_type::order_entry_message_9002},
        {"Order Entry Request Message", enum_type::order_entry_request_message},
        {"Order Modification Confirmation Message", enum_type::order_modification_confirmation_message},
        {"Order Modification Request Message", enum_type::order_modification_request_message},
        {"Price Modification Message", enum_type::price_modification_message_2013},
        {"Price Modification Message", enum_type::price_modification_message_20406},
        {"Reset User Password Message", enum_type::reset_user_password_message},
        {"Sector Index Report Message", enum_type::sector_index_report_message},
        {"Secure Box Registration Request In Message", enum_type::secure_box_registration_request_in_message},
        {"Secure Box Registration Response Out Message", enum_type::secure_box_registration_response_out_message},
        {"Sign Off Request In Message", enum_type::sign_off_request_in_message},
        {"Sign Off Request Out Message", enum_type::sign_off_request_out_message},
        {"Sign On Request In Message", enum_type::sign_on_request_in_message},
        {"Sign On Request Out Message", enum_type::sign_on_request_out_message},
        {"Spread Bhavcopy Message", enum_type::spread_bhavcopy_message},
        {"Spread Order Entry Message", enum_type::spread_order_entry_message_2100},
        {"Spread Order Entry Message", enum_type::spread_order_entry_message_2102},
        {"Spread Order Entry Message", enum_type::spread_order_entry_message_2104},
        {"Spread Order Entry Message", enum_type::spread_order_entry_message_2106},
        {"Spread Order Entry Message", enum_type::spread_order_entry_message_2118},
        {"Spread Order Entry Message", enum_type::spread_order_entry_message_2124},
        {"Spread Order Entry Message", enum_type::spread_order_entry_message_2125},
        {"Spread Order Entry Message", enum_type::spread_order_entry_message_2126},
        {"Spread Order Entry Message", enum_type::spread_order_entry_message_2127},
        {"Spread Order Entry Message", enum_type::spread_order_entry_message_2130},
        {"Spread Order Entry Message", enum_type::spread_order_entry_message_2131},
        {"Spread Order Entry Message", enum_type::spread_order_entry_message_2132},
        {"Spread Order Entry Message", enum_type::spread_order_entry_message_2133},
        {"Spread Order Entry Message", enum_type::spread_order_entry_message_2136},
        {"Spread Order Entry Message", enum_type::spread_order_entry_message_2154},
        {"Spread Order Entry Message", enum_type::spread_order_entry_message_2155},
        {"Spread Order Entry Message", enum_type::spread_order_entry_message_2156},
        {"Spread Order Entry Message", enum_type::spread_order_entry_message_9004},
        {"Spread Order Entry Message", enum_type::spread_order_entry_message_20408},
        {"Spread Order Entry Message", enum_type::spread_order_entry_message_20410},
        {"Spread Order Entry Message", enum_type::spread_order_entry_message_20412},
        {"Spread Order Entry Message", enum_type::spread_order_entry_message_20414},
        {"Spread Order Entry Message", enum_type::spread_order_entry_message_20416},
        {"Spread Order Limit Update Message", enum_type::spread_order_limit_update_message},
        {"System Information In Message", enum_type::system_information_in_message},
        {"System Information Out Message", enum_type::system_information_out_message},
        {"Trade Confirmation Message", enum_type::trade_confirmation_message_2222},
        {"Trade Confirmation Message", enum_type::trade_confirmation_message_2212},
        {"Trade Confirmation Message", enum_type::trade_confirmation_message_2282},
        {"Trade Confirmation Message", enum_type::trade_confirmation_message_2286},
        {"Trade Confirmation Message", enum_type::trade_confirmation_message_2287},
        {"Trade Confirmation Message", enum_type::trade_confirmation_message_2288},
        {"Trade Inquiry Message", enum_type::trade_inquiry_message_5445},
        {"Trade Inquiry Message", enum_type::trade_inquiry_message_5440},
        {"Trade Inquiry Message", enum_type::trade_inquiry_message_5441},
        {"Trade Inquiry Message", enum_type::trade_inquiry_message_2223},
        {"Trailer Record Message", enum_type::trailer_record_message},
        {"Update Local Database Data Message", enum_type::update_local_database_data_message},
        {"Update Local Database Header Message", enum_type::update_local_database_header_message},
        {"Update Local Database In Message", enum_type::update_local_database_in_message},
        {"Update Local Database Trailer Message", enum_type::update_local_database_trailer_message},
        {"User Address Unlock Approve Message", enum_type::user_address_unlock_approve_message},
        {"User Address Unlock Confirm Message", enum_type::user_address_unlock_confirm_message},
        {"User Address Unlock Request Message", enum_type::user_address_unlock_request_message},
        {"User Order Limit Update Message", enum_type::user_order_limit_update_message},
        {"User Order Value Limit Update Message", enum_type::user_order_value_limit_update_message},
        {"User Trade Modify Cancel Status Change Request Message", enum_type::user_trade_modify_cancel_status_change_request_message},
        {"User Trade Modify Cancel Status Change Response Message", enum_type::user_trade_modify_cancel_status_change_response_message}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::sign_on_request_in_message: return "Sign On Request In Message";
            case enum_type::sign_on_request_out_message: return "Sign On Request Out Message";
            case enum_type::sign_off_request_out_message: return "Sign Off Request Out Message";
            case enum_type::system_information_in_message: return "System Information In Message";
            case enum_type::system_information_out_message: return "System Information Out Message";
            case enum_type::gateway_router_request_message: return "Gateway Router Request Message";
            case enum_type::gateway_router_response_message: return "Gateway Router Response Message";
            case enum_type::sign_off_request_in_message: return "Sign Off Request In Message";
            case enum_type::update_local_database_in_message: return "Update Local Database In Message";
            case enum_type::update_local_database_header_message: return "Update Local Database Header Message";
            case enum_type::update_local_database_trailer_message: return "Update Local Database Trailer Message";
            case enum_type::download_request_message: return "Download Request Message";
            case enum_type::update_local_database_data_message: return "Update Local Database Data Message";
            case enum_type::header_record_message: return "Header Record Message";
            case enum_type::message_record_message: return "Message Record Message";
            case enum_type::trailer_record_message: return "Trailer Record Message";
            case enum_type::order_entry_request_message: return "Order Entry Request Message";
            case enum_type::order_modification_request_message: return "Order Modification Request Message";
            case enum_type::order_cancellation_request_message: return "Order Cancellation Request Message";
            case enum_type::order_confirmation_message: return "Order Confirmation Message";
            case enum_type::order_modification_confirmation_message: return "Order Modification Confirmation Message";
            case enum_type::order_entry_message_2012: return "Order Entry Message";
            case enum_type::order_entry_message_2042: return "Order Entry Message";
            case enum_type::order_entry_message_2062: return "Order Entry Message";
            case enum_type::order_entry_message_2072: return "Order Entry Message";
            case enum_type::order_entry_message_2170: return "Order Entry Message";
            case enum_type::order_entry_message_2231: return "Order Entry Message";
            case enum_type::order_entry_message_9002: return "Order Entry Message";
            case enum_type::price_modification_message_2013: return "Price Modification Message";
            case enum_type::price_modification_message_20406: return "Price Modification Message";
            case enum_type::trade_inquiry_message_5445: return "Trade Inquiry Message";
            case enum_type::trade_inquiry_message_5440: return "Trade Inquiry Message";
            case enum_type::trade_inquiry_message_5441: return "Trade Inquiry Message";
            case enum_type::trade_inquiry_message_2223: return "Trade Inquiry Message";
            case enum_type::spread_order_entry_message_2100: return "Spread Order Entry Message";
            case enum_type::spread_order_entry_message_2102: return "Spread Order Entry Message";
            case enum_type::spread_order_entry_message_2104: return "Spread Order Entry Message";
            case enum_type::spread_order_entry_message_2106: return "Spread Order Entry Message";
            case enum_type::spread_order_entry_message_2118: return "Spread Order Entry Message";
            case enum_type::spread_order_entry_message_2124: return "Spread Order Entry Message";
            case enum_type::spread_order_entry_message_2125: return "Spread Order Entry Message";
            case enum_type::spread_order_entry_message_2126: return "Spread Order Entry Message";
            case enum_type::spread_order_entry_message_2127: return "Spread Order Entry Message";
            case enum_type::spread_order_entry_message_2130: return "Spread Order Entry Message";
            case enum_type::spread_order_entry_message_2131: return "Spread Order Entry Message";
            case enum_type::spread_order_entry_message_2132: return "Spread Order Entry Message";
            case enum_type::spread_order_entry_message_2133: return "Spread Order Entry Message";
            case enum_type::spread_order_entry_message_2136: return "Spread Order Entry Message";
            case enum_type::spread_order_entry_message_2154: return "Spread Order Entry Message";
            case enum_type::spread_order_entry_message_2155: return "Spread Order Entry Message";
            case enum_type::spread_order_entry_message_2156: return "Spread Order Entry Message";
            case enum_type::spread_order_entry_message_9004: return "Spread Order Entry Message";
            case enum_type::spread_order_entry_message_20408: return "Spread Order Entry Message";
            case enum_type::spread_order_entry_message_20410: return "Spread Order Entry Message";
            case enum_type::spread_order_entry_message_20412: return "Spread Order Entry Message";
            case enum_type::spread_order_entry_message_20414: return "Spread Order Entry Message";
            case enum_type::spread_order_entry_message_20416: return "Spread Order Entry Message";
            case enum_type::trade_confirmation_message_2222: return "Trade Confirmation Message";
            case enum_type::trade_confirmation_message_2212: return "Trade Confirmation Message";
            case enum_type::trade_confirmation_message_2282: return "Trade Confirmation Message";
            case enum_type::trade_confirmation_message_2286: return "Trade Confirmation Message";
            case enum_type::trade_confirmation_message_2287: return "Trade Confirmation Message";
            case enum_type::trade_confirmation_message_2288: return "Trade Confirmation Message";
            case enum_type::user_order_limit_update_message: return "User Order Limit Update Message";
            case enum_type::dealer_limit_update_message: return "Dealer Limit Update Message";
            case enum_type::spread_order_limit_update_message: return "Spread Order Limit Update Message";
            case enum_type::control_message_to_trader_message: return "Control Message To Trader Message";
            case enum_type::market_statistics_report_message: return "Market Statistics Report Message";
            case enum_type::enhanced_market_statistics_report_message: return "Enhanced Market Statistics Report Message";
            case enum_type::market_index_report_message: return "Market Index Report Message";
            case enum_type::industry_index_report_message: return "Industry Index Report Message";
            case enum_type::sector_index_report_message: return "Sector Index Report Message";
            case enum_type::spread_bhavcopy_message: return "Spread Bhavcopy Message";
            case enum_type::global_indices_message: return "Global Indices Message";
            case enum_type::global_contracts_message: return "Global Contracts Message";
            case enum_type::secure_box_registration_request_in_message: return "Secure Box Registration Request In Message";
            case enum_type::secure_box_registration_response_out_message: return "Secure Box Registration Response Out Message";
            case enum_type::box_sign_on_request_in_message: return "Box Sign On Request In Message";
            case enum_type::box_sign_on_request_out_message: return "Box Sign On Request Out Message";
            case enum_type::heartbeat_message: return "Heartbeat Message";
            case enum_type::box_sign_off_message: return "Box Sign Off Message";
            case enum_type::contingency_broadcast_message: return "Contingency Broadcast Message";
            case enum_type::branch_order_value_limit_update_message: return "Branch Order Value Limit Update Message";
            case enum_type::user_order_value_limit_update_message: return "User Order Value Limit Update Message";
            case enum_type::normal_order_limit_update_message: return "Normal Order Limit Update Message";
            case enum_type::reset_user_password_message: return "Reset User Password Message";
            case enum_type::collateral_user_status_change_request_message: return "Collateral User Status Change Request Message";
            case enum_type::collateral_user_status_change_response_message: return "Collateral User Status Change Response Message";
            case enum_type::user_trade_modify_cancel_status_change_request_message: return "User Trade Modify Cancel Status Change Request Message";
            case enum_type::user_trade_modify_cancel_status_change_response_message: return "User Trade Modify Cancel Status Change Response Message";
            case enum_type::user_address_unlock_request_message: return "User Address Unlock Request Message";
            case enum_type::user_address_unlock_confirm_message: return "User Address Unlock Confirm Message";
            case enum_type::user_address_unlock_approve_message: return "User Address Unlock Approve Message";
            case enum_type::giveup_confirmation_message_4506: return "Giveup Confirmation Message";
            case enum_type::giveup_confirmation_message_4507: return "Giveup Confirmation Message";
            case enum_type::order_cancellation_confirmation_message: return "Order Cancellation Confirmation Message";
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

    static constexpr const char* name = "transaction_code";
    static constexpr std::size_t size = 2;
    static constexpr bool is_optional = false;

    using result_type = required<transaction_code::enum_type>;
    using storage_type = result_type;

    constexpr transaction_code()
     : value{ enum_type{} } {}

    constexpr transaction_code(enum_type v)
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
            set(enum_type::order_cancellation_confirmation_message);
    }

  protected:
    enum_type value;
};
}
