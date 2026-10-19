#pragma once

#include <cstddef>
#include "../types/MessageType.hpp"

namespace nse::nsefo::mtbt::binary::v7_1 {

namespace binary_mtbt = ::nse::nsefo::mtbt::binary::v7_1;

#pragma pack(push, 1)

struct message {

    binary_mtbt::message_type message_type;

    // parse method
    static message* parse(std::byte* buffer) {
        return reinterpret_cast<message*>(buffer);
    }

    // parse method const
    static const message* parse(const std::byte* buffer) {
        return reinterpret_cast<const message*>(buffer);
    }
};

#pragma pack(pop)
}
