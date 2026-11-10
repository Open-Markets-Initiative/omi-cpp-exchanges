#pragma once

#include <cstddef>
#include "../structs/GroupSize.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

struct quote_cancel_by_group_entries_groups {

    sbe_ilink3::group_size group_size;

    // parse method
    static quote_cancel_by_group_entries_groups* parse(std::byte* buffer) {
        return reinterpret_cast<quote_cancel_by_group_entries_groups*>(buffer);
    }

    // parse method const
    static const quote_cancel_by_group_entries_groups* parse(const std::byte* buffer) {
        return reinterpret_cast<const quote_cancel_by_group_entries_groups*>(buffer);
    }
};

#pragma pack(pop)
}
