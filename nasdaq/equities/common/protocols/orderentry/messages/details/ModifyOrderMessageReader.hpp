#pragma once

#include "ModifyOrderMessageWriter.hpp"
#include <optional>
#include <stdexcept>
#include <string_view>

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

namespace ouch_orders = ::nasdaq::nsmequities::orders::ouch::v5_0;


inline external_var_data read_modify_order_appendage(const modify_order_message& msg) {
    return { msg.tail_begin(), msg.fields.appendage_length.get().value(), msg.tail_end() };
}

}
