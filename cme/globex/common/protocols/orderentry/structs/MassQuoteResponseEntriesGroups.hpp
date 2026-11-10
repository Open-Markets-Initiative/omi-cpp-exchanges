#pragma once

#include <cstddef>
#include "../structs/GroupSize.hpp"

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;

#pragma pack(push, 1)

struct mass_quote_response_entries_groups {

    sbe_ilink3::group_size group_size;

    // parse method
    static mass_quote_response_entries_groups* parse(std::byte* buffer) {
        return reinterpret_cast<mass_quote_response_entries_groups*>(buffer);
    }

    // parse method const
    static const mass_quote_response_entries_groups* parse(const std::byte* buffer) {
        return reinterpret_cast<const mass_quote_response_entries_groups*>(buffer);
    }
};

#pragma pack(pop)
}
