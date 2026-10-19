#pragma once

#include "OrderCancellationConfirmationMessageWriter.hpp"
#include <optional>
#include <stdexcept>
#include <string_view>

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;


inline sbe_var_data read_cancellation_confirmation_body(const order_cancellation_confirmation_message& msg) {
    return { msg.tail_begin(), msg.tail_end() };
}

}
