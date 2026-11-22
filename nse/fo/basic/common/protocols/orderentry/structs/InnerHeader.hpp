#pragma once

#include <cstddef>
#include "../types/InnerTraderId.hpp"
#include "../types/InnerLogTime.hpp"
#include "../types/InnerAlphaChar.hpp"
#include "../types/InnerTransactionCode.hpp"
#include "../types/InnerErrorCode.hpp"
#include "../types/InnerTimestamp.hpp"
#include "../types/InnerTimeStamp1.hpp"
#include "../types/InnerTimeStamp2.hpp"
#include "../types/InnerMessageLength.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct inner_header {

    nnf_orderentry::inner_trader_id inner_trader_id;
    nnf_orderentry::inner_log_time inner_log_time;
    nnf_orderentry::inner_alpha_char inner_alpha_char;
    nnf_orderentry::inner_transaction_code inner_transaction_code;
    nnf_orderentry::inner_error_code inner_error_code;
    nnf_orderentry::inner_timestamp inner_timestamp;
    nnf_orderentry::inner_time_stamp_1 inner_time_stamp_1;
    nnf_orderentry::inner_time_stamp_2 inner_time_stamp_2;
    nnf_orderentry::inner_message_length inner_message_length;

    // parse method
    static inner_header* parse(std::byte* buffer) {
        return reinterpret_cast<inner_header*>(buffer);
    }

    // parse method const
    static const inner_header* parse(const std::byte* buffer) {
        return reinterpret_cast<const inner_header*>(buffer);
    }
};

// layout verification
static_assert(offsetof(inner_header, inner_trader_id) == 0, "unexpected offset of inner_header::inner_trader_id");
static_assert(offsetof(inner_header, inner_log_time) == 4, "unexpected offset of inner_header::inner_log_time");
static_assert(offsetof(inner_header, inner_alpha_char) == 8, "unexpected offset of inner_header::inner_alpha_char");
static_assert(offsetof(inner_header, inner_transaction_code) == 10, "unexpected offset of inner_header::inner_transaction_code");
static_assert(offsetof(inner_header, inner_error_code) == 12, "unexpected offset of inner_header::inner_error_code");
static_assert(offsetof(inner_header, inner_timestamp) == 14, "unexpected offset of inner_header::inner_timestamp");
static_assert(offsetof(inner_header, inner_time_stamp_1) == 22, "unexpected offset of inner_header::inner_time_stamp_1");
static_assert(offsetof(inner_header, inner_time_stamp_2) == 30, "unexpected offset of inner_header::inner_time_stamp_2");
static_assert(offsetof(inner_header, inner_message_length) == 38, "unexpected offset of inner_header::inner_message_length");
static_assert(sizeof(inner_header) == 40, "unexpected sizeof inner_header");

#pragma pack(pop)
}
