#pragma once

#include <cstdint>

namespace nse::nsefo::orderentry::nnf::v9_50 {

// Sign Off Request In Message
struct sign_off_request_in_message {

    static constexpr auto name = "Sign Off Request In Message";

    // selected when transaction_code == 2320
    static constexpr auto field = "transaction_code";
    static constexpr std::int16_t value = 2320;

    // action properties
    static constexpr auto session = "Logout";
    static constexpr auto origin = "Firm";
};

}
