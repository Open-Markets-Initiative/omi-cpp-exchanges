#pragma once

#include <cstddef>
#include "../types/InstrumentName.hpp"
#include "../types/Symbol.hpp"
#include "../types/ExpiryDate.hpp"
#include "../types/StrikePrice.hpp"
#include "../types/OptionType.hpp"
#include "../types/CaLevel.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct contract_desc {

    nnf_orderentry::instrument_name instrument_name;
    nnf_orderentry::symbol symbol;
    nnf_orderentry::expiry_date expiry_date;
    nnf_orderentry::strike_price strike_price;
    nnf_orderentry::option_type option_type;
    nnf_orderentry::ca_level ca_level;

    // parse method
    static contract_desc* parse(std::byte* buffer) {
        return reinterpret_cast<contract_desc*>(buffer);
    }

    // parse method const
    static const contract_desc* parse(const std::byte* buffer) {
        return reinterpret_cast<const contract_desc*>(buffer);
    }
};

// layout verification
static_assert(offsetof(contract_desc, instrument_name) == 0, "unexpected offset of contract_desc::instrument_name");
static_assert(offsetof(contract_desc, symbol) == 6, "unexpected offset of contract_desc::symbol");
static_assert(offsetof(contract_desc, expiry_date) == 16, "unexpected offset of contract_desc::expiry_date");
static_assert(offsetof(contract_desc, strike_price) == 20, "unexpected offset of contract_desc::strike_price");
static_assert(offsetof(contract_desc, option_type) == 24, "unexpected offset of contract_desc::option_type");
static_assert(offsetof(contract_desc, ca_level) == 26, "unexpected offset of contract_desc::ca_level");
static_assert(sizeof(contract_desc) == 28, "unexpected sizeof contract_desc");

#pragma pack(pop)
}
