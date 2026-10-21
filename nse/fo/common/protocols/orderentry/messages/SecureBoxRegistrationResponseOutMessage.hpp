#pragma once

#include <cstdint>

namespace nse::nsefo::orderentry::nnf::v9_50 {

// Secure Box Registration Response Out Message
struct secure_box_registration_response_out_message {

    static constexpr auto name = "Secure Box Registration Response Out Message";

    // selected when transaction_code == 23009
    static constexpr auto field = "transaction_code";
    static constexpr std::int16_t value = 23009;
};

}
