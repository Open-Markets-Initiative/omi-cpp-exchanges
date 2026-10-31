#pragma once

#include "AiqCanceledMessageWriter.hpp"
#include <optional>
#include <stdexcept>
#include <string_view>

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

namespace ouch_orders = ::nasdaq::nsmequities::orders::ouch::v5_0;


inline external_count_iterator<aiq_canceled_message_aiq_canceled_appendage_entry> read_aiq_canceled_appendage(const aiq_canceled_message& msg) {
    return { msg.tail_begin(), msg.fields.appendage_length.get().value(), msg.tail_end() };
}

}
