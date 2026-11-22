#pragma once

#include <cstddef>
#include "../types/ParticipantType.hpp"
#include "../types/Reserved1.hpp"
#include "../types/CompetitorPeriod.hpp"
#include "../types/SolicitorPeriod.hpp"
#include "../types/ModifiedCancelledBy.hpp"
#include "../types/SecondReserved1.hpp"
#include "../types/ReasonCode.hpp"
#include "../types/Reserved4.hpp"
#include "../types/TokenNo.hpp"
#include "../structs/ContractDesc.hpp"
#include "../types/CounterPartyBrokerId.hpp"
#include "../types/ThirdReserved1.hpp"
#include "../types/Reserved2.hpp"
#include "../types/CloseoutFlag.hpp"
#include "../types/FourthReserved1.hpp"
#include "../types/OrderType.hpp"
#include "../types/OrderNumber.hpp"
#include "../types/AccountNumber.hpp"
#include "../types/BookType.hpp"
#include "../types/BuySellIndicator.hpp"
#include "../types/DisclosedVolume.hpp"
#include "../types/DisclosedVolumeRemaining.hpp"
#include "../types/TotalVolumeRemaining.hpp"
#include "../types/Volume.hpp"
#include "../types/VolumeFilledToday.hpp"
#include "../types/Price.hpp"
#include "../types/TriggerPrice.hpp"
#include "../types/GoodTillDate.hpp"
#include "../types/EntryDateTime.hpp"
#include "../types/MinimumFillAonVolume.hpp"
#include "../types/LastModified.hpp"
#include "../types/StOrderFlags.hpp"
#include "../types/BranchId.hpp"
#include "../types/TraderId.hpp"
#include "../types/BrokerId.hpp"
#include "../types/COrdFiller.hpp"
#include "../types/OpenClose.hpp"
#include "../types/Settlor.hpp"
#include "../types/ProClientIndicator.hpp"
#include "../types/SettlementPeriod.hpp"
#include "../types/AdditionalOrderFlags.hpp"
#include "../types/FifthReserved1.hpp"
#include "../types/Filler116.hpp"
#include "../types/Filler17.hpp"
#include "../types/Filler18.hpp"
#include "../types/NnfField.hpp"
#include "../types/MktReplay.hpp"
#include "../types/Pan.hpp"
#include "../types/AlgoId.hpp"
#include "../types/SecondReserved2.hpp"
#include "../types/LastActivityReference.hpp"
#include "../types/Reserved52.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct order_entry_body {

    nnf_orderentry::participant_type participant_type;
    nnf_orderentry::reserved_1 reserved_1;
    nnf_orderentry::competitor_period competitor_period;
    nnf_orderentry::solicitor_period solicitor_period;
    nnf_orderentry::modified_cancelled_by modified_cancelled_by;
    nnf_orderentry::second_reserved_1 second_reserved_1;
    nnf_orderentry::reason_code reason_code;
    nnf_orderentry::reserved_4 reserved_4;
    nnf_orderentry::token_no token_no;
    nnf_orderentry::contract_desc contract_desc;
    nnf_orderentry::counter_party_broker_id counter_party_broker_id;
    nnf_orderentry::third_reserved_1 third_reserved_1;
    nnf_orderentry::reserved_2 reserved_2;
    nnf_orderentry::closeout_flag closeout_flag;
    nnf_orderentry::fourth_reserved_1 fourth_reserved_1;
    nnf_orderentry::order_type order_type;
    nnf_orderentry::order_number order_number;
    nnf_orderentry::account_number account_number;
    nnf_orderentry::book_type book_type;
    nnf_orderentry::buy_sell_indicator buy_sell_indicator;
    nnf_orderentry::disclosed_volume disclosed_volume;
    nnf_orderentry::disclosed_volume_remaining disclosed_volume_remaining;
    nnf_orderentry::total_volume_remaining total_volume_remaining;
    nnf_orderentry::volume volume;
    nnf_orderentry::volume_filled_today volume_filled_today;
    nnf_orderentry::price price;
    nnf_orderentry::trigger_price trigger_price;
    nnf_orderentry::good_till_date good_till_date;
    nnf_orderentry::entry_date_time entry_date_time;
    nnf_orderentry::minimum_fill_aon_volume minimum_fill_aon_volume;
    nnf_orderentry::last_modified last_modified;
    nnf_orderentry::st_order_flags st_order_flags;
    nnf_orderentry::branch_id branch_id;
    nnf_orderentry::trader_id trader_id;
    nnf_orderentry::broker_id broker_id;
    nnf_orderentry::c_ord_filler c_ord_filler;
    nnf_orderentry::open_close open_close;
    nnf_orderentry::settlor settlor;
    nnf_orderentry::pro_client_indicator pro_client_indicator;
    nnf_orderentry::settlement_period settlement_period;
    nnf_orderentry::additional_order_flags additional_order_flags;
    nnf_orderentry::fifth_reserved_1 fifth_reserved_1;
    nnf_orderentry::filler_116 filler_116;
    nnf_orderentry::filler_17 filler_17;
    nnf_orderentry::filler_18 filler_18;
    nnf_orderentry::nnf_field nnf_field;
    nnf_orderentry::mkt_replay mkt_replay;
    nnf_orderentry::pan pan;
    nnf_orderentry::algo_id algo_id;
    nnf_orderentry::second_reserved_2 second_reserved_2;
    nnf_orderentry::last_activity_reference last_activity_reference;
    nnf_orderentry::reserved_52 reserved_52;

    // parse method
    static order_entry_body* parse(std::byte* buffer) {
        return reinterpret_cast<order_entry_body*>(buffer);
    }

    // parse method const
    static const order_entry_body* parse(const std::byte* buffer) {
        return reinterpret_cast<const order_entry_body*>(buffer);
    }
};

