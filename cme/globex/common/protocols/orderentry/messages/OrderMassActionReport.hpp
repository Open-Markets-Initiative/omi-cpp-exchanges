#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"
#include "../types/SeqNum.hpp"
#include "../types/Uuid.hpp"
#include "../types/SenderId.hpp"
#include "../types/PartyDetailsListReqId.hpp"
#include "../types/TransactTime.hpp"
#include "../types/SendingTimeEpoch.hpp"
#include "../types/OrderRequestId.hpp"
#include "../types/MassActionReportId.hpp"
#include "../types/SecurityGroup.hpp"
#include "../types/Location.hpp"
#include "../types/SecurityIdOptional.hpp"
#include "../types/DelayDuration.hpp"
#include "../types/MassActionResponse.hpp"
#include "../types/ManualOrderIndicator.hpp"
#include "../types/MassActionScope.hpp"
#include "../types/TotalAffectedOrders.hpp"
#include "../types/LastFragment.hpp"
#include "../types/MassActionRejectReason.hpp"
#include "../types/MarketSegmentId.hpp"
#include "../types/MassCancelRequestType.hpp"
#include "../types/SideOptional.hpp"
#include "../types/MassActionOrdTyp.hpp"
#include "../types/MassCancelTif.hpp"
#include "../types/SplitMsg.hpp"
#include "../types/LiquidityFlag.hpp"
#include "../types/PossRetransFlag.hpp"
#include "../types/DelayToTime.hpp"
#include "../types/OrigOrderUser.hpp"
#include "../types/CancelText.hpp"
#include "../types/Origclordid.hpp"
#include "../types/AffectedOrderId.hpp"
#include "../types/CxlQuantity.hpp"
#include "../structs/GroupSize.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)
struct order_mass_action_report_affected_orders_groups_entry {
    sbe_ilink3::origclordid origclordid;
    sbe_ilink3::affected_order_id affected_order_id;
    sbe_ilink3::cxl_quantity cxl_quantity;
};
#pragma pack(pop)


#pragma pack(push, 1)

// Order Mass Action Report
struct order_mass_action_report {

    struct fields_type {
        sbe_ilink3::seq_num seq_num;
        sbe_ilink3::uuid uuid;
        sbe_ilink3::sender_id sender_id;
        sbe_ilink3::party_details_list_req_id party_details_list_req_id;
        sbe_ilink3::transact_time transact_time;
        sbe_ilink3::sending_time_epoch sending_time_epoch;
        sbe_ilink3::order_request_id order_request_id;
        sbe_ilink3::mass_action_report_id mass_action_report_id;
        sbe_ilink3::security_group security_group;
        sbe_ilink3::location location;
        sbe_ilink3::security_id_optional security_id_optional;
        sbe_ilink3::delay_duration delay_duration;
        sbe_ilink3::mass_action_response mass_action_response;
        sbe_ilink3::manual_order_indicator manual_order_indicator;
        sbe_ilink3::mass_action_scope mass_action_scope;
        sbe_ilink3::total_affected_orders total_affected_orders;
        sbe_ilink3::last_fragment last_fragment;
        sbe_ilink3::mass_action_reject_reason mass_action_reject_reason;
        sbe_ilink3::market_segment_id market_segment_id;
        sbe_ilink3::mass_cancel_request_type mass_cancel_request_type;
        sbe_ilink3::side_optional side_optional;
        sbe_ilink3::mass_action_ord_typ mass_action_ord_typ;
        sbe_ilink3::mass_cancel_tif mass_cancel_tif;
        sbe_ilink3::split_msg split_msg;
        sbe_ilink3::liquidity_flag liquidity_flag;
        sbe_ilink3::poss_retrans_flag poss_retrans_flag;
        sbe_ilink3::delay_to_time delay_to_time;
        sbe_ilink3::orig_order_user orig_order_user;
        sbe_ilink3::cancel_text cancel_text;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::order_mass_action_report, schema_id(8), version(9)};

    static constexpr std::size_t max_message_size = 1280;
    static constexpr std::size_t tail_capacity = max_message_size - sizeof(sbe_ilink3::simple_open_framing_header) - sizeof(fields_type) - sizeof(sbe_ilink3::message_header);

    fields_type fields;
    std::byte tail[tail_capacity];

    // tail buffer accessors
    const std::byte* tail_begin() const { return tail; }
    const std::byte* tail_end() const {
        auto sz = header.message_length.get().value();
        return sz == sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)
            ? tail + tail_capacity
            : tail + (sz - sizeof(sbe_ilink3::simple_open_framing_header) - sizeof(fields_type) - sizeof(sbe_ilink3::message_header));
    }

    // sequential access to variable-length regions
    sbe_group_iterator<order_mass_action_report_affected_orders_groups_entry, group_size> affected_orders_groups() const {
        return { tail_begin(), tail_end() };
    }


    // parse method
    static order_mass_action_report* parse(std::byte* buffer) {
        return reinterpret_cast<order_mass_action_report*>(buffer);
    }

    // parse method const
    static const order_mass_action_report* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_mass_action_report*>(buffer);
    }

};

