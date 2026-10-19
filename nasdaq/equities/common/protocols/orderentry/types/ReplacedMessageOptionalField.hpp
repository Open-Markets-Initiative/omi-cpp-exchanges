#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

// replaced_message_optional_field
struct replaced_message_optional_field {

    enum class enum_type : std::uint8_t {
        firm = 2,
        minqty = 3,
        maxfloor = 5,
        pricetype = 6,
        postonly = 12,
        expiretime = 15,
        tradenow = 16,
        handleinst = 17,
        bbo_weight_indicator = 18,
        userrefidx = 28,
        aiq_strategy = 29,
        aiq_group_id = 30
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 12> from_string_map = {{
        {"AIQ Group ID", enum_type::aiq_group_id},
        {"AIQ Strategy", enum_type::aiq_strategy},
        {"BBO Weight Indicator", enum_type::bbo_weight_indicator},
        {"ExpireTime", enum_type::expiretime},
        {"Firm", enum_type::firm},
        {"HandleInst", enum_type::handleinst},
        {"MaxFloor", enum_type::maxfloor},
        {"MinQty", enum_type::minqty},
        {"PostOnly", enum_type::postonly},
        {"PriceType", enum_type::pricetype},
        {"TradeNow", enum_type::tradenow},
        {"UserRefIdx", enum_type::userrefidx}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::firm: return "Firm";
            case enum_type::minqty: return "MinQty";
            case enum_type::maxfloor: return "MaxFloor";
            case enum_type::pricetype: return "PriceType";
            case enum_type::postonly: return "PostOnly";
            case enum_type::expiretime: return "ExpireTime";
            case enum_type::tradenow: return "TradeNow";
            case enum_type::handleinst: return "HandleInst";
            case enum_type::bbo_weight_indicator: return "BBO Weight Indicator";
            case enum_type::userrefidx: return "UserRefIdx";
            case enum_type::aiq_strategy: return "AIQ Strategy";
            case enum_type::aiq_group_id: return "AIQ Group ID";
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

    static constexpr const char* name = "replaced_message_optional_field";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<replaced_message_optional_field::enum_type>;
    using storage_type = result_type;

    constexpr replaced_message_optional_field()
     : value{ enum_type{} } {}

    constexpr replaced_message_optional_field(enum_type v)
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
            set(enum_type::aiq_group_id);
    }

  protected:
    enum_type value;
};
}
