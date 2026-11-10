#pragma once

#include <cstddef>
#include "../structs/GroupSize.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

struct trd_reg_publications_groups {

    sbe_ilink3::group_size group_size;

    // parse method
    static trd_reg_publications_groups* parse(std::byte* buffer) {
        return reinterpret_cast<trd_reg_publications_groups*>(buffer);
    }

    // parse method const
    static const trd_reg_publications_groups* parse(const std::byte* buffer) {
        return reinterpret_cast<const trd_reg_publications_groups*>(buffer);
    }
};

#pragma pack(pop)
}
