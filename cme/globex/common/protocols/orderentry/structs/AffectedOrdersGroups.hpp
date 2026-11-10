#pragma once

#include <cstddef>
#include "../structs/GroupSize.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

struct affected_orders_groups {

    sbe_ilink3::group_size group_size;

    // parse method
    static affected_orders_groups* parse(std::byte* buffer) {
        return reinterpret_cast<affected_orders_groups*>(buffer);
    }

    // parse method const
    static const affected_orders_groups* parse(const std::byte* buffer) {
        return reinterpret_cast<const affected_orders_groups*>(buffer);
    }
};

#pragma pack(pop)
}
