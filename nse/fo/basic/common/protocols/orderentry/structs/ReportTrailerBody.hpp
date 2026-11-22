#pragma once

#include <cstddef>
#include "../types/NumberOfPackets.hpp"
#include "../types/Reserved1.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct report_trailer_body {

    nnf_orderentry::number_of_packets number_of_packets;
    nnf_orderentry::reserved_1 reserved_1;

    // parse method
    static report_trailer_body* parse(std::byte* buffer) {
        return reinterpret_cast<report_trailer_body*>(buffer);
    }

    // parse method const
    static const report_trailer_body* parse(const std::byte* buffer) {
        return reinterpret_cast<const report_trailer_body*>(buffer);
    }
};

// layout verification
static_assert(offsetof(report_trailer_body, number_of_packets) == 0, "unexpected offset of report_trailer_body::number_of_packets");
static_assert(offsetof(report_trailer_body, reserved_1) == 4, "unexpected offset of report_trailer_body::reserved_1");
static_assert(sizeof(report_trailer_body) == 5, "unexpected sizeof report_trailer_body");

#pragma pack(pop)
}
