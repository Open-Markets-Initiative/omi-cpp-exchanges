#pragma once

#include <cstddef>
#include "../types/OrderNumber.hpp"
#include "../types/FillNumber.hpp"
#include "../types/InstrumentName.hpp"
#include "../types/Symbol.hpp"
#include "../types/ExpiryDate.hpp"
#include "../types/StrikePrice.hpp"
#include "../types/OptionType.hpp"
#include "../types/CaLevel.hpp"
#include "../types/FillVolume.hpp"
#include "../types/FillPrice.hpp"
#include "../types/BrokerId.hpp"
#include "../types/Filler.hpp"
#include "../types/BuySell.hpp"
#include "../types/BookType.hpp"
#include "../types/LastModifiedDateTime.hpp"
#include "../types/InitiatedByControl.hpp"
#include "../types/OpenClose.hpp"
#include "../types/Reserved1.hpp"
#include "../types/Participant.hpp"
#include "../types/GiveupFlag.hpp"
#include "../types/Deleted.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct giveup {

    nnf_orderentry::order_number order_number;
    nnf_orderentry::fill_number fill_number;
    nnf_orderentry::instrument_name instrument_name;
    nnf_orderentry::symbol symbol;
    nnf_orderentry::expiry_date expiry_date;
    nnf_orderentry::strike_price strike_price;
    nnf_orderentry::option_type option_type;
    nnf_orderentry::ca_level ca_level;
    nnf_orderentry::fill_volume fill_volume;
    nnf_orderentry::fill_price fill_price;
    nnf_orderentry::broker_id broker_id;
    nnf_orderentry::filler filler;
    nnf_orderentry::buy_sell buy_sell;
    nnf_orderentry::book_type book_type;
    nnf_orderentry::last_modified_date_time last_modified_date_time;
    nnf_orderentry::initiated_by_control initiated_by_control;
    nnf_orderentry::open_close open_close;
    nnf_orderentry::reserved_1 reserved_1;
    nnf_orderentry::participant participant;
    nnf_orderentry::giveup_flag giveup_flag;
    nnf_orderentry::deleted deleted;

    // parse method
    static giveup* parse(std::byte* buffer) {
        return reinterpret_cast<giveup*>(buffer);
    }

    // parse method const
    static const giveup* parse(const std::byte* buffer) {
        return reinterpret_cast<const giveup*>(buffer);
    }
};

// layout verification
static_assert(offsetof(giveup, order_number) == 0, "unexpected offset of giveup::order_number");
static_assert(offsetof(giveup, fill_number) == 8, "unexpected offset of giveup::fill_number");
static_assert(offsetof(giveup, instrument_name) == 12, "unexpected offset of giveup::instrument_name");
static_assert(offsetof(giveup, symbol) == 18, "unexpected offset of giveup::symbol");
static_assert(offsetof(giveup, expiry_date) == 28, "unexpected offset of giveup::expiry_date");
static_assert(offsetof(giveup, strike_price) == 32, "unexpected offset of giveup::strike_price");
static_assert(offsetof(giveup, option_type) == 36, "unexpected offset of giveup::option_type");
static_assert(offsetof(giveup, ca_level) == 38, "unexpected offset of giveup::ca_level");
static_assert(offsetof(giveup, fill_volume) == 40, "unexpected offset of giveup::fill_volume");
static_assert(offsetof(giveup, fill_price) == 44, "unexpected offset of giveup::fill_price");
static_assert(offsetof(giveup, broker_id) == 48, "unexpected offset of giveup::broker_id");
static_assert(offsetof(giveup, filler) == 53, "unexpected offset of giveup::filler");
static_assert(offsetof(giveup, buy_sell) == 54, "unexpected offset of giveup::buy_sell");
static_assert(offsetof(giveup, book_type) == 56, "unexpected offset of giveup::book_type");
static_assert(offsetof(giveup, last_modified_date_time) == 58, "unexpected offset of giveup::last_modified_date_time");
static_assert(offsetof(giveup, initiated_by_control) == 62, "unexpected offset of giveup::initiated_by_control");
static_assert(offsetof(giveup, open_close) == 63, "unexpected offset of giveup::open_close");
static_assert(offsetof(giveup, reserved_1) == 64, "unexpected offset of giveup::reserved_1");
static_assert(offsetof(giveup, participant) == 65, "unexpected offset of giveup::participant");
static_assert(offsetof(giveup, giveup_flag) == 77, "unexpected offset of giveup::giveup_flag");
static_assert(offsetof(giveup, deleted) == 78, "unexpected offset of giveup::deleted");
static_assert(sizeof(giveup) == 79, "unexpected sizeof giveup");

#pragma pack(pop)
}
