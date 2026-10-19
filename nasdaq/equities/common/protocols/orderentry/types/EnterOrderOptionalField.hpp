#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace nasdaq::nsmequities::orders::ouch::v5_0 {

// enter_order_optional_field
struct enter_order_optional_field {

    enum class enum_type : std::uint8_t {
        firm = 2,
        minqty = 3,
        customertype = 4,
        maxfloor = 5,
        pricetype = 6,
        pegoffset = 7,
        discretionprice = 9,
        discretionpricetype = 10,
        discretionpegoffset = 11,
        postonly = 12,
        randomreserves = 13,
        expiretime = 15,
        tradenow = 16,
        handleinst = 17,
        group_id = 24,
        shares_located = 25,
        locate_broker = 26,
        userrefidx = 28,
        aiq_strategy = 29,
        aiq_group_id = 30
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 20> from_string_map = {{
        {"AIQ Group ID", enum_type::aiq_group_id},
        {"AIQ Strategy", enum_type::aiq_strategy},
        {"CustomerType", enum_type::customertype},
        {"DiscretionPegOffset", enum_type::discretionpegoffset},
        {"DiscretionPrice", enum_type::discretionprice},
        {"DiscretionPriceType", enum_type::discretionpricetype},
        {"ExpireTime", enum_type::expiretime},
        {"Firm", enum_type::firm},
        {"Group ID", enum_type::group_id},
        {"HandleInst", enum_type::handleinst},
        {"Locate Broker", enum_type::locate_broker},
        {"MaxFloor", enum_type::maxfloor},
        {"MinQty", enum_type::minqty},
        {"PegOffset", enum_type::pegoffset},
        {"PostOnly", enum_type::postonly},
        {"PriceType", enum_type::pricetype},
        {"RandomReserves", enum_type::randomreserves},
        {"Shares Located", enum_type::shares_located},
        {"TradeNow", enum_type::tradenow},
        {"UserRefIdx", enum_type::userrefidx}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::firm: return "Firm";
            case enum_type::minqty: return "MinQty";
            case enum_type::customertype: return "CustomerType";
            case enum_type::maxfloor: return "MaxFloor";
            case enum_type::pricetype: return "PriceType";
            case enum_type::pegoffset: return "PegOffset";
            case enum_type::discretionprice: return "DiscretionPrice";
            case enum_type::discretionpricetype: return "DiscretionPriceType";
            case enum_type::discretionpegoffset: return "DiscretionPegOffset";
            case enum_type::postonly: return "PostOnly";
            case enum_type::randomreserves: return "RandomReserves";
            case enum_type::expiretime: return "ExpireTime";
            case enum_type::tradenow: return "TradeNow";
            case enum_type::handleinst: return "HandleInst";
            case enum_type::group_id: return "Group ID";
            case enum_type::shares_located: return "Shares Located";
            case enum_type::locate_broker: return "Locate Broker";
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

    static constexpr const char* name = "enter_order_optional_field";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<enter_order_optional_field::enum_type>;
    using storage_type = result_type;

    constexpr enter_order_optional_field()
     : value{ enum_type{} } {}

    constexpr enter_order_optional_field(enum_type v)
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
