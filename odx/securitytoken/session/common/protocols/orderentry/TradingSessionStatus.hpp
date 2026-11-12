#pragma once

#include <cstdint>
#include "FixTags.hpp"

namespace odxodxsecuritytoken::order_entry {

#pragma pack(push, 1)
struct TradingSessionStatus {
    char trading_session_id[256];
    int32_t trad_ses_mode;
    int32_t trad_ses_status;
};
#pragma pack(pop)

}  // namespace odxodxsecuritytoken::order_entry
