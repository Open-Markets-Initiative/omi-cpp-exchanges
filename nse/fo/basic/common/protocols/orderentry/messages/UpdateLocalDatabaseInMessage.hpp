#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/LastUpdateSecurityTime.hpp"
#include "../types/LastUpdateParticipantTime.hpp"
#include "../types/LastUpdateInstrumentTime.hpp"
#include "../types/LastUpdateIndexTime.hpp"
#include "../types/RequestForOpenOrders.hpp"
#include "../types/Reserved1.hpp"
#include "../structs/StMarketStatus.hpp"
#include "../structs/StExMarketStatus.hpp"
#include "../structs/StPlMarketStatus.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Update Local Database In Message
struct update_local_database_in_message {

    struct fields_type {
        nnf_orderentry::last_update_security_time last_update_security_time;
        nnf_orderentry::last_update_participant_time last_update_participant_time;
        nnf_orderentry::last_update_instrument_time last_update_instrument_time;
        nnf_orderentry::last_update_index_time last_update_index_time;
        nnf_orderentry::request_for_open_orders request_for_open_orders;
        nnf_orderentry::reserved_1 reserved_1;
        nnf_orderentry::st_market_status st_market_status;
        nnf_orderentry::st_ex_market_status st_ex_market_status;
        nnf_orderentry::st_pl_market_status st_pl_market_status;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::update_local_database_in_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static update_local_database_in_message* parse(std::byte* buffer) {
        return reinterpret_cast<update_local_database_in_message*>(buffer);
    }

    // parse method const
    static const update_local_database_in_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const update_local_database_in_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(update_local_database_in_message::fields_type, last_update_security_time) == 0, "unexpected offset of update_local_database_in_message::fields_type::last_update_security_time");
static_assert(offsetof(update_local_database_in_message::fields_type, last_update_participant_time) == 4, "unexpected offset of update_local_database_in_message::fields_type::last_update_participant_time");
static_assert(offsetof(update_local_database_in_message::fields_type, last_update_instrument_time) == 8, "unexpected offset of update_local_database_in_message::fields_type::last_update_instrument_time");
static_assert(offsetof(update_local_database_in_message::fields_type, last_update_index_time) == 12, "unexpected offset of update_local_database_in_message::fields_type::last_update_index_time");
static_assert(offsetof(update_local_database_in_message::fields_type, request_for_open_orders) == 16, "unexpected offset of update_local_database_in_message::fields_type::request_for_open_orders");
static_assert(offsetof(update_local_database_in_message::fields_type, reserved_1) == 17, "unexpected offset of update_local_database_in_message::fields_type::reserved_1");
static_assert(offsetof(update_local_database_in_message::fields_type, st_market_status) == 18, "unexpected offset of update_local_database_in_message::fields_type::st_market_status");
static_assert(offsetof(update_local_database_in_message::fields_type, st_ex_market_status) == 26, "unexpected offset of update_local_database_in_message::fields_type::st_ex_market_status");
static_assert(offsetof(update_local_database_in_message::fields_type, st_pl_market_status) == 34, "unexpected offset of update_local_database_in_message::fields_type::st_pl_market_status");
static_assert(sizeof(update_local_database_in_message::fields_type) == 42, "unexpected sizeof update_local_database_in_message::fields_type");
static_assert(sizeof(update_local_database_in_message) == sizeof(nnf_orderentry::message_header) + 42, "unexpected sizeof update_local_database_in_message");

#pragma pack(pop)
}
