#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/Symbol.hpp"
#include "../types/Cross.hpp"
#include "../types/Price.hpp"
#include "../types/PairedQuantity.hpp"
#include "../types/ImbalanceQuantity.hpp"
#include "../types/ImbalanceDirection.hpp"
#include "../types/FarPrice.hpp"
#include "../types/NearPrice.hpp"
#include "../types/ReferencePrice.hpp"
#include "../types/PriceVariation.hpp"
#include "../types/Early.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct imbalance_indicated_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::symbol symbol;
    ipc_matchingengine::cross cross;
    ipc_matchingengine::price price;
    ipc_matchingengine::paired_quantity paired_quantity;
    ipc_matchingengine::imbalance_quantity imbalance_quantity;
    ipc_matchingengine::imbalance_direction imbalance_direction;
    ipc_matchingengine::far_price far_price;
    ipc_matchingengine::near_price near_price;
    ipc_matchingengine::reference_price reference_price;
    ipc_matchingengine::price_variation price_variation;
    ipc_matchingengine::early early;

    // parse method
    static imbalance_indicated_message* parse(std::byte* buffer) {
        return reinterpret_cast<imbalance_indicated_message*>(buffer);
    }

    // parse method const
    static const imbalance_indicated_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const imbalance_indicated_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(imbalance_indicated_message, timestamp) == 0, "unexpected offset of imbalance_indicated_message::timestamp");
static_assert(offsetof(imbalance_indicated_message, symbol) == 8, "unexpected offset of imbalance_indicated_message::symbol");
static_assert(offsetof(imbalance_indicated_message, cross) == 16, "unexpected offset of imbalance_indicated_message::cross");
static_assert(offsetof(imbalance_indicated_message, price) == 17, "unexpected offset of imbalance_indicated_message::price");
static_assert(offsetof(imbalance_indicated_message, paired_quantity) == 25, "unexpected offset of imbalance_indicated_message::paired_quantity");
static_assert(offsetof(imbalance_indicated_message, imbalance_quantity) == 33, "unexpected offset of imbalance_indicated_message::imbalance_quantity");
static_assert(offsetof(imbalance_indicated_message, imbalance_direction) == 41, "unexpected offset of imbalance_indicated_message::imbalance_direction");
static_assert(offsetof(imbalance_indicated_message, far_price) == 42, "unexpected offset of imbalance_indicated_message::far_price");
static_assert(offsetof(imbalance_indicated_message, near_price) == 46, "unexpected offset of imbalance_indicated_message::near_price");
static_assert(offsetof(imbalance_indicated_message, reference_price) == 50, "unexpected offset of imbalance_indicated_message::reference_price");
static_assert(offsetof(imbalance_indicated_message, price_variation) == 54, "unexpected offset of imbalance_indicated_message::price_variation");
static_assert(offsetof(imbalance_indicated_message, early) == 55, "unexpected offset of imbalance_indicated_message::early");
static_assert(sizeof(imbalance_indicated_message) == 56, "unexpected sizeof imbalance_indicated_message");

#pragma pack(pop)
}
