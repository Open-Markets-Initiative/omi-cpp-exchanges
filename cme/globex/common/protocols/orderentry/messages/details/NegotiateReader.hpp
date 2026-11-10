#pragma once

#include "NegotiateWriter.hpp"
#include <optional>
#include <stdexcept>
#include <string_view>

namespace cme::ilink3::v9 {

namespace sbe_ilink3 = ::cme::ilink3::v9;


inline sbe_var_data<std::uint16_t> read_credentials(const negotiate& msg) {
    return { msg.tail_begin(), msg.tail_end() };
}

}
