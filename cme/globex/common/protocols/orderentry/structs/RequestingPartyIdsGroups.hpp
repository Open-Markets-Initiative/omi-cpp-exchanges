#pragma once

#include <cstddef>
#include "../structs/GroupSize.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

struct requesting_party_ids_groups {

    sbe_ilink3::group_size group_size;

    // parse method
    static requesting_party_ids_groups* parse(std::byte* buffer) {
        return reinterpret_cast<requesting_party_ids_groups*>(buffer);
    }

    // parse method const
    static const requesting_party_ids_groups* parse(const std::byte* buffer) {
        return reinterpret_cast<const requesting_party_ids_groups*>(buffer);
    }
};

#pragma pack(pop)
}
