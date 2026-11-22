#pragma once

#include <cstddef>
#include "../types/Timestamp.hpp"
#include "../types/Symbol.hpp"
#include "../types/Announcement.hpp"
#include "../types/Reason.hpp"
#include "../types/Release.hpp"
#include "../types/CollarReference.hpp"
#include "../types/CollarUpper.hpp"
#include "../types/CollarLower.hpp"
#include "../types/CollarExtension.hpp"

namespace nasdaq::nsmequities::matchingengine::ipc::v2026 {

namespace ipc_matchingengine = ::nasdaq::nsmequities::matchingengine::ipc::v2026;

#pragma pack(push, 1)

struct phase_announced_message {

    ipc_matchingengine::timestamp timestamp;
    ipc_matchingengine::symbol symbol;
    ipc_matchingengine::announcement announcement;
    ipc_matchingengine::reason reason;
    ipc_matchingengine::release release;
    ipc_matchingengine::collar_reference collar_reference;
    ipc_matchingengine::collar_upper collar_upper;
    ipc_matchingengine::collar_lower collar_lower;
    ipc_matchingengine::collar_extension collar_extension;

    // parse method
    static phase_announced_message* parse(std::byte* buffer) {
        return reinterpret_cast<phase_announced_message*>(buffer);
    }

    // parse method const
    static const phase_announced_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const phase_announced_message*>(buffer);
    }
};

// layout verification
static_assert(offsetof(phase_announced_message, timestamp) == 0, "unexpected offset of phase_announced_message::timestamp");
static_assert(offsetof(phase_announced_message, symbol) == 8, "unexpected offset of phase_announced_message::symbol");
static_assert(offsetof(phase_announced_message, announcement) == 16, "unexpected offset of phase_announced_message::announcement");
static_assert(offsetof(phase_announced_message, reason) == 17, "unexpected offset of phase_announced_message::reason");
static_assert(offsetof(phase_announced_message, release) == 21, "unexpected offset of phase_announced_message::release");
static_assert(offsetof(phase_announced_message, collar_reference) == 29, "unexpected offset of phase_announced_message::collar_reference");
static_assert(offsetof(phase_announced_message, collar_upper) == 37, "unexpected offset of phase_announced_message::collar_upper");
static_assert(offsetof(phase_announced_message, collar_lower) == 45, "unexpected offset of phase_announced_message::collar_lower");
static_assert(offsetof(phase_announced_message, collar_extension) == 53, "unexpected offset of phase_announced_message::collar_extension");
static_assert(sizeof(phase_announced_message) == 57, "unexpected sizeof phase_announced_message");

#pragma pack(pop)
}
