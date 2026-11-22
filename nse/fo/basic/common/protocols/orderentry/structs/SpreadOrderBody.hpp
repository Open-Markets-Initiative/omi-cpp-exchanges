#pragma once

#include <cstddef>
#include "../types/ParticipantType1.hpp"
#include "../types/Filler1.hpp"
#include "../types/CompetitorPeriod1.hpp"
#include "../types/SolicitorPeriod1.hpp"
#include "../types/ModCxlBy1.hpp"
#include "../types/Filler9.hpp"
#include "../types/ReasonCode1.hpp"
#include "../types/StartAlpha1.hpp"
#include "../types/EndAlpha1.hpp"
#include "../types/Token1.hpp"
#include "../structs/ContractDesc.hpp"
#include "../types/OpBrokerId1.hpp"
#include "../types/Fillerx1.hpp"
#include "../types/FillerOptions1.hpp"
#include "../types/Fillery1.hpp"
#include "../types/OrderType1.hpp"
#include "../types/OrderNumber1.hpp"
#include "../types/AccountNumber1.hpp"
#include "../types/BookType1.hpp"
#include "../types/BuySell1.hpp"
#include "../types/DisclosedVol1.hpp"
#include "../types/DisclosedVolRemaining1.hpp"
#include "../types/TotalVolRemaining1.hpp"
#include "../types/Volume1.hpp"
#include "../types/VolumeFilledToday1.hpp"
#include "../types/Price1.hpp"
#include "../types/TriggerPrice1.hpp"
#include "../types/GoodTillDate1.hpp"
#include "../types/EntryDateTime1.hpp"
#include "../types/MinFillAon1.hpp"
#include "../types/LastModified1.hpp"
#include "../types/StOrderFlags.hpp"
#include "../types/BranchId1.hpp"
#include "../types/TraderId1.hpp"
#include "../types/BrokerId1.hpp"
#include "../types/COrdFiller.hpp"
#include "../types/OpenClose1.hpp"
#include "../types/Settlor1.hpp"
#include "../types/ProClient1.hpp"
#include "../types/SettlementPeriod1.hpp"
#include "../types/AdditionalOrderFlags.hpp"
#include "../types/Reserved1.hpp"
#include "../types/Filler116.hpp"
#include "../types/Filler17.hpp"
#include "../types/Filler18.hpp"
#include "../types/NnfField.hpp"
#include "../types/MktReplay.hpp"
#include "../types/Pan.hpp"
#include "../types/AlgoId.hpp"
#include "../types/Reserved2.hpp"
#include "../types/LastActivityReference.hpp"
#include "../types/Reserved52.hpp"
#include "../types/PriceDiff.hpp"
#include "../structs/MsSpdLegInfo.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct spread_order_body {

    nnf_orderentry::participant_type_1 participant_type_1;
    nnf_orderentry::filler_1 filler_1;
    nnf_orderentry::competitor_period_1 competitor_period_1;
    nnf_orderentry::solicitor_period_1 solicitor_period_1;
    nnf_orderentry::mod_cxl_by_1 mod_cxl_by_1;
    nnf_orderentry::filler_9 filler_9;
    nnf_orderentry::reason_code_1 reason_code_1;
    nnf_orderentry::start_alpha_1 start_alpha_1;
    nnf_orderentry::end_alpha_1 end_alpha_1;
    nnf_orderentry::token_1 token_1;
    nnf_orderentry::contract_desc contract_desc;
    nnf_orderentry::op_broker_id_1 op_broker_id_1;
    nnf_orderentry::fillerx_1 fillerx_1;
    nnf_orderentry::filler_options_1 filler_options_1;
    nnf_orderentry::fillery_1 fillery_1;
    nnf_orderentry::order_type_1 order_type_1;
    nnf_orderentry::order_number_1 order_number_1;
    nnf_orderentry::account_number_1 account_number_1;
    nnf_orderentry::book_type_1 book_type_1;
    nnf_orderentry::buy_sell_1 buy_sell_1;
    nnf_orderentry::disclosed_vol_1 disclosed_vol_1;
    nnf_orderentry::disclosed_vol_remaining_1 disclosed_vol_remaining_1;
    nnf_orderentry::total_vol_remaining_1 total_vol_remaining_1;
    nnf_orderentry::volume_1 volume_1;
    nnf_orderentry::volume_filled_today_1 volume_filled_today_1;
    nnf_orderentry::price_1 price_1;
    nnf_orderentry::trigger_price_1 trigger_price_1;
    nnf_orderentry::good_till_date_1 good_till_date_1;
    nnf_orderentry::entry_date_time_1 entry_date_time_1;
    nnf_orderentry::min_fill_aon_1 min_fill_aon_1;
    nnf_orderentry::last_modified_1 last_modified_1;
    nnf_orderentry::st_order_flags st_order_flags;
    nnf_orderentry::branch_id_1 branch_id_1;
    nnf_orderentry::trader_id_1 trader_id_1;
    nnf_orderentry::broker_id_1 broker_id_1;
    nnf_orderentry::c_ord_filler c_ord_filler;
    nnf_orderentry::open_close_1 open_close_1;
    nnf_orderentry::settlor_1 settlor_1;
    nnf_orderentry::pro_client_1 pro_client_1;
    nnf_orderentry::settlement_period_1 settlement_period_1;
    nnf_orderentry::additional_order_flags additional_order_flags;
    nnf_orderentry::reserved_1 reserved_1;
    nnf_orderentry::filler_116 filler_116;
    nnf_orderentry::filler_17 filler_17;
    nnf_orderentry::filler_18 filler_18;
    nnf_orderentry::nnf_field nnf_field;
    nnf_orderentry::mkt_replay mkt_replay;
    nnf_orderentry::pan pan;
    nnf_orderentry::algo_id algo_id;
    nnf_orderentry::reserved_2 reserved_2;
    nnf_orderentry::last_activity_reference last_activity_reference;
    nnf_orderentry::reserved_52 reserved_52;
    nnf_orderentry::price_diff price_diff;
    nnf_orderentry::ms_spd_leg_info ms_spd_leg_info[2];

    // parse method
    static spread_order_body* parse(std::byte* buffer) {
        return reinterpret_cast<spread_order_body*>(buffer);
    }

    // parse method const
    static const spread_order_body* parse(const std::byte* buffer) {
        return reinterpret_cast<const spread_order_body*>(buffer);
    }
};

