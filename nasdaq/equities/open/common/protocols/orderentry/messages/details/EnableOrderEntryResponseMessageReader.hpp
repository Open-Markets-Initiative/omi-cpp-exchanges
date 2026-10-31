#pragma once

#include "EnableOrderEntryResponseMessageWriter.hpp"
#include <optional>
#include <stdexcept>
#include <string_view>

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

namespace ouch_orders = ::nasdaq::nsmequities::orders::ouch::v5_0;


inline external_count_iterator<enable_order_entry_response_message_enable_order_entry_response_appendage_entry> read_enable_order_entry_response_appendage(const enable_order_entry_response_message& msg) {
    return { msg.tail_begin(), msg.fields.appendage_length.get().value(), msg.tail_end() };
}

}
