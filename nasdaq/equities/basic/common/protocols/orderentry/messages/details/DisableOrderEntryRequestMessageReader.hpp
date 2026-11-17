#pragma once

#include "DisableOrderEntryRequestMessageWriter.hpp"
#include <optional>
#include <stdexcept>
#include <string_view>

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

namespace ouch_orders = ::nasdaq::nsmequities::orders::ouch::v5_0;


inline external_count_iterator<disable_order_entry_request_message_disable_order_entry_request_appendage_entry> read_disable_order_entry_request_appendage(const disable_order_entry_request_message& msg) {
    return { msg.tail_begin(), msg.fields.appendage_length.get().value(), msg.tail_end() };
}

}
