#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/TokenNo.hpp"
#include "../structs/ContractDesc.hpp"
#include "../types/FillNumber.hpp"
#include "../types/FillQuantity.hpp"
#include "../types/FillPrice.hpp"
#include "../types/MktType.hpp"
#include "../types/BuyOpenClose.hpp"
#include "../types/Reserved4.hpp"
#include "../types/BuyBrokerId.hpp"
#include "../types/SellBrokerId.hpp"
#include "../types/TraderId.hpp"
#include "../types/RequestedBy.hpp"
#include "../types/SellOpenClose.hpp"
#include "../types/BuyAccountNumber.hpp"
#include "../types/SellAccountNumber.hpp"
#include "../types/Reserved24.hpp"
#include "../types/Reserved2.hpp"
#include "../types/SecondReserved2.hpp"
#include "../types/BuyPan.hpp"
#include "../types/SellPan.hpp"
#include "../types/Reserved60.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Trade Inquiry Message
struct trade_inquiry_message {

    struct fields_type {
        nnf_orderentry::token_no token_no;
        nnf_orderentry::contract_desc contract_desc;
        nnf_orderentry::fill_number fill_number;
        nnf_orderentry::fill_quantity fill_quantity;
        nnf_orderentry::fill_price fill_price;
        nnf_orderentry::mkt_type mkt_type;
        nnf_orderentry::buy_open_close buy_open_close;
        nnf_orderentry::reserved_4 reserved_4;
        nnf_orderentry::buy_broker_id buy_broker_id;
        nnf_orderentry::sell_broker_id sell_broker_id;
        nnf_orderentry::trader_id trader_id;
        nnf_orderentry::requested_by requested_by;
        nnf_orderentry::sell_open_close sell_open_close;
        nnf_orderentry::buy_account_number buy_account_number;
        nnf_orderentry::sell_account_number sell_account_number;
        nnf_orderentry::reserved_24 reserved_24;
        nnf_orderentry::reserved_2 reserved_2;
        nnf_orderentry::second_reserved_2 second_reserved_2;
        nnf_orderentry::buy_pan buy_pan;
        nnf_orderentry::sell_pan sell_pan;
        nnf_orderentry::reserved_60 reserved_60;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::trade_inquiry_message_5445, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static trade_inquiry_message* parse(std::byte* buffer) {
        return reinterpret_cast<trade_inquiry_message*>(buffer);
    }

    // parse method const
    static const trade_inquiry_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const trade_inquiry_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(trade_inquiry_message::fields_type, token_no) == 0, "unexpected offset of trade_inquiry_message::fields_type::token_no");
static_assert(offsetof(trade_inquiry_message::fields_type, contract_desc) == 4, "unexpected offset of trade_inquiry_message::fields_type::contract_desc");
static_assert(offsetof(trade_inquiry_message::fields_type, fill_number) == 32, "unexpected offset of trade_inquiry_message::fields_type::fill_number");
static_assert(offsetof(trade_inquiry_message::fields_type, fill_quantity) == 36, "unexpected offset of trade_inquiry_message::fields_type::fill_quantity");
static_assert(offsetof(trade_inquiry_message::fields_type, fill_price) == 40, "unexpected offset of trade_inquiry_message::fields_type::fill_price");
static_assert(offsetof(trade_inquiry_message::fields_type, mkt_type) == 44, "unexpected offset of trade_inquiry_message::fields_type::mkt_type");
static_assert(offsetof(trade_inquiry_message::fields_type, buy_open_close) == 45, "unexpected offset of trade_inquiry_message::fields_type::buy_open_close");
static_assert(offsetof(trade_inquiry_message::fields_type, reserved_4) == 46, "unexpected offset of trade_inquiry_message::fields_type::reserved_4");
static_assert(offsetof(trade_inquiry_message::fields_type, buy_broker_id) == 50, "unexpected offset of trade_inquiry_message::fields_type::buy_broker_id");
static_assert(offsetof(trade_inquiry_message::fields_type, sell_broker_id) == 55, "unexpected offset of trade_inquiry_message::fields_type::sell_broker_id");
static_assert(offsetof(trade_inquiry_message::fields_type, trader_id) == 60, "unexpected offset of trade_inquiry_message::fields_type::trader_id");
static_assert(offsetof(trade_inquiry_message::fields_type, requested_by) == 64, "unexpected offset of trade_inquiry_message::fields_type::requested_by");
static_assert(offsetof(trade_inquiry_message::fields_type, sell_open_close) == 65, "unexpected offset of trade_inquiry_message::fields_type::sell_open_close");
static_assert(offsetof(trade_inquiry_message::fields_type, buy_account_number) == 66, "unexpected offset of trade_inquiry_message::fields_type::buy_account_number");
static_assert(offsetof(trade_inquiry_message::fields_type, sell_account_number) == 76, "unexpected offset of trade_inquiry_message::fields_type::sell_account_number");
static_assert(offsetof(trade_inquiry_message::fields_type, reserved_24) == 86, "unexpected offset of trade_inquiry_message::fields_type::reserved_24");
static_assert(offsetof(trade_inquiry_message::fields_type, reserved_2) == 110, "unexpected offset of trade_inquiry_message::fields_type::reserved_2");
static_assert(offsetof(trade_inquiry_message::fields_type, second_reserved_2) == 112, "unexpected offset of trade_inquiry_message::fields_type::second_reserved_2");
static_assert(offsetof(trade_inquiry_message::fields_type, buy_pan) == 114, "unexpected offset of trade_inquiry_message::fields_type::buy_pan");
static_assert(offsetof(trade_inquiry_message::fields_type, sell_pan) == 124, "unexpected offset of trade_inquiry_message::fields_type::sell_pan");
static_assert(offsetof(trade_inquiry_message::fields_type, reserved_60) == 134, "unexpected offset of trade_inquiry_message::fields_type::reserved_60");
static_assert(sizeof(trade_inquiry_message::fields_type) == 194, "unexpected sizeof trade_inquiry_message::fields_type");
static_assert(sizeof(trade_inquiry_message) == sizeof(nnf_orderentry::message_header) + 194, "unexpected sizeof trade_inquiry_message");

#pragma pack(pop)
}
