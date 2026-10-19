#pragma once

#include <cstddef>
#include "../types/StaticCryptographicIv.hpp"
#include "../types/DynamicCryptographicIv.hpp"
#include "../types/CryptographicAdditionalKey.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct new_encryption_tail {

    nnf_orderentry::static_cryptographic_iv static_cryptographic_iv;
    nnf_orderentry::dynamic_cryptographic_iv dynamic_cryptographic_iv;
    nnf_orderentry::cryptographic_additional_key cryptographic_additional_key;

    // parse method
    static new_encryption_tail* parse(std::byte* buffer) {
        return reinterpret_cast<new_encryption_tail*>(buffer);
    }

    // parse method const
    static const new_encryption_tail* parse(const std::byte* buffer) {
        return reinterpret_cast<const new_encryption_tail*>(buffer);
    }
};

// layout verification
static_assert(offsetof(new_encryption_tail, static_cryptographic_iv) == 0, "unexpected offset of new_encryption_tail::static_cryptographic_iv");
static_assert(offsetof(new_encryption_tail, dynamic_cryptographic_iv) == 8, "unexpected offset of new_encryption_tail::dynamic_cryptographic_iv");
static_assert(offsetof(new_encryption_tail, cryptographic_additional_key) == 16, "unexpected offset of new_encryption_tail::cryptographic_additional_key");
static_assert(sizeof(new_encryption_tail) == 28, "unexpected sizeof new_encryption_tail");

#pragma pack(pop)
}
