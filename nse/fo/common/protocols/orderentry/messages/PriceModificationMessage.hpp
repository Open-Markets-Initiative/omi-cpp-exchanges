#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/TokenNo.hpp"
#include "../types/TraderId.hpp"
#include "../types/OrderNumber.hpp"
#include "../types/BuySell.hpp"
#include "../types/Price.hpp"
#include "../types/Volume.hpp"
#include "../types/LastModified.hpp"
#include "../types/Reference.hpp"
#include "../types/LastActivityReference.hpp"
#include "../types/Reserved24.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Price Modification Message
struct price_modification_message {

    struct fields_type {
        nnf_orderentry::token_no token_no;
        nnf_orderentry::trader_id trader_id;
        nnf_orderentry::order_number order_number;
        nnf_orderentry::buy_sell buy_sell;
        nnf_orderentry::price price;
        nnf_orderentry::volume volume;
        nnf_orderentry::last_modified last_modified;
        nnf_orderentry::reference reference;
        nnf_orderentry::last_activity_reference last_activity_reference;
        nnf_orderentry::reserved_24 reserved_24;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::price_modification_message_2013, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static price_modification_message* parse(std::byte* buffer) {
        return reinterpret_cast<price_modification_message*>(buffer);
    }

    // parse method const
    static const price_modification_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const price_modification_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(price_modification_message::fields_type, token_no) == 0, "unexpected offset of price_modification_message::fields_type::token_no");
static_assert(offsetof(price_modification_message::fields_type, trader_id) == 4, "unexpected offset of price_modification_message::fields_type::trader_id");
static_assert(offsetof(price_modification_message::fields_type, order_number) == 8, "unexpected offset of price_modification_message::fields_type::order_number");
static_assert(offsetof(price_modification_message::fields_type, buy_sell) == 16, "unexpected offset of price_modification_message::fields_type::buy_sell");
static_assert(offsetof(price_modification_message::fields_type, price) == 18, "unexpected offset of price_modification_message::fields_type::price");
static_assert(offsetof(price_modification_message::fields_type, volume) == 22, "unexpected offset of price_modification_message::fields_type::volume");
static_assert(offsetof(price_modification_message::fields_type, last_modified) == 26, "unexpected offset of price_modification_message::fields_type::last_modified");
static_assert(offsetof(price_modification_message::fields_type, reference) == 30, "unexpected offset of price_modification_message::fields_type::reference");
static_assert(offsetof(price_modification_message::fields_type, last_activity_reference) == 34, "unexpected offset of price_modification_message::fields_type::last_activity_reference");
static_assert(offsetof(price_modification_message::fields_type, reserved_24) == 42, "unexpected offset of price_modification_message::fields_type::reserved_24");
static_assert(sizeof(price_modification_message::fields_type) == 66, "unexpected sizeof price_modification_message::fields_type");
static_assert(sizeof(price_modification_message) == sizeof(nnf_orderentry::message_header) + 66, "unexpected sizeof price_modification_message");

#pragma pack(pop)
}
