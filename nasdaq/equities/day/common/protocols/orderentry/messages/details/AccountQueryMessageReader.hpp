#pragma once

#include "AccountQueryMessageWriter.hpp"
#include <optional>
#include <stdexcept>
#include <string_view>

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

namespace ouch_orders = ::nasdaq::nsmequities::orders::ouch::v5_0;


inline external_count_iterator<account_query_message_account_query_appendage_entry> read_account_query_appendage(const account_query_message& msg) {
    return { msg.tail_begin(), msg.fields.appendage_length.get().value(), msg.tail_end() };
}

}
