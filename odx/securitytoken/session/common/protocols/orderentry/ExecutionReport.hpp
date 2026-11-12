#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace odxodxsecuritytoken::order_entry {

#pragma pack(push, 1)
struct ExecutionReport {
    char account[256];
    double avg_px;
    char cl_ord_id[256];
    double cum_qty;
    char exec_id[256];
    char exec_trans_type;
    double last_px;
    double last_shares;
    char order_id[256];
    double order_qty;
    char ord_status;
    char ord_type;
    char orig_cl_ord_id[256];
    double price;
    char rule80_a;
    char side;
    char symbol[256];
    char text[256];
    char time_in_force;
    uint64_t transact_time;
    int32_t ord_rej_reason;
    char exec_type;
    double leaves_qty;
    int32_t exec_restatement_reason;
    char contra_broker[256];
    char trd_match_id[256];
};
#pragma pack(pop)

}  // namespace odxodxsecuritytoken::order_entry
