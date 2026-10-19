#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/ResponseOrderNumber.hpp"
#include "../types/BrokerId.hpp"
#include "../types/Reserved1.hpp"
#include "../types/TraderNumber.hpp"
#include "../types/AccountNumber.hpp"
#include "../types/BuySellIndicator.hpp"
#include "../types/OriginalVolume.hpp"
#include "../types/DisclosedVolume.hpp"
#include "../types/RemainingVolume.hpp"
#include "../types/DisclosedVolumeRemaining.hpp"
#include "../types/Price.hpp"
#include "../types/StOrderFlags.hpp"
#include "../types/GoodTillDate.hpp"
#include "../types/FillNumber.hpp"
#include "../types/FillQuantity.hpp"
#include "../types/FillPrice.hpp"
#include "../types/VolumeFilledToday.hpp"
#include "../types/ActivityType.hpp"
#include "../types/ActivityTime.hpp"
#include "../types/CounterTraderOrderNumber.hpp"
#include "../types/CounterBrokerId.hpp"
#include "../types/SecondReserved1.hpp"
#include "../types/Token.hpp"
#include "../structs/ContractDesc.hpp"
#include "../types/OpenClose.hpp"
#include "../types/OldOpenClose.hpp"
#include "../types/TradeBookType.hpp"
#include "../types/ThirdReserved1.hpp"
#include "../types/Reserved4.hpp"
#include "../types/OldAccountNumber.hpp"
#include "../types/Participant.hpp"
#include "../types/OldParticipant.hpp"
#include "../types/AdditionalOrderFlags.hpp"
#include "../types/FourthReserved1.hpp"
#include "../types/FifthReserved1.hpp"
#include "../types/SixthReserved1.hpp"
#include "../types/Pan.hpp"
#include "../types/OldPan.hpp"
#include "../types/AlgoId.hpp"
#include "../types/Reserved2.hpp"
#include "../types/LastActivityReference.hpp"
#include "../types/Reserved52.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Trade Confirmation Message
struct trade_confirmation_message {

    struct fields_type {
        nnf_orderentry::response_order_number response_order_number;
        nnf_orderentry::broker_id broker_id;
        nnf_orderentry::reserved_1 reserved_1;
        nnf_orderentry::trader_number trader_number;
        nnf_orderentry::account_number account_number;
        nnf_orderentry::buy_sell_indicator buy_sell_indicator;
        nnf_orderentry::original_volume original_volume;
        nnf_orderentry::disclosed_volume disclosed_volume;
        nnf_orderentry::remaining_volume remaining_volume;
        nnf_orderentry::disclosed_volume_remaining disclosed_volume_remaining;
        nnf_orderentry::price price;
        nnf_orderentry::st_order_flags st_order_flags;
        nnf_orderentry::good_till_date good_till_date;
        nnf_orderentry::fill_number fill_number;
        nnf_orderentry::fill_quantity fill_quantity;
        nnf_orderentry::fill_price fill_price;
        nnf_orderentry::volume_filled_today volume_filled_today;
        nnf_orderentry::activity_type activity_type;
        nnf_orderentry::activity_time activity_time;
        nnf_orderentry::counter_trader_order_number counter_trader_order_number;
        nnf_orderentry::counter_broker_id counter_broker_id;
        nnf_orderentry::second_reserved_1 second_reserved_1;
        nnf_orderentry::token token;
        nnf_orderentry::contract_desc contract_desc;
        nnf_orderentry::open_close open_close;
        nnf_orderentry::old_open_close old_open_close;
        nnf_orderentry::trade_book_type trade_book_type;
        nnf_orderentry::third_reserved_1 third_reserved_1;
        nnf_orderentry::reserved_4 reserved_4;
        nnf_orderentry::old_account_number old_account_number;
        nnf_orderentry::participant participant;
        nnf_orderentry::old_participant old_participant;
        nnf_orderentry::additional_order_flags additional_order_flags;
        nnf_orderentry::fourth_reserved_1 fourth_reserved_1;
        nnf_orderentry::fifth_reserved_1 fifth_reserved_1;
        nnf_orderentry::sixth_reserved_1 sixth_reserved_1;
        nnf_orderentry::pan pan;
        nnf_orderentry::old_pan old_pan;
        nnf_orderentry::algo_id algo_id;
        nnf_orderentry::reserved_2 reserved_2;
        nnf_orderentry::last_activity_reference last_activity_reference;
        nnf_orderentry::reserved_52 reserved_52;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::trade_confirmation_message_2222, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static trade_confirmation_message* parse(std::byte* buffer) {
        return reinterpret_cast<trade_confirmation_message*>(buffer);
    }

