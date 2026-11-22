#pragma once

#include "GatewayRouterResponseMessageWriter.hpp"
#include <optional>
#include <stdexcept>
#include <string_view>

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;


inline sbe_var_data<std::uint32_t> read_cryptographic_tail(const gateway_router_response_message& msg) {
    return { msg.tail_begin(), msg.tail_end() };
}

}
