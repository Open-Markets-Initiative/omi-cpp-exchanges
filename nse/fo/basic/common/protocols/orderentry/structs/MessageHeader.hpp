#pragma once

#include <cstddef>
#include "../types/TransactionCode.hpp"
#include "../types/LogTime.hpp"
#include "../types/AlphaChar.hpp"
#include "../types/TraderId.hpp"
#include "../types/ErrorCode.hpp"
#include "../types/HeaderTimestamp.hpp"
#include "../types/TimeStamp1.hpp"
#include "../types/TimeStamp2.hpp"
#include "../types/MessageLength.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

struct message_header {

    nnf_orderentry::transaction_code transaction_code;
    nnf_orderentry::log_time log_time;
    nnf_orderentry::alpha_char alpha_char;
    nnf_orderentry::trader_id trader_id;
    nnf_orderentry::error_code error_code;
    nnf_orderentry::header_timestamp header_timestamp;
    nnf_orderentry::time_stamp_1 time_stamp_1;
    nnf_orderentry::time_stamp_2 time_stamp_2;
    nnf_orderentry::message_length message_length;

    // parse method
    static message_header* parse(std::byte* buffer) {
        return reinterpret_cast<message_header*>(buffer);
    }

    // parse method const
    static const message_header* parse(const std::byte* buffer) {
        return reinterpret_cast<const message_header*>(buffer);
    }
};

// layout verification
static_assert(offsetof(message_header, transaction_code) == 0, "unexpected offset of message_header::transaction_code");
static_assert(offsetof(message_header, log_time) == 2, "unexpected offset of message_header::log_time");
static_assert(offsetof(message_header, alpha_char) == 6, "unexpected offset of message_header::alpha_char");
static_assert(offsetof(message_header, trader_id) == 8, "unexpected offset of message_header::trader_id");
static_assert(offsetof(message_header, error_code) == 12, "unexpected offset of message_header::error_code");
static_assert(offsetof(message_header, header_timestamp) == 14, "unexpected offset of message_header::header_timestamp");
static_assert(offsetof(message_header, time_stamp_1) == 22, "unexpected offset of message_header::time_stamp_1");
static_assert(offsetof(message_header, time_stamp_2) == 30, "unexpected offset of message_header::time_stamp_2");
static_assert(offsetof(message_header, message_length) == 38, "unexpected offset of message_header::message_length");
static_assert(sizeof(message_header) == 40, "unexpected sizeof message_header");

#pragma pack(pop)
}
