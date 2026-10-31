#pragma once

#include "CancelRejectMessageWriter.hpp"
#include <optional>
#include <stdexcept>
#include <string_view>

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

namespace ouch_orders = ::nasdaq::nsmequities::orders::ouch::v5_0;


inline external_count_iterator<cancel_reject_message_cancel_reject_appendage_entry> read_cancel_reject_appendage(const cancel_reject_message& msg) {
    return { msg.tail_begin(), msg.fields.appendage_length.get().value(), msg.tail_end() };
}

}