// layout verification
static_assert(offsetof(order_mass_action_report::fields_type, seq_num) == 0, "unexpected offset of order_mass_action_report::fields_type::seq_num");
static_assert(offsetof(order_mass_action_report::fields_type, uuid) == 4, "unexpected offset of order_mass_action_report::fields_type::uuid");
static_assert(offsetof(order_mass_action_report::fields_type, sender_id) == 12, "unexpected offset of order_mass_action_report::fields_type::sender_id");
static_assert(offsetof(order_mass_action_report::fields_type, party_details_list_req_id) == 32, "unexpected offset of order_mass_action_report::fields_type::party_details_list_req_id");
static_assert(offsetof(order_mass_action_report::fields_type, transact_time) == 40, "unexpected offset of order_mass_action_report::fields_type::transact_time");
static_assert(offsetof(order_mass_action_report::fields_type, sending_time_epoch) == 48, "unexpected offset of order_mass_action_report::fields_type::sending_time_epoch");
static_assert(offsetof(order_mass_action_report::fields_type, order_request_id) == 56, "unexpected offset of order_mass_action_report::fields_type::order_request_id");
static_assert(offsetof(order_mass_action_report::fields_type, mass_action_report_id) == 64, "unexpected offset of order_mass_action_report::fields_type::mass_action_report_id");
static_assert(offsetof(order_mass_action_report::fields_type, security_group) == 72, "unexpected offset of order_mass_action_report::fields_type::security_group");
static_assert(offsetof(order_mass_action_report::fields_type, location) == 78, "unexpected offset of order_mass_action_report::fields_type::location");
static_assert(offsetof(order_mass_action_report::fields_type, security_id_optional) == 83, "unexpected offset of order_mass_action_report::fields_type::security_id_optional");
static_assert(offsetof(order_mass_action_report::fields_type, delay_duration) == 87, "unexpected offset of order_mass_action_report::fields_type::delay_duration");
static_assert(offsetof(order_mass_action_report::fields_type, mass_action_response) == 89, "unexpected offset of order_mass_action_report::fields_type::mass_action_response");
static_assert(offsetof(order_mass_action_report::fields_type, manual_order_indicator) == 90, "unexpected offset of order_mass_action_report::fields_type::manual_order_indicator");
static_assert(offsetof(order_mass_action_report::fields_type, mass_action_scope) == 91, "unexpected offset of order_mass_action_report::fields_type::mass_action_scope");
static_assert(offsetof(order_mass_action_report::fields_type, total_affected_orders) == 92, "unexpected offset of order_mass_action_report::fields_type::total_affected_orders");
static_assert(offsetof(order_mass_action_report::fields_type, last_fragment) == 96, "unexpected offset of order_mass_action_report::fields_type::last_fragment");
static_assert(offsetof(order_mass_action_report::fields_type, mass_action_reject_reason) == 97, "unexpected offset of order_mass_action_report::fields_type::mass_action_reject_reason");
static_assert(offsetof(order_mass_action_report::fields_type, market_segment_id) == 98, "unexpected offset of order_mass_action_report::fields_type::market_segment_id");
static_assert(offsetof(order_mass_action_report::fields_type, mass_cancel_request_type) == 99, "unexpected offset of order_mass_action_report::fields_type::mass_cancel_request_type");
static_assert(offsetof(order_mass_action_report::fields_type, side_optional) == 100, "unexpected offset of order_mass_action_report::fields_type::side_optional");
static_assert(offsetof(order_mass_action_report::fields_type, mass_action_ord_typ) == 101, "unexpected offset of order_mass_action_report::fields_type::mass_action_ord_typ");
static_assert(offsetof(order_mass_action_report::fields_type, mass_cancel_tif) == 102, "unexpected offset of order_mass_action_report::fields_type::mass_cancel_tif");
static_assert(offsetof(order_mass_action_report::fields_type, split_msg) == 103, "unexpected offset of order_mass_action_report::fields_type::split_msg");
static_assert(offsetof(order_mass_action_report::fields_type, liquidity_flag) == 104, "unexpected offset of order_mass_action_report::fields_type::liquidity_flag");
static_assert(offsetof(order_mass_action_report::fields_type, poss_retrans_flag) == 105, "unexpected offset of order_mass_action_report::fields_type::poss_retrans_flag");
static_assert(offsetof(order_mass_action_report::fields_type, delay_to_time) == 106, "unexpected offset of order_mass_action_report::fields_type::delay_to_time");
static_assert(offsetof(order_mass_action_report::fields_type, orig_order_user) == 114, "unexpected offset of order_mass_action_report::fields_type::orig_order_user");
static_assert(offsetof(order_mass_action_report::fields_type, cancel_text) == 122, "unexpected offset of order_mass_action_report::fields_type::cancel_text");
static_assert(sizeof(order_mass_action_report::fields_type) == 130, "unexpected sizeof order_mass_action_report::fields_type");

#pragma pack(pop)
}

#include "details/OrderMassActionReportWriter.hpp"
#include "details/OrderMassActionReportReader.hpp"
