#pragma once

#include <cstddef>
#include "../structs/SimpleOpenFramingHeader.hpp"
#include "../structs/MessageHeader.hpp"
#include "../types/PartyDetailsListReqId.hpp"
#include "../types/MassStatusReqId.hpp"
#include "../types/ManualOrderIndicator.hpp"
#include "../types/SeqNum.hpp"
#include "../types/SenderId.hpp"
#include "../types/SendingTimeEpoch.hpp"
#include "../types/SecurityGroup.hpp"
#include "../types/Location.hpp"
#include "../types/SecurityIdOptional.hpp"
#include "../types/MassStatusReqType.hpp"
#include "../types/OrdStatusReqType.hpp"
#include "../types/MassStatusTif.hpp"
#include "../types/MarketSegmentId.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

// Order Mass Status Request
struct order_mass_status_request {

    struct fields_type {
        sbe_ilink3::party_details_list_req_id party_details_list_req_id;
        sbe_ilink3::mass_status_req_id mass_status_req_id;
        sbe_ilink3::manual_order_indicator manual_order_indicator;
        sbe_ilink3::seq_num seq_num;
        sbe_ilink3::sender_id sender_id;
        sbe_ilink3::sending_time_epoch sending_time_epoch;
        sbe_ilink3::security_group security_group;
        sbe_ilink3::location location;
        sbe_ilink3::security_id_optional security_id_optional;
        sbe_ilink3::mass_status_req_type mass_status_req_type;
        sbe_ilink3::ord_status_req_type ord_status_req_type;
        sbe_ilink3::mass_status_tif mass_status_tif;
        sbe_ilink3::market_segment_id market_segment_id;
    };

    sbe_ilink3::simple_open_framing_header header = {std::uint16_t(sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(fields_type) + sizeof(sbe_ilink3::message_header)), encoding_type(51966)};
    sbe_ilink3::message_header message_header = {block_length(sizeof(fields_type)), template_id::enum_type::order_mass_status_request, schema_id(8), version(9)};

    fields_type fields;

    // parse method
    static order_mass_status_request* parse(std::byte* buffer) {
        return reinterpret_cast<order_mass_status_request*>(buffer);
    }

    // parse method const
    static const order_mass_status_request* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_mass_status_request*>(buffer);
    }

};

// layout verification
static_assert(offsetof(order_mass_status_request::fields_type, party_details_list_req_id) == 0, "unexpected offset of order_mass_status_request::fields_type::party_details_list_req_id");
static_assert(offsetof(order_mass_status_request::fields_type, mass_status_req_id) == 8, "unexpected offset of order_mass_status_request::fields_type::mass_status_req_id");
static_assert(offsetof(order_mass_status_request::fields_type, manual_order_indicator) == 16, "unexpected offset of order_mass_status_request::fields_type::manual_order_indicator");
static_assert(offsetof(order_mass_status_request::fields_type, seq_num) == 17, "unexpected offset of order_mass_status_request::fields_type::seq_num");
static_assert(offsetof(order_mass_status_request::fields_type, sender_id) == 21, "unexpected offset of order_mass_status_request::fields_type::sender_id");
static_assert(offsetof(order_mass_status_request::fields_type, sending_time_epoch) == 41, "unexpected offset of order_mass_status_request::fields_type::sending_time_epoch");
static_assert(offsetof(order_mass_status_request::fields_type, security_group) == 49, "unexpected offset of order_mass_status_request::fields_type::security_group");
static_assert(offsetof(order_mass_status_request::fields_type, location) == 55, "unexpected offset of order_mass_status_request::fields_type::location");
static_assert(offsetof(order_mass_status_request::fields_type, security_id_optional) == 60, "unexpected offset of order_mass_status_request::fields_type::security_id_optional");
static_assert(offsetof(order_mass_status_request::fields_type, mass_status_req_type) == 64, "unexpected offset of order_mass_status_request::fields_type::mass_status_req_type");
static_assert(offsetof(order_mass_status_request::fields_type, ord_status_req_type) == 65, "unexpected offset of order_mass_status_request::fields_type::ord_status_req_type");
static_assert(offsetof(order_mass_status_request::fields_type, mass_status_tif) == 66, "unexpected offset of order_mass_status_request::fields_type::mass_status_tif");
static_assert(offsetof(order_mass_status_request::fields_type, market_segment_id) == 67, "unexpected offset of order_mass_status_request::fields_type::market_segment_id");
static_assert(sizeof(order_mass_status_request::fields_type) == 68, "unexpected sizeof order_mass_status_request::fields_type");
static_assert(sizeof(order_mass_status_request) == sizeof(sbe_ilink3::simple_open_framing_header) + sizeof(sbe_ilink3::message_header) + 68, "unexpected sizeof order_mass_status_request");

#pragma pack(pop)
}
