#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/BoxId.hpp"
#include "../types/BrokerId.hpp"
#include "../types/Filler.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Gateway Router Request Message
struct gateway_router_request_message {

    struct fields_type {
        nnf_orderentry::box_id box_id;
        nnf_orderentry::broker_id broker_id;
        nnf_orderentry::filler filler;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::gateway_router_request_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static gateway_router_request_message* parse(std::byte* buffer) {
        return reinterpret_cast<gateway_router_request_message*>(buffer);
    }

    // parse method const
    static const gateway_router_request_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const gateway_router_request_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(gateway_router_request_message::fields_type, box_id) == 0, "unexpected offset of gateway_router_request_message::fields_type::box_id");
static_assert(offsetof(gateway_router_request_message::fields_type, broker_id) == 2, "unexpected offset of gateway_router_request_message::fields_type::broker_id");
static_assert(offsetof(gateway_router_request_message::fields_type, filler) == 7, "unexpected offset of gateway_router_request_message::fields_type::filler");
static_assert(sizeof(gateway_router_request_message::fields_type) == 8, "unexpected sizeof gateway_router_request_message::fields_type");
static_assert(sizeof(gateway_router_request_message) == sizeof(nnf_orderentry::message_header) + 8, "unexpected sizeof gateway_router_request_message");

#pragma pack(pop)
}