// layout verification
static_assert(offsetof(spread_order_body, participant_type_1) == 0, "unexpected offset of spread_order_body::participant_type_1");
static_assert(offsetof(spread_order_body, filler_1) == 1, "unexpected offset of spread_order_body::filler_1");
static_assert(offsetof(spread_order_body, competitor_period_1) == 2, "unexpected offset of spread_order_body::competitor_period_1");
static_assert(offsetof(spread_order_body, solicitor_period_1) == 4, "unexpected offset of spread_order_body::solicitor_period_1");
static_assert(offsetof(spread_order_body, mod_cxl_by_1) == 6, "unexpected offset of spread_order_body::mod_cxl_by_1");
static_assert(offsetof(spread_order_body, filler_9) == 7, "unexpected offset of spread_order_body::filler_9");
static_assert(offsetof(spread_order_body, reason_code_1) == 8, "unexpected offset of spread_order_body::reason_code_1");
static_assert(offsetof(spread_order_body, start_alpha_1) == 10, "unexpected offset of spread_order_body::start_alpha_1");
static_assert(offsetof(spread_order_body, end_alpha_1) == 12, "unexpected offset of spread_order_body::end_alpha_1");
static_assert(offsetof(spread_order_body, token_1) == 14, "unexpected offset of spread_order_body::token_1");
static_assert(offsetof(spread_order_body, contract_desc) == 18, "unexpected offset of spread_order_body::contract_desc");
static_assert(offsetof(spread_order_body, op_broker_id_1) == 46, "unexpected offset of spread_order_body::op_broker_id_1");
static_assert(offsetof(spread_order_body, fillerx_1) == 51, "unexpected offset of spread_order_body::fillerx_1");
static_assert(offsetof(spread_order_body, filler_options_1) == 52, "unexpected offset of spread_order_body::filler_options_1");
static_assert(offsetof(spread_order_body, fillery_1) == 55, "unexpected offset of spread_order_body::fillery_1");
static_assert(offsetof(spread_order_body, order_type_1) == 56, "unexpected offset of spread_order_body::order_type_1");
static_assert(offsetof(spread_order_body, order_number_1) == 58, "unexpected offset of spread_order_body::order_number_1");
static_assert(offsetof(spread_order_body, account_number_1) == 66, "unexpected offset of spread_order_body::account_number_1");
static_assert(offsetof(spread_order_body, book_type_1) == 76, "unexpected offset of spread_order_body::book_type_1");
static_assert(offsetof(spread_order_body, buy_sell_1) == 78, "unexpected offset of spread_order_body::buy_sell_1");
static_assert(offsetof(spread_order_body, disclosed_vol_1) == 80, "unexpected offset of spread_order_body::disclosed_vol_1");
static_assert(offsetof(spread_order_body, disclosed_vol_remaining_1) == 84, "unexpected offset of spread_order_body::disclosed_vol_remaining_1");
static_assert(offsetof(spread_order_body, total_vol_remaining_1) == 88, "unexpected offset of spread_order_body::total_vol_remaining_1");
static_assert(offsetof(spread_order_body, volume_1) == 92, "unexpected offset of spread_order_body::volume_1");
static_assert(offsetof(spread_order_body, volume_filled_today_1) == 96, "unexpected offset of spread_order_body::volume_filled_today_1");
static_assert(offsetof(spread_order_body, price_1) == 100, "unexpected offset of spread_order_body::price_1");
static_assert(offsetof(spread_order_body, trigger_price_1) == 104, "unexpected offset of spread_order_body::trigger_price_1");
static_assert(offsetof(spread_order_body, good_till_date_1) == 108, "unexpected offset of spread_order_body::good_till_date_1");
static_assert(offsetof(spread_order_body, entry_date_time_1) == 112, "unexpected offset of spread_order_body::entry_date_time_1");
static_assert(offsetof(spread_order_body, min_fill_aon_1) == 116, "unexpected offset of spread_order_body::min_fill_aon_1");
static_assert(offsetof(spread_order_body, last_modified_1) == 120, "unexpected offset of spread_order_body::last_modified_1");
static_assert(offsetof(spread_order_body, st_order_flags) == 124, "unexpected offset of spread_order_body::st_order_flags");
static_assert(offsetof(spread_order_body, branch_id_1) == 126, "unexpected offset of spread_order_body::branch_id_1");
static_assert(offsetof(spread_order_body, trader_id_1) == 128, "unexpected offset of spread_order_body::trader_id_1");
static_assert(offsetof(spread_order_body, broker_id_1) == 132, "unexpected offset of spread_order_body::broker_id_1");
static_assert(offsetof(spread_order_body, c_ord_filler) == 137, "unexpected offset of spread_order_body::c_ord_filler");
static_assert(offsetof(spread_order_body, open_close_1) == 161, "unexpected offset of spread_order_body::open_close_1");
static_assert(offsetof(spread_order_body, settlor_1) == 162, "unexpected offset of spread_order_body::settlor_1");
static_assert(offsetof(spread_order_body, pro_client_1) == 174, "unexpected offset of spread_order_body::pro_client_1");
static_assert(offsetof(spread_order_body, settlement_period_1) == 176, "unexpected offset of spread_order_body::settlement_period_1");
static_assert(offsetof(spread_order_body, additional_order_flags) == 178, "unexpected offset of spread_order_body::additional_order_flags");
static_assert(offsetof(spread_order_body, reserved_1) == 179, "unexpected offset of spread_order_body::reserved_1");
static_assert(offsetof(spread_order_body, filler_116) == 180, "unexpected offset of spread_order_body::filler_116");
static_assert(offsetof(spread_order_body, filler_17) == 182, "unexpected offset of spread_order_body::filler_17");
static_assert(offsetof(spread_order_body, filler_18) == 183, "unexpected offset of spread_order_body::filler_18");
static_assert(offsetof(spread_order_body, nnf_field) == 184, "unexpected offset of spread_order_body::nnf_field");
static_assert(offsetof(spread_order_body, mkt_replay) == 192, "unexpected offset of spread_order_body::mkt_replay");
static_assert(offsetof(spread_order_body, pan) == 200, "unexpected offset of spread_order_body::pan");
static_assert(offsetof(spread_order_body, algo_id) == 210, "unexpected offset of spread_order_body::algo_id");
static_assert(offsetof(spread_order_body, reserved_2) == 214, "unexpected offset of spread_order_body::reserved_2");
static_assert(offsetof(spread_order_body, last_activity_reference) == 216, "unexpected offset of spread_order_body::last_activity_reference");
static_assert(offsetof(spread_order_body, reserved_52) == 224, "unexpected offset of spread_order_body::reserved_52");
static_assert(offsetof(spread_order_body, price_diff) == 276, "unexpected offset of spread_order_body::price_diff");
static_assert(offsetof(spread_order_body, ms_spd_leg_info) == 280, "unexpected offset of spread_order_body::ms_spd_leg_info");
static_assert(sizeof(spread_order_body) == 440, "unexpected sizeof spread_order_body");

#pragma pack(pop)
}
