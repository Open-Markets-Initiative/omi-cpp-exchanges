#pragma once

#include <cstddef>
#include "../structs/GroupSize.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

struct outright_order_events_groups {

    sbe_ilink3::group_size group_size;

    // parse method
    static outright_order_events_groups* parse(std::byte* buffer) {
        return reinterpret_cast<outright_order_events_groups*>(buffer);
    }

    // parse method const
    static const outright_order_events_groups* parse(const std::byte* buffer) {
        return reinterpret_cast<const outright_order_events_groups*>(buffer);
    }
};

#pragma pack(pop)
}
