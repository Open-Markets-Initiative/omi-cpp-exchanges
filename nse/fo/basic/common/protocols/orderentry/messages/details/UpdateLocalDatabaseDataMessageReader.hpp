#pragma once

#include "UpdateLocalDatabaseDataMessageWriter.hpp"
#include <optional>
#include <stdexcept>
#include <string_view>

namespace nse::nsefo::orderentry::nnf::v9_50 {

namespace nnf_orderentry = ::nse::nsefo::orderentry::nnf::v9_50;


inline external_var_data read_local_database_data(const update_local_database_data_message& msg) {
    return { msg.tail_begin(), static_cast<std::size_t>(msg.header.message_length.get().value() - 80), msg.tail_end() };
}

}
