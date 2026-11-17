#pragma once

#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include "../cache/Required.hpp"

namespace iex::iexequities::matchingengine::ipc::v2026 {

// book_place
struct book_place {

    enum class enum_type : char {
        book = 'B',
        held = 'H',
        parked = 'P',
        queued = 'Q',
        waiting = 'W'
    };

    static constexpr std::array<std::pair<std::string_view, enum_type>, 5> from_string_map = {{
        {"Book", enum_type::book},
        {"Held", enum_type::held},
        {"Parked", enum_type::parked},
        {"Queued", enum_type::queued},
        {"Waiting", enum_type::waiting}
    }};

    static constexpr std::string_view to_string(enum_type value) {
        switch (value) {
            case enum_type::book: return "Book";
            case enum_type::held: return "Held";
            case enum_type::parked: return "Parked";
            case enum_type::queued: return "Queued";
            case enum_type::waiting: return "Waiting";
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

    static constexpr const char* name = "book_place";
    static constexpr std::size_t size = 1;
    static constexpr bool is_optional = false;

    using result_type = required<book_place::enum_type>;
    using storage_type = result_type;

    constexpr book_place()
     : value{ enum_type{} } {}

    constexpr book_place(enum_type v)
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
            set(enum_type::waiting);
    }

  protected:
    enum_type value;
};
}