// layout verification
static_assert(offsetof(order_entry_body, participant_type) == 0, "unexpected offset of order_entry_body::participant_type");
static_assert(offsetof(order_entry_body, reserved_1) == 1, "unexpected offset of order_entry_body::reserved_1");
static_assert(offsetof(order_entry_body, competitor_period) == 2, "unexpected offset of order_entry_body::competitor_period");
static_assert(offsetof(order_entry_body, solicitor_period) == 4, "unexpected offset of order_entry_body::solicitor_period");
static_assert(offsetof(order_entry_body, modified_cancelled_by) == 6, "unexpected offset of order_entry_body::modified_cancelled_by");
static_assert(offsetof(order_entry_body, second_reserved_1) == 7, "unexpected offset of order_entry_body::second_reserved_1");
static_assert(offsetof(order_entry_body, reason_code) == 8, "unexpected offset of order_entry_body::reason_code");
static_assert(offsetof(order_entry_body, reserved_4) == 10, "unexpected offset of order_entry_body::reserved_4");
static_assert(offsetof(order_entry_body, token_no) == 14, "unexpected offset of order_entry_body::token_no");
static_assert(offsetof(order_entry_body, contract_desc) == 18, "unexpected offset of order_entry_body::contract_desc");
static_assert(offsetof(order_entry_body, counter_party_broker_id) == 46, "unexpected offset of order_entry_body::counter_party_broker_id");
static_assert(offsetof(order_entry_body, third_reserved_1) == 51, "unexpected offset of order_entry_body::third_reserved_1");
static_assert(offsetof(order_entry_body, reserved_2) == 52, "unexpected offset of order_entry_body::reserved_2");
static_assert(offsetof(order_entry_body, closeout_flag) == 54, "unexpected offset of order_entry_body::closeout_flag");
static_assert(offsetof(order_entry_body, fourth_reserved_1) == 55, "unexpected offset of order_entry_body::fourth_reserved_1");
static_assert(offsetof(order_entry_body, order_type) == 56, "unexpected offset of order_entry_body::order_type");
static_assert(offsetof(order_entry_body, order_number) == 58, "unexpected offset of order_entry_body::order_number");
static_assert(offsetof(order_entry_body, account_number) == 66, "unexpected offset of order_entry_body::account_number");
static_assert(offsetof(order_entry_body, book_type) == 76, "unexpected offset of order_entry_body::book_type");
static_assert(offsetof(order_entry_body, buy_sell_indicator) == 78, "unexpected offset of order_entry_body::buy_sell_indicator");
static_assert(offsetof(order_entry_body, disclosed_volume) == 80, "unexpected offset of order_entry_body::disclosed_volume");
static_assert(offsetof(order_entry_body, disclosed_volume_remaining) == 84, "unexpected offset of order_entry_body::disclosed_volume_remaining");
static_assert(offsetof(order_entry_body, total_volume_remaining) == 88, "unexpected offset of order_entry_body::total_volume_remaining");
static_assert(offsetof(order_entry_body, volume) == 92, "unexpected offset of order_entry_body::volume");
static_assert(offsetof(order_entry_body, volume_filled_today) == 96, "unexpected offset of order_entry_body::volume_filled_today");
static_assert(offsetof(order_entry_body, price) == 100, "unexpected offset of order_entry_body::price");
static_assert(offsetof(order_entry_body, trigger_price) == 104, "unexpected offset of order_entry_body::trigger_price");
static_assert(offsetof(order_entry_body, good_till_date) == 108, "unexpected offset of order_entry_body::good_till_date");
static_assert(offsetof(order_entry_body, entry_date_time) == 112, "unexpected offset of order_entry_body::entry_date_time");
static_assert(offsetof(order_entry_body, minimum_fill_aon_volume) == 116, "unexpected offset of order_entry_body::minimum_fill_aon_volume");
static_assert(offsetof(order_entry_body, last_modified) == 120, "unexpected offset of order_entry_body::last_modified");
static_assert(offsetof(order_entry_body, st_order_flags) == 124, "unexpected offset of order_entry_body::st_order_flags");
static_assert(offsetof(order_entry_body, branch_id) == 126, "unexpected offset of order_entry_body::branch_id");
static_assert(offsetof(order_entry_body, trader_id) == 128, "unexpected offset of order_entry_body::trader_id");
static_assert(offsetof(order_entry_body, broker_id) == 132, "unexpected offset of order_entry_body::broker_id");
static_assert(offsetof(order_entry_body, c_ord_filler) == 137, "unexpected offset of order_entry_body::c_ord_filler");
static_assert(offsetof(order_entry_body, open_close) == 161, "unexpected offset of order_entry_body::open_close");
static_assert(offsetof(order_entry_body, settlor) == 162, "unexpected offset of order_entry_body::settlor");
static_assert(offsetof(order_entry_body, pro_client_indicator) == 174, "unexpected offset of order_entry_body::pro_client_indicator");
static_assert(offsetof(order_entry_body, settlement_period) == 176, "unexpected offset of order_entry_body::settlement_period");
static_assert(offsetof(order_entry_body, additional_order_flags) == 178, "unexpected offset of order_entry_body::additional_order_flags");
static_assert(offsetof(order_entry_body, fifth_reserved_1) == 179, "unexpected offset of order_entry_body::fifth_reserved_1");
static_assert(offsetof(order_entry_body, filler_116) == 180, "unexpected offset of order_entry_body::filler_116");
static_assert(offsetof(order_entry_body, filler_17) == 182, "unexpected offset of order_entry_body::filler_17");
static_assert(offsetof(order_entry_body, filler_18) == 183, "unexpected offset of order_entry_body::filler_18");
static_assert(offsetof(order_entry_body, nnf_field) == 184, "unexpected offset of order_entry_body::nnf_field");
static_assert(offsetof(order_entry_body, mkt_replay) == 192, "unexpected offset of order_entry_body::mkt_replay");
static_assert(offsetof(order_entry_body, pan) == 200, "unexpected offset of order_entry_body::pan");
static_assert(offsetof(order_entry_body, algo_id) == 210, "unexpected offset of order_entry_body::algo_id");
static_assert(offsetof(order_entry_body, second_reserved_2) == 214, "unexpected offset of order_entry_body::second_reserved_2");
static_assert(offsetof(order_entry_body, last_activity_reference) == 216, "unexpected offset of order_entry_body::last_activity_reference");
static_assert(offsetof(order_entry_body, reserved_52) == 224, "unexpected offset of order_entry_body::reserved_52");
static_assert(sizeof(order_entry_body) == 276, "unexpected sizeof order_entry_body");

#pragma pack(pop)
}
