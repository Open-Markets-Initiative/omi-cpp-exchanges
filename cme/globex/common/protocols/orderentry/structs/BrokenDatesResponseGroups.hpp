#pragma once

#include <cstddef>
#include "../structs/GroupSize.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

struct broken_dates_response_groups {

    sbe_ilink3::group_size group_size;

    // parse method
    static broken_dates_response_groups* parse(std::byte* buffer) {
        return reinterpret_cast<broken_dates_response_groups*>(buffer);
    }

    // parse method const
    static const broken_dates_response_groups* parse(const std::byte* buffer) {
        return reinterpret_cast<const broken_dates_response_groups*>(buffer);
    }
};

#pragma pack(pop)
}
