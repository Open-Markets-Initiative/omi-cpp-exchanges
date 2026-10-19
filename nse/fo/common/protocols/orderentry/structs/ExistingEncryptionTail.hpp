#pragma once

#include <cstddef>
#include "../types/CryptographicIv.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct existing_encryption_tail {

    nnf_orderentry::cryptographic_iv cryptographic_iv;

    // parse method
    static existing_encryption_tail* parse(std::byte* buffer) {
        return reinterpret_cast<existing_encryption_tail*>(buffer);
    }

    // parse method const
    static const existing_encryption_tail* parse(const std::byte* buffer) {
        return reinterpret_cast<const existing_encryption_tail*>(buffer);
    }
};

// layout verification
static_assert(offsetof(existing_encryption_tail, cryptographic_iv) == 0, "unexpected offset of existing_encryption_tail::cryptographic_iv");
static_assert(sizeof(existing_encryption_tail) == 16, "unexpected sizeof existing_encryption_tail");

#pragma pack(pop)
}
