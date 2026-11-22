#pragma once

#include <cstddef>
#include "../types/Token2.hpp"
#include "../structs/ContractDesc.hpp"
#include "../types/OpBrokerId2.hpp"
#include "../types/Fillerx2.hpp"
#include "../types/OrderType2.hpp"
#include "../types/BuySell2.hpp"
#include "../types/DisclosedVol2.hpp"
#include "../types/DisclosedVolRemaining2.hpp"
#include "../types/TotalVolRemaining2.hpp"
#include "../types/Volume2.hpp"
#include "../types/VolumeFilledToday2.hpp"
#include "../types/Price2.hpp"
#include "../types/TriggerPrice2.hpp"
#include "../types/MinFillAon2.hpp"
#include "../types/StOrderFlags.hpp"
#include "../types/OpenClose2.hpp"
#include "../types/AdditionalOrderFlags.hpp"
#include "../types/Reserved1.hpp"
#include "../types/FillerY.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct ms_spd_leg_info {

    nnf_orderentry::token_2 token_2;
    nnf_orderentry::contract_desc contract_desc;
    nnf_orderentry::op_broker_id_2 op_broker_id_2;
    nnf_orderentry::fillerx_2 fillerx_2;
    nnf_orderentry::order_type_2 order_type_2;
    nnf_orderentry::buy_sell_2 buy_sell_2;
    nnf_orderentry::disclosed_vol_2 disclosed_vol_2;
    nnf_orderentry::disclosed_vol_remaining_2 disclosed_vol_remaining_2;
    nnf_orderentry::total_vol_remaining_2 total_vol_remaining_2;
    nnf_orderentry::volume_2 volume_2;
    nnf_orderentry::volume_filled_today_2 volume_filled_today_2;
    nnf_orderentry::price_2 price_2;
    nnf_orderentry::trigger_price_2 trigger_price_2;
    nnf_orderentry::min_fill_aon_2 min_fill_aon_2;
    nnf_orderentry::st_order_flags st_order_flags;
    nnf_orderentry::open_close_2 open_close_2;
    nnf_orderentry::additional_order_flags additional_order_flags;
    nnf_orderentry::reserved_1 reserved_1;
    nnf_orderentry::filler_y filler_y;

    // parse method
    static ms_spd_leg_info* parse(std::byte* buffer) {
        return reinterpret_cast<ms_spd_leg_info*>(buffer);
    }

    // parse method const
    static const ms_spd_leg_info* parse(const std::byte* buffer) {
        return reinterpret_cast<const ms_spd_leg_info*>(buffer);
    }
};

// layout verification
static_assert(offsetof(ms_spd_leg_info, token_2) == 0, "unexpected offset of ms_spd_leg_info::token_2");
static_assert(offsetof(ms_spd_leg_info, contract_desc) == 4, "unexpected offset of ms_spd_leg_info::contract_desc");
static_assert(offsetof(ms_spd_leg_info, op_broker_id_2) == 32, "unexpected offset of ms_spd_leg_info::op_broker_id_2");
static_assert(offsetof(ms_spd_leg_info, fillerx_2) == 37, "unexpected offset of ms_spd_leg_info::fillerx_2");
static_assert(offsetof(ms_spd_leg_info, order_type_2) == 38, "unexpected offset of ms_spd_leg_info::order_type_2");
static_assert(offsetof(ms_spd_leg_info, buy_sell_2) == 40, "unexpected offset of ms_spd_leg_info::buy_sell_2");
static_assert(offsetof(ms_spd_leg_info, disclosed_vol_2) == 42, "unexpected offset of ms_spd_leg_info::disclosed_vol_2");
static_assert(offsetof(ms_spd_leg_info, disclosed_vol_remaining_2) == 46, "unexpected offset of ms_spd_leg_info::disclosed_vol_remaining_2");
static_assert(offsetof(ms_spd_leg_info, total_vol_remaining_2) == 50, "unexpected offset of ms_spd_leg_info::total_vol_remaining_2");
static_assert(offsetof(ms_spd_leg_info, volume_2) == 54, "unexpected offset of ms_spd_leg_info::volume_2");
static_assert(offsetof(ms_spd_leg_info, volume_filled_today_2) == 58, "unexpected offset of ms_spd_leg_info::volume_filled_today_2");
static_assert(offsetof(ms_spd_leg_info, price_2) == 62, "unexpected offset of ms_spd_leg_info::price_2");
static_assert(offsetof(ms_spd_leg_info, trigger_price_2) == 66, "unexpected offset of ms_spd_leg_info::trigger_price_2");
static_assert(offsetof(ms_spd_leg_info, min_fill_aon_2) == 70, "unexpected offset of ms_spd_leg_info::min_fill_aon_2");
static_assert(offsetof(ms_spd_leg_info, st_order_flags) == 74, "unexpected offset of ms_spd_leg_info::st_order_flags");
static_assert(offsetof(ms_spd_leg_info, open_close_2) == 76, "unexpected offset of ms_spd_leg_info::open_close_2");
static_assert(offsetof(ms_spd_leg_info, additional_order_flags) == 77, "unexpected offset of ms_spd_leg_info::additional_order_flags");
static_assert(offsetof(ms_spd_leg_info, reserved_1) == 78, "unexpected offset of ms_spd_leg_info::reserved_1");
static_assert(offsetof(ms_spd_leg_info, filler_y) == 79, "unexpected offset of ms_spd_leg_info::filler_y");
static_assert(sizeof(ms_spd_leg_info) == 80, "unexpected sizeof ms_spd_leg_info");

#pragma pack(pop)
}
