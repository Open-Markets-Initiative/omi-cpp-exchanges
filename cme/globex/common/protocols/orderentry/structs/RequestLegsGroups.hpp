#pragma once

#include <cstddef>
#include "../structs/GroupSize.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

struct request_legs_groups {

    sbe_ilink3::group_size group_size;

    // parse method
    static request_legs_groups* parse(std::byte* buffer) {
        return reinterpret_cast<request_legs_groups*>(buffer);
    }

    // parse method const
    static const request_legs_groups* parse(const std::byte* buffer) {
        return reinterpret_cast<const request_legs_groups*>(buffer);
    }
};

#pragma pack(pop)
}
