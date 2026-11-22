#pragma once

#include <cstddef>
#include "../types/SectorName.hpp"
#include "../types/IndexValue.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct index_data {

    nnf_orderentry::sector_name sector_name;
    nnf_orderentry::index_value index_value;

    // parse method
    static index_data* parse(std::byte* buffer) {
        return reinterpret_cast<index_data*>(buffer);
    }

    // parse method const
    static const index_data* parse(const std::byte* buffer) {
        return reinterpret_cast<const index_data*>(buffer);
    }
};

// layout verification
static_assert(offsetof(index_data, sector_name) == 0, "unexpected offset of index_data::sector_name");
static_assert(offsetof(index_data, index_value) == 15, "unexpected offset of index_data::index_value");
static_assert(sizeof(index_data) == 19, "unexpected sizeof index_data");

#pragma pack(pop)
}
