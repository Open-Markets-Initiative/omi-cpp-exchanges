#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/MessageType.hpp"
#include "../types/IndexName.hpp"
#include "../structs/MktIndex.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Market Index Report Message
struct market_index_report_message {

    struct fields_type {
        nnf_orderentry::message_type message_type;
        nnf_orderentry::index_name index_name;
        nnf_orderentry::mkt_index mkt_index;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::market_index_report_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static market_index_report_message* parse(std::byte* buffer) {
        return reinterpret_cast<market_index_report_message*>(buffer);
    }

    // parse method const
    static const market_index_report_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const market_index_report_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(market_index_report_message::fields_type, message_type) == 0, "unexpected offset of market_index_report_message::fields_type::message_type");
static_assert(offsetof(market_index_report_message::fields_type, index_name) == 1, "unexpected offset of market_index_report_message::fields_type::index_name");
static_assert(offsetof(market_index_report_message::fields_type, mkt_index) == 16, "unexpected offset of market_index_report_message::fields_type::mkt_index");
static_assert(sizeof(market_index_report_message::fields_type) == 26, "unexpected sizeof market_index_report_message::fields_type");
static_assert(sizeof(market_index_report_message) == sizeof(nnf_orderentry::message_header) + 26, "unexpected sizeof market_index_report_message");

#pragma pack(pop)
}
