#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../structs/SbeGroupSupport.hpp"
#include "../types/BoxId.hpp"
#include "../types/BrokerId.hpp"
#include "../types/Filler.hpp"
#include "../types/IpAddress.hpp"
#include "../types/Port.hpp"
#include "../types/SessionKey.hpp"
#include "../types/CryptographicKey.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;


#pragma pack(push, 1)

// Gateway Router Response Message
struct gateway_router_response_message {

    struct fields_type {
        nnf_orderentry::box_id box_id;
        nnf_orderentry::broker_id broker_id;
        nnf_orderentry::filler filler;
        nnf_orderentry::ip_address ip_address;
        nnf_orderentry::port port;
        nnf_orderentry::session_key session_key;
        nnf_orderentry::cryptographic_key cryptographic_key;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::gateway_router_response_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    static constexpr std::size_t max_message_size = 1280;
    static constexpr std::size_t tail_capacity = max_message_size - sizeof(nnf_orderentry::message_header) - sizeof(fields_type);

    fields_type fields;
    std::byte tail[tail_capacity];

    // tail buffer accessors
    const std::byte* tail_begin() const { return tail; }
    const std::byte* tail_end() const {
        auto sz = header.message_length.get().value();
        return sz == sizeof(nnf_orderentry::message_header) + sizeof(fields_type)
            ? tail + tail_capacity
            : tail + (sz - sizeof(nnf_orderentry::message_header) - sizeof(fields_type));
    }

    // sequential access to variable-length regions
    sbe_var_data cryptographic_tail() const {
        return { tail_begin(), tail_end() };
    }


    // parse method
    static gateway_router_response_message* parse(std::byte* buffer) {
        return reinterpret_cast<gateway_router_response_message*>(buffer);
    }

    // parse method const
    static const gateway_router_response_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const gateway_router_response_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(gateway_router_response_message::fields_type, box_id) == 0, "unexpected offset of gateway_router_response_message::fields_type::box_id");
static_assert(offsetof(gateway_router_response_message::fields_type, broker_id) == 2, "unexpected offset of gateway_router_response_message::fields_type::broker_id");
static_assert(offsetof(gateway_router_response_message::fields_type, filler) == 7, "unexpected offset of gateway_router_response_message::fields_type::filler");
static_assert(offsetof(gateway_router_response_message::fields_type, ip_address) == 8, "unexpected offset of gateway_router_response_message::fields_type::ip_address");
static_assert(offsetof(gateway_router_response_message::fields_type, port) == 24, "unexpected offset of gateway_router_response_message::fields_type::port");
static_assert(offsetof(gateway_router_response_message::fields_type, session_key) == 28, "unexpected offset of gateway_router_response_message::fields_type::session_key");
static_assert(offsetof(gateway_router_response_message::fields_type, cryptographic_key) == 36, "unexpected offset of gateway_router_response_message::fields_type::cryptographic_key");
static_assert(sizeof(gateway_router_response_message::fields_type) == 68, "unexpected sizeof gateway_router_response_message::fields_type");

#pragma pack(pop)
}

#include "details/GatewayRouterResponseMessageWriter.hpp"
#include "details/GatewayRouterResponseMessageReader.hpp"
