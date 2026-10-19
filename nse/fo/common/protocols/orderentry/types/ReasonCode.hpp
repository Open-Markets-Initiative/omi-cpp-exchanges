#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nse::nsefo::orderentry::nnf::v9_50 {

// reason_code
struct reason_code {

    enum class enum_type : std::int16_t {
        exercise = 2,
        position_liquidation = 3,
        security = 5,
        broker = 6,
        branch = 7,
        user = 8,
        participant = 9,
        counter_party = 10,
        order_number = 11,
        market_status = 12,
        auction_number = 15,
        order_type = 16,
        price_freeze = 17,
        quantity_freeze = 18,
        contract = 20,
        exercise_mode_mismatch = 30
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 16> from_string_map = {{
        {"Auction Number", enum_type::auction_number},
        {"Branch", enum_type::branch},
        {"Broker", enum_type::broker},
        {"Contract", enum_type::contract},
        {"Counter Party", enum_type::counter_party},
        {"Exercise", enum_type::exercise},
        {"Exercise Mode Mismatch", enum_type::exercise_mode_mismatch},
        {"Market Status", enum_type::market_status},
        {"Order Number", enum_type::order_number},
        {"Order Type", enum_type::order_type},
        {"Participant", enum_type::participant},
        {"Position Liquidation", enum_type::position_liquidation},
        {"Price Freeze", enum_type::price_freeze},
        {"Quantity Freeze", enum_type::quantity_freeze},
        {"Security", enum_type::security},
        {"User", enum_type::user}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::exercise: return "Exercise";
            case enum_type::position_liquidation: return "Position Liquidation";
            case enum_type::security: return "Security";
            case enum_type::broker: return "Broker";
            case enum_type::branch: return "Branch";
            case enum_type::user: return "User";
            case enum_type::participant: return "Participant";
            case enum_type::counter_party: return "Counter Party";
            case enum_type::order_number: return "Order Number";
            case enum_type::market_status: return "Market Status";
            case enum_type::auction_number: return "Auction Number";
            case enum_type::order_type: return "Order Type";
            case enum_type::price_freeze: return "Price Freeze";
            case enum_type::quantity_freeze: return "Quantity Freeze";
            case enum_type::contract: return "Contract";
            case enum_type::exercise_mode_mismatch: return "Exercise Mode Mismatch";
            default: return "unknown";
        }
    }

    static constexpr std::optional<enum_type> from_string(std::string_view str) {
        auto it = std::lower_bound(
            from_string_map.begin(),
            from_string_map.end(),
            str,
            [](const auto& pair, std::string_view s) { return pair.first < s; }
        );
        if (it != from_string_map.end() && it->first == str) {
            return it->second;
        }
        return std::nullopt;
    }

    static constexpr const char* name = "reason_code";
    static constexpr std::size_t size = 2;
    static constexpr bool is_optional = false;

    using result_type = required<reason_code::enum_type>;
    using storage_type = result_type;

    constexpr reason_code()
     : value{ enum_type{} } {}

    constexpr reason_code(enum_type v)
     : value{ v } {}

    [[nodiscard]] constexpr result_type get() const {
        return result_type{value};
    }

    constexpr void set(enum_type v) {
        value = v;
    }

    constexpr void set(result_type value) {
        if (value.has_value())
            set(value.value());
        else
            set(enum_type::exercise_mode_mismatch);
    }

  protected:
    enum_type value;
};
}
