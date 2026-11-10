#pragma once

#include <charconv>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string_view>

#include "FixWire.hpp"

namespace iexiexequities::order_entry {

// Writes tag=value fields, each closed by SOH, into a buffer it will not run past:
// a message that would not fit is marked and not sent, which only a field echoed
// back from what arrived, or a configured one, can make happen.
struct Writer {
    char* at;
    char* end;
    bool overflowed = false;

    void raw(std::string_view text) {
        if (static_cast<std::size_t>(end - at) < text.size()) {
            overflowed = true;
            return;
        }

        std::memcpy(at, text.data(), text.size());
        at += text.size();
    }

    void put(const std::uint32_t tag, std::string_view value) {
        char number[16];
        const auto written = std::to_chars(number, number + sizeof number, tag);

        raw(std::string_view{number, static_cast<std::size_t>(written.ptr - number)});
        raw("=");
        raw(value);
        raw("\x01");
    }

    void put(const std::uint32_t tag, const std::int64_t value) {
        char number[24];
        const auto written = std::to_chars(number, number + sizeof number, value);

        put(tag, std::string_view{number, static_cast<std::size_t>(written.ptr - number)});
    }

    void put(const std::uint32_t tag, const bool value) { put(tag, std::string_view{value ? "Y" : "N"}); }

    void put(const std::uint32_t tag, const char value) { put(tag, std::string_view{&value, 1}); }

    // A UTCTimestamp, YYYYMMDD-HH:MM:SS.sss, from nanoseconds since the epoch: the
    // civil date by Howard Hinnant's days-to-civil algorithm.
    void put_time(const std::uint32_t tag, const std::int64_t ns) {
        std::int64_t days = ns / 86'400'000'000'000;
        std::int64_t in_day = ns % 86'400'000'000'000 / 1'000'000;

        if (in_day < 0) {
            in_day += 86'400'000;
            --days;
        }

        const std::int64_t shifted = days + 719'468;
        const std::int64_t era = (shifted >= 0 ? shifted : shifted - 146'096) / 146'097;
        const std::int64_t of_era = shifted - era * 146'097;
        const std::int64_t year_of_era = (of_era - of_era / 1'460 + of_era / 36'524 - of_era / 146'096) / 365;
        const std::int64_t of_year = of_era - (365 * year_of_era + year_of_era / 4 - year_of_era / 100);
        const std::int64_t shifted_month = (5 * of_year + 2) / 153;
        const std::int64_t day = of_year - (153 * shifted_month + 2) / 5 + 1;
        const std::int64_t month = shifted_month < 10 ? shifted_month + 3 : shifted_month - 9;
        const std::int64_t year = year_of_era + era * 400 + (month <= 2 ? 1 : 0);

        char text[21];
        digits(text, year, 4);
        digits(text + 4, month, 2);
        digits(text + 6, day, 2);
        text[8] = '-';
        digits(text + 9, in_day / 3'600'000, 2);
        text[11] = ':';
        digits(text + 12, in_day / 60'000 % 60, 2);
        text[14] = ':';
        digits(text + 15, in_day / 1'000 % 60, 2);
        text[17] = '.';
        digits(text + 18, in_day % 1'000, 3);

        put(tag, std::string_view{text, sizeof text});
    }

    static void digits(char* at, std::int64_t value, const int width) {
        for (int place = width - 1; place >= 0; --place) {
            at[place] = static_cast<char>('0' + value % 10);
            value /= 10;
        }
    }

    // A whole number of a decimal field's smallest unit, written as the decimal it stands
    // for: 1002500 at four places is "100.25".
    void put_scaled(const std::uint32_t tag, const std::int64_t value, const int places) {
        char number[32];
        std::int64_t unit = 1;

        for (int place = 0; place < places; ++place) {
            unit *= 10;
        }

        const std::int64_t whole = (value < 0 ? -value : value) / unit;
        std::int64_t fraction = (value < 0 ? -value : value) % unit;
        int shown = places;

        while (shown > 0 && fraction % 10 == 0) {
            fraction /= 10;
            --shown;
        }

        char* at_digit = number;

        if (value < 0) {
            *at_digit++ = '-';
        }

        at_digit = std::to_chars(at_digit, number + sizeof number, whole).ptr;

        if (shown > 0) {
            *at_digit++ = '.';
            digits(at_digit, fraction, shown);
            at_digit += shown;
        }

        put(tag, std::string_view{number, static_cast<std::size_t>(at_digit - number)});
    }

    std::size_t used(const char* begin) const { return static_cast<std::size_t>(at - begin); }
};

// A field of what arrived, by its tag: the text between "<SOH><tag>=" and the
// next SOH, or nothing when it does not carry it.
inline std::string_view field(std::string_view received, const std::uint32_t tag) {
    char key[16];
    key[0] = '\x01';
    const auto written = std::to_chars(key + 1, key + sizeof key - 1, tag);
    *written.ptr = '=';

    const std::string_view needle{key, static_cast<std::size_t>(written.ptr + 1 - key)};
    const auto at = received.find(needle);

    if (at == std::string_view::npos) {
        return {};
    }

    const auto begin = at + needle.size();
    const auto end = received.find('\x01', begin);

    return end == std::string_view::npos ? std::string_view{} : received.substr(begin, end - begin);
}

inline std::int64_t field_int(std::string_view received, const std::uint32_t tag) {
    const auto value = field(received, tag);
    std::int64_t number = 0;
    std::from_chars(value.data(), value.data() + value.size(), number);
    return number;
}

inline bool field_flag(std::string_view received, const std::uint32_t tag) { return field(received, tag) == "Y"; }

// A decimal field read as a whole number of its smallest unit: "100.25" at four places is
// 1002500. Digits past the places are dropped rather than rounded - a price finer than the
// field's tick is not one the venue takes.
inline std::int64_t field_scaled(std::string_view received, const std::uint32_t tag, const int places) {
    const auto value = field(received, tag);
    std::size_t at = 0;
    const bool negative = !value.empty() && value[0] == '-';
    std::int64_t whole = 0;
    int fraction = -1;

    for (at = negative ? 1 : 0; at < value.size(); ++at) {
        const char digit = value[at];

        if (digit == '.' && fraction < 0) {
            fraction = 0;
            continue;
        }

        if (digit < '0' || digit > '9' || fraction == places) {
            break;
        }

        whole = whole * 10 + (digit - '0');
        fraction += fraction >= 0 ? 1 : 0;
    }

    for (int place = fraction < 0 ? 0 : fraction; place < places; ++place) {
        whole *= 10;
    }

    return negative ? -whole : whole;
}

inline char field_char(std::string_view received, const std::uint32_t tag) {
    const auto value = field(received, tag);
    return value.empty() ? '\0' : value.front();
}

// The time a session is told, in nanoseconds since the epoch:
// what SendingTime is stamped with, and what its timers wait from.
struct SystemClock {
    std::int64_t operator()() const {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
};

}  // namespace iexiexequities::order_entry
