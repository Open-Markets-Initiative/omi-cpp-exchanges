#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/Orderbook.hpp"
#include "../types/Cross.hpp"
#include "../types/Price.hpp"
#include "../types/PairedQuantity.hpp"
#include "../types/ImbalanceQuantity.hpp"
#include "../types/ImbalanceDirection.hpp"

namespace odx::odxequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::odx::odxequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct imbalance_indicated_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::orderbook orderbook;
    ipc_matchingengine::cross cross;
    ipc_matchingengine::price price;
    ipc_matchingengine::paired_quantity paired_quantity;
    ipc_matchingengine::imbalance_quantity imbalance_quantity;
    ipc_matchingengine::imbalance_direction imbalance_direction;

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
static_assert(offsetof(imbalance_indicated_message, orderbook) == 8, "unexpected offset of imbalance_indicated_message::orderbook");
static_assert(offsetof(imbalance_indicated_message, cross) == 12, "unexpected offset of imbalance_indicated_message::cross");
static_assert(offsetof(imbalance_indicated_message, price) == 13, "unexpected offset of imbalance_indicated_message::price");
static_assert(offsetof(imbalance_indicated_message, paired_quantity) == 17, "unexpected offset of imbalance_indicated_message::paired_quantity");
static_assert(offsetof(imbalance_indicated_message, imbalance_quantity) == 21, "unexpected offset of imbalance_indicated_message::imbalance_quantity");
static_assert(offsetof(imbalance_indicated_message, imbalance_direction) == 25, "unexpected offset of imbalance_indicated_message::imbalance_direction");
static_assert(sizeof(imbalance_indicated_message) == 26, "unexpected sizeof imbalance_indicated_message");

#pragma pack(pop)
}
