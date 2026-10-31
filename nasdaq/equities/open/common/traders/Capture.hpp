#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>

namespace sim::traders {

// A classic libpcap capture, read once at start: every packet's transport payload, in order.
// A pcapng file is refused rather than half-read, and a capture that cannot be read leaves the
// trader with nothing to say rather than stopping the venue.
class Capture {
public:
    explicit Capture(const std::string& path) { read(path); }

    bool                            ok()       const { return error_.empty(); }
    const std::string&              error()    const { return error_; }
    const std::vector<std::string>& payloads() const { return payloads_; }

private:
    void read(const std::string& path) {
        std::FILE* file = std::fopen(path.c_str(), "rb");
        if (!file) { error_ = "cannot be opened"; return; }

        unsigned char global[24];
        if (std::fread(global, 1, sizeof global, file) != sizeof global) { error_ = "is too short to be a capture"; std::fclose(file); return; }

        const std::uint32_t magic = read_u32(global, false);
        if (magic == 0x0a0d0d0a) { error_ = "is a pcapng capture, and only the classic format is read here"; std::fclose(file); return; }

        bool swapped = false;
        if      (magic == 0xa1b2c3d4 || magic == 0xa1b23c4d) swapped = false;
        else if (magic == 0xd4c3b2a1 || magic == 0x4d3cb2a1) swapped = true;
        else { error_ = "is not a libpcap capture"; std::fclose(file); return; }

        const std::uint32_t link = read_u32(global + 20, swapped);

        for (;;) {
            unsigned char record[16];
            if (std::fread(record, 1, sizeof record, file) != sizeof record) break;
            const std::uint32_t captured = read_u32(record + 8, swapped);
            if (captured == 0 || captured > (1u << 20)) break;

            std::string frame(captured, '\0');
            if (std::fread(frame.data(), 1, captured, file) != captured) break;

            std::string_view payload = transport(std::string_view(frame), link);
            if (!payload.empty()) payloads_.emplace_back(payload);
        }
        std::fclose(file);

        if (payloads_.empty() && error_.empty()) error_ = "holds no packets this venue's transport carries";
    }

    // The payload the protocol sits in: past the link header, the IPv4 header, and the Udp or Tcp one.
    static std::string_view transport(std::string_view frame, std::uint32_t link) {
        std::size_t at = 0;
        switch (link) {
            case 1: {   // Ethernet
                if (frame.size() < 14) return {};
                std::uint16_t ether = static_cast<std::uint8_t>(frame[12]) << 8 | static_cast<std::uint8_t>(frame[13]);
                at = 14;
                while (ether == 0x8100 || ether == 0x88a8) {   // tagged
                    if (frame.size() < at + 4) return {};
                    ether = static_cast<std::uint8_t>(frame[at + 2]) << 8 | static_cast<std::uint8_t>(frame[at + 3]);
                    at += 4;
                }
                if (ether != 0x0800) return {};   // IPv4 only
                break;
            }
            case 0:   at = 4;  break;   // BSD loopback
            case 101: at = 0;  break;   // raw IP
            case 113: at = 16; break;   // Linux cooked
            default:  return {};
        }

        if (frame.size() < at + 20) return {};
        const auto ihl = static_cast<std::size_t>(frame[at] & 0x0f) * 4;
        const auto protocol = static_cast<std::uint8_t>(frame[at + 9]);
        if (frame.size() < at + ihl) return {};
        at += ihl;

        if (protocol == 17) {                      // Udp
            if (frame.size() < at + 8) return {};
            return frame.substr(at + 8);
        }
        if (protocol == 6) {                       // Tcp
            if (frame.size() < at + 20) return {};
            const auto offset = (static_cast<std::uint8_t>(frame[at + 12]) >> 4) * 4;
            if (frame.size() < at + static_cast<std::size_t>(offset)) return {};
            return frame.substr(at + static_cast<std::size_t>(offset));
        }
        return {};
    }

    static std::uint32_t read_u32(const unsigned char* at, bool swapped) {
        const std::uint32_t value = static_cast<std::uint32_t>(at[0]) | static_cast<std::uint32_t>(at[1]) << 8
                                  | static_cast<std::uint32_t>(at[2]) << 16 | static_cast<std::uint32_t>(at[3]) << 24;
        return swapped ? __builtin_bswap32(value) : value;
    }

    std::string              error_;
    std::vector<std::string> payloads_;
};}
