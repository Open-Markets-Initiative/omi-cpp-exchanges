#pragma once

#include <cstddef>
#include "../structs/MessageHeader.hpp"
#include "../types/BrokerId.hpp"
#include "../types/UserId.hpp"
#include "../types/OrdQtyBuff.hpp"
#include "../types/OrdValBuff.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;

#pragma pack(push, 1)

// Dealer Limit Update Message
struct dealer_limit_update_message {

    struct fields_type {
        nnf_orderentry::broker_id broker_id;
        nnf_orderentry::user_id user_id;
        nnf_orderentry::ord_qty_buff ord_qty_buff;
        nnf_orderentry::ord_val_buff ord_val_buff;
    };

    nnf_orderentry::message_header header = {transaction_code::enum_type::dealer_limit_update_message, {}, {}, {}, {}, {}, {}, {}, std::uint16_t(sizeof(nnf_orderentry::message_header) + sizeof(fields_type))};

    fields_type fields;

    // parse method
    static dealer_limit_update_message* parse(std::byte* buffer) {
        return reinterpret_cast<dealer_limit_update_message*>(buffer);
    }

    // parse method const
    static const dealer_limit_update_message* parse(const std::byte* buffer) {
        return reinterpret_cast<const dealer_limit_update_message*>(buffer);
    }

};

// layout verification
static_assert(offsetof(dealer_limit_update_message::fields_type, broker_id) == 0, "unexpected offset of dealer_limit_update_message::fields_type::broker_id");
static_assert(offsetof(dealer_limit_update_message::fields_type, user_id) == 5, "unexpected offset of dealer_limit_update_message::fields_type::user_id");
static_assert(offsetof(dealer_limit_update_message::fields_type, ord_qty_buff) == 9, "unexpected offset of dealer_limit_update_message::fields_type::ord_qty_buff");
static_assert(offsetof(dealer_limit_update_message::fields_type, ord_val_buff) == 17, "unexpected offset of dealer_limit_update_message::fields_type::ord_val_buff");
static_assert(sizeof(dealer_limit_update_message::fields_type) == 25, "unexpected sizeof dealer_limit_update_message::fields_type");
static_assert(sizeof(dealer_limit_update_message) == sizeof(nnf_orderentry::message_header) + 25, "unexpected sizeof dealer_limit_update_message");

#pragma pack(pop)
}
