#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../types/PartyDetailsListReqId.hpp"
#include "../types/OrderRequestId.hpp"
#include "../types/ManualOrderIndicator.hpp"
#include "../types/SeqNum.hpp"
#include "../types/SenderId.hpp"
#include "../types/SendingTimeEpoch.hpp"
#include "../types/SecurityGroup.hpp"
#include "../types/Location.hpp"
#include "../types/SecurityIdOptional.hpp"
#include "../types/MassActionScope.hpp"
#include "../types/MarketSegmentId.hpp"
#include "../types/MassCancelRequestType.hpp"
#include "../types/SideOptional.hpp"
#include "../types/MassActionOrdTyp.hpp"
#include "../types/MassCancelTif.hpp"
#include "../types/LiquidityFlag.hpp"
#include "../types/OrigOrderUser.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

// Order Mass Action Request
struct order_mass_action_request {

    struct fields_type {
        sbe_ilink3::party_details_list_req_id party_details_list_req_id;
        sbe_ilink3::order_request_id order_request_id;
        sbe_ilink3::manual_order_indicator manual_order_indicator;
        sbe_ilink3::seq_num seq_num;
        sbe_ilink3::sender_id sender_id;
        sbe_ilink3::sending_time_epoch sending_time_epoch;
        sbe_ilink3::security_group security_group;
        sbe_ilink3::location location;
        sbe_ilink3::security_id_optional security_id_optional;
        sbe_ilink3::mass_action_scope mass_action_scope;
        sbe_ilink3::market_segment_id market_segment_id;
        sbe_ilink3::mass_cancel_request_type mass_cancel_request_type;
        sbe_ilink3::side_optional side_optional;
        sbe_ilink3::mass_action_ord_typ mass_action_ord_typ;
        sbe_ilink3::mass_cancel_tif mass_cancel_tif;
        sbe_ilink3::liquidity_flag liquidity_flag;
        sbe_ilink3::orig_order_user orig_order_user;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::order_mass_action_request, schema_id(8), version(9)};

    fields_type fields;

    // parse method
    static order_mass_action_request* parse(std::byte* buffer) {
        return reinterpret_cast<order_mass_action_request*>(buffer);
    }

    // parse method const
    static const order_mass_action_request* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_mass_action_request*>(buffer);
    }

};

// layout verification
static_assert(offsetof(order_mass_action_request::fields_type, party_details_list_req_id) == 0, "unexpected offset of order_mass_action_request::fields_type::party_details_list_req_id");
static_assert(offsetof(order_mass_action_request::fields_type, order_request_id) == 8, "unexpected offset of order_mass_action_request::fields_type::order_request_id");
static_assert(offsetof(order_mass_action_request::fields_type, manual_order_indicator) == 16, "unexpected offset of order_mass_action_request::fields_type::manual_order_indicator");
static_assert(offsetof(order_mass_action_request::fields_type, seq_num) == 17, "unexpected offset of order_mass_action_request::fields_type::seq_num");
static_assert(offsetof(order_mass_action_request::fields_type, sender_id) == 21, "unexpected offset of order_mass_action_request::fields_type::sender_id");
static_assert(offsetof(order_mass_action_request::fields_type, sending_time_epoch) == 41, "unexpected offset of order_mass_action_request::fields_type::sending_time_epoch");
static_assert(offsetof(order_mass_action_request::fields_type, security_group) == 49, "unexpected offset of order_mass_action_request::fields_type::security_group");
static_assert(offsetof(order_mass_action_request::fields_type, location) == 55, "unexpected offset of order_mass_action_request::fields_type::location");
static_assert(offsetof(order_mass_action_request::fields_type, security_id_optional) == 60, "unexpected offset of order_mass_action_request::fields_type::security_id_optional");
static_assert(offsetof(order_mass_action_request::fields_type, mass_action_scope) == 64, "unexpected offset of order_mass_action_request::fields_type::mass_action_scope");
static_assert(offsetof(order_mass_action_request::fields_type, market_segment_id) == 65, "unexpected offset of order_mass_action_request::fields_type::market_segment_id");
static_assert(offsetof(order_mass_action_request::fields_type, mass_cancel_request_type) == 66, "unexpected offset of order_mass_action_request::fields_type::mass_cancel_request_type");
static_assert(offsetof(order_mass_action_request::fields_type, side_optional) == 67, "unexpected offset of order_mass_action_request::fields_type::side_optional");
static_assert(offsetof(order_mass_action_request::fields_type, mass_action_ord_typ) == 68, "unexpected offset of order_mass_action_request::fields_type::mass_action_ord_typ");
static_assert(offsetof(order_mass_action_request::fields_type, mass_cancel_tif) == 69, "unexpected offset of order_mass_action_request::fields_type::mass_cancel_tif");
static_assert(offsetof(order_mass_action_request::fields_type, liquidity_flag) == 70, "unexpected offset of order_mass_action_request::fields_type::liquidity_flag");
static_assert(offsetof(order_mass_action_request::fields_type, orig_order_user) == 71, "unexpected offset of order_mass_action_request::fields_type::orig_order_user");
static_assert(sizeof(order_mass_action_request::fields_type) == 79, "unexpected sizeof order_mass_action_request::fields_type");
static_assert(sizeof(order_mass_action_request) == sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(sbe_ilink3::message_header) + 79, "unexpected sizeof order_mass_action_request");

#pragma pack(pop)
}
