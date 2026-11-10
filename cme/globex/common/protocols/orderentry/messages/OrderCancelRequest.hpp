#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../types/OrderIdOptional.hpp"
#include "../types/PartyDetailsListReqId.hpp"
#include "../types/ManualOrderIndicator.hpp"
#include "../types/SeqNum.hpp"
#include "../types/SenderId.hpp"
#include "../types/Clordid.hpp"
#include "../types/OrderRequestId.hpp"
#include "../types/SendingTimeEpoch.hpp"
#include "../types/Location.hpp"
#include "../types/SecurityId.hpp"
#include "../types/Side.hpp"
#include "../types/LiquidityFlag.hpp"
#include "../types/OrigOrderUser.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

// Order Cancel Request
struct order_cancel_request {

    struct fields_type {
        sbe_ilink3::order_id_optional order_id_optional;
        sbe_ilink3::party_details_list_req_id party_details_list_req_id;
        sbe_ilink3::manual_order_indicator manual_order_indicator;
        sbe_ilink3::seq_num seq_num;
        sbe_ilink3::sender_id sender_id;
        sbe_ilink3::clordid clordid;
        sbe_ilink3::order_request_id order_request_id;
        sbe_ilink3::sending_time_epoch sending_time_epoch;
        sbe_ilink3::location location;
        sbe_ilink3::security_id security_id;
        sbe_ilink3::side side;
        sbe_ilink3::liquidity_flag liquidity_flag;
        sbe_ilink3::orig_order_user orig_order_user;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::order_cancel_request, schema_id(8), version(9)};

    fields_type fields;

    // parse method
    static order_cancel_request* parse(std::byte* buffer) {
        return reinterpret_cast<order_cancel_request*>(buffer);
    }

    // parse method const
    static const order_cancel_request* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_cancel_request*>(buffer);
    }

};

// layout verification
static_assert(offsetof(order_cancel_request::fields_type, order_id_optional) == 0, "unexpected offset of order_cancel_request::fields_type::order_id_optional");
static_assert(offsetof(order_cancel_request::fields_type, party_details_list_req_id) == 8, "unexpected offset of order_cancel_request::fields_type::party_details_list_req_id");
static_assert(offsetof(order_cancel_request::fields_type, manual_order_indicator) == 16, "unexpected offset of order_cancel_request::fields_type::manual_order_indicator");
static_assert(offsetof(order_cancel_request::fields_type, seq_num) == 17, "unexpected offset of order_cancel_request::fields_type::seq_num");
static_assert(offsetof(order_cancel_request::fields_type, sender_id) == 21, "unexpected offset of order_cancel_request::fields_type::sender_id");
static_assert(offsetof(order_cancel_request::fields_type, clordid) == 41, "unexpected offset of order_cancel_request::fields_type::clordid");
static_assert(offsetof(order_cancel_request::fields_type, order_request_id) == 61, "unexpected offset of order_cancel_request::fields_type::order_request_id");
static_assert(offsetof(order_cancel_request::fields_type, sending_time_epoch) == 69, "unexpected offset of order_cancel_request::fields_type::sending_time_epoch");
static_assert(offsetof(order_cancel_request::fields_type, location) == 77, "unexpected offset of order_cancel_request::fields_type::location");
static_assert(offsetof(order_cancel_request::fields_type, security_id) == 82, "unexpected offset of order_cancel_request::fields_type::security_id");
static_assert(offsetof(order_cancel_request::fields_type, side) == 86, "unexpected offset of order_cancel_request::fields_type::side");
static_assert(offsetof(order_cancel_request::fields_type, liquidity_flag) == 87, "unexpected offset of order_cancel_request::fields_type::liquidity_flag");
static_assert(offsetof(order_cancel_request::fields_type, orig_order_user) == 88, "unexpected offset of order_cancel_request::fields_type::orig_order_user");
static_assert(sizeof(order_cancel_request::fields_type) == 96, "unexpected sizeof order_cancel_request::fields_type");
static_assert(sizeof(order_cancel_request) == sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(sbe_ilink3::message_header) + 96, "unexpected sizeof order_cancel_request");

#pragma pack(pop)
}