    // parse method const
    static const trade_confirmation_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const trade_confirmation_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(trade_confirmation_message::fields_type, response_order_number) == 0, "unexpected offset of trade_confirmation_message::fields_type::response_order_number");
static_assert(offsetof(trade_confirmation_message::fields_type, broker_id) == 8, "unexpected offset of trade_confirmation_message::fields_type::broker_id");
static_assert(offsetof(trade_confirmation_message::fields_type, reserved_1) == 13, "unexpected offset of trade_confirmation_message::fields_type::reserved_1");
static_assert(offsetof(trade_confirmation_message::fields_type, trader_number) == 14, "unexpected offset of trade_confirmation_message::fields_type::trader_number");
static_assert(offsetof(trade_confirmation_message::fields_type, account_number) == 18, "unexpected offset of trade_confirmation_message::fields_type::account_number");
static_assert(offsetof(trade_confirmation_message::fields_type, buy_sell_indicator) == 28, "unexpected offset of trade_confirmation_message::fields_type::buy_sell_indicator");
static_assert(offsetof(trade_confirmation_message::fields_type, original_volume) == 30, "unexpected offset of trade_confirmation_message::fields_type::original_volume");
static_assert(offsetof(trade_confirmation_message::fields_type, disclosed_volume) == 34, "unexpected offset of trade_confirmation_message::fields_type::disclosed_volume");
static_assert(offsetof(trade_confirmation_message::fields_type, remaining_volume) == 38, "unexpected offset of trade_confirmation_message::fields_type::remaining_volume");
static_assert(offsetof(trade_confirmation_message::fields_type, disclosed_volume_remaining) == 42, "unexpected offset of trade_confirmation_message::fields_type::disclosed_volume_remaining");
static_assert(offsetof(trade_confirmation_message::fields_type, price) == 46, "unexpected offset of trade_confirmation_message::fields_type::price");
static_assert(offsetof(trade_confirmation_message::fields_type, st_order_flags) == 50, "unexpected offset of trade_confirmation_message::fields_type::st_order_flags");
static_assert(offsetof(trade_confirmation_message::fields_type, good_till_date) == 52, "unexpected offset of trade_confirmation_message::fields_type::good_till_date");
static_assert(offsetof(trade_confirmation_message::fields_type, fill_number) == 56, "unexpected offset of trade_confirmation_message::fields_type::fill_number");
static_assert(offsetof(trade_confirmation_message::fields_type, fill_quantity) == 60, "unexpected offset of trade_confirmation_message::fields_type::fill_quantity");
static_assert(offsetof(trade_confirmation_message::fields_type, fill_price) == 64, "unexpected offset of trade_confirmation_message::fields_type::fill_price");
static_assert(offsetof(trade_confirmation_message::fields_type, volume_filled_today) == 68, "unexpected offset of trade_confirmation_message::fields_type::volume_filled_today");
static_assert(offsetof(trade_confirmation_message::fields_type, activity_type) == 72, "unexpected offset of trade_confirmation_message::fields_type::activity_type");
static_assert(offsetof(trade_confirmation_message::fields_type, activity_time) == 74, "unexpected offset of trade_confirmation_message::fields_type::activity_time");
static_assert(offsetof(trade_confirmation_message::fields_type, counter_trader_order_number) == 78, "unexpected offset of trade_confirmation_message::fields_type::counter_trader_order_number");
static_assert(offsetof(trade_confirmation_message::fields_type, counter_broker_id) == 86, "unexpected offset of trade_confirmation_message::fields_type::counter_broker_id");
static_assert(offsetof(trade_confirmation_message::fields_type, second_reserved_1) == 91, "unexpected offset of trade_confirmation_message::fields_type::second_reserved_1");
static_assert(offsetof(trade_confirmation_message::fields_type, token) == 92, "unexpected offset of trade_confirmation_message::fields_type::token");
static_assert(offsetof(trade_confirmation_message::fields_type, contract_desc) == 96, "unexpected offset of trade_confirmation_message::fields_type::contract_desc");
static_assert(offsetof(trade_confirmation_message::fields_type, open_close) == 124, "unexpected offset of trade_confirmation_message::fields_type::open_close");
static_assert(offsetof(trade_confirmation_message::fields_type, old_open_close) == 125, "unexpected offset of trade_confirmation_message::fields_type::old_open_close");
static_assert(offsetof(trade_confirmation_message::fields_type, trade_book_type) == 126, "unexpected offset of trade_confirmation_message::fields_type::trade_book_type");
static_assert(offsetof(trade_confirmation_message::fields_type, third_reserved_1) == 127, "unexpected offset of trade_confirmation_message::fields_type::third_reserved_1");
static_assert(offsetof(trade_confirmation_message::fields_type, reserved_4) == 128, "unexpected offset of trade_confirmation_message::fields_type::reserved_4");
static_assert(offsetof(trade_confirmation_message::fields_type, old_account_number) == 132, "unexpected offset of trade_confirmation_message::fields_type::old_account_number");
static_assert(offsetof(trade_confirmation_message::fields_type, participant) == 142, "unexpected offset of trade_confirmation_message::fields_type::participant");
static_assert(offsetof(trade_confirmation_message::fields_type, old_participant) == 154, "unexpected offset of trade_confirmation_message::fields_type::old_participant");
static_assert(offsetof(trade_confirmation_message::fields_type, additional_order_flags) == 166, "unexpected offset of trade_confirmation_message::fields_type::additional_order_flags");
static_assert(offsetof(trade_confirmation_message::fields_type, fourth_reserved_1) == 167, "unexpected offset of trade_confirmation_message::fields_type::fourth_reserved_1");
static_assert(offsetof(trade_confirmation_message::fields_type, fifth_reserved_1) == 168, "unexpected offset of trade_confirmation_message::fields_type::fifth_reserved_1");
static_assert(offsetof(trade_confirmation_message::fields_type, sixth_reserved_1) == 169, "unexpected offset of trade_confirmation_message::fields_type::sixth_reserved_1");
static_assert(offsetof(trade_confirmation_message::fields_type, pan) == 170, "unexpected offset of trade_confirmation_message::fields_type::pan");
static_assert(offsetof(trade_confirmation_message::fields_type, old_pan) == 180, "unexpected offset of trade_confirmation_message::fields_type::old_pan");
static_assert(offsetof(trade_confirmation_message::fields_type, algo_id) == 190, "unexpected offset of trade_confirmation_message::fields_type::algo_id");
static_assert(offsetof(trade_confirmation_message::fields_type, reserved_2) == 194, "unexpected offset of trade_confirmation_message::fields_type::reserved_2");
static_assert(offsetof(trade_confirmation_message::fields_type, last_activity_reference) == 196, "unexpected offset of trade_confirmation_message::fields_type::last_activity_reference");
static_assert(offsetof(trade_confirmation_message::fields_type, reserved_52) == 204, "unexpected offset of trade_confirmation_message::fields_type::reserved_52");
static_assert(sizeof(trade_confirmation_message::fields_type) == 256, "unexpected sizeof trade_confirmation_message::fields_type");
static_assert(sizeof(trade_confirmation_message) == sizeof(nnf_orderentry::message_header) + 256, "unexpected sizeof trade_confirmation_message");

#pragma pack(pop)
}
