#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../types/PartyDetailsListReqId.hpp"
#include "../types/OrdStatusReqId.hpp"
#include "../types/ManualOrderIndicator.hpp"
#include "../types/SeqNum.hpp"
#include "../types/SenderId.hpp"
#include "../types/OrderId.hpp"
#include "../types/SendingTimeEpoch.hpp"
#include "../types/Location.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

// Order Status Request
struct order_status_request {

    struct fields_type {
        sbe_ilink3::party_details_list_req_id party_details_list_req_id;
        sbe_ilink3::ord_status_req_id ord_status_req_id;
        sbe_ilink3::manual_order_indicator manual_order_indicator;
        sbe_ilink3::seq_num seq_num;
        sbe_ilink3::sender_id sender_id;
        sbe_ilink3::order_id order_id;
        sbe_ilink3::sending_time_epoch sending_time_epoch;
        sbe_ilink3::location location;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::order_status_request, schema_id(8), version(9)};

    fields_type fields;

    // parse method
    static order_status_request* parse(std::byte* buffer) {
        return reinterpret_cast<order_status_request*>(buffer);
    }

    // parse method const
    static const order_status_request* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_status_request*>(buffer);
    }

};

// layout verification
static_assert(offsetof(order_status_request::fields_type, party_details_list_req_id) == 0, "unexpected offset of order_status_request::fields_type::party_details_list_req_id");
static_assert(offsetof(order_status_request::fields_type, ord_status_req_id) == 8, "unexpected offset of order_status_request::fields_type::ord_status_req_id");
static_assert(offsetof(order_status_request::fields_type, manual_order_indicator) == 16, "unexpected offset of order_status_request::fields_type::manual_order_indicator");
static_assert(offsetof(order_status_request::fields_type, seq_num) == 17, "unexpected offset of order_status_request::fields_type::seq_num");
static_assert(offsetof(order_status_request::fields_type, sender_id) == 21, "unexpected offset of order_status_request::fields_type::sender_id");
static_assert(offsetof(order_status_request::fields_type, order_id) == 41, "unexpected offset of order_status_request::fields_type::order_id");
static_assert(offsetof(order_status_request::fields_type, sending_time_epoch) == 49, "unexpected offset of order_status_request::fields_type::sending_time_epoch");
static_assert(offsetof(order_status_request::fields_type, location) == 57, "unexpected offset of order_status_request::fields_type::location");
static_assert(sizeof(order_status_request::fields_type) == 62, "unexpected sizeof order_status_request::fields_type");
static_assert(sizeof(order_status_request) == sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(sbe_ilink3::message_header) + 62, "unexpected sizeof order_status_request");

#pragma pack(pop)
}
