#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <mutex>
#include <string>
#include <string_view>

namespace sim {

// Fixed-size bracketed ASCII record header:  [D TTTT SSSSSSSSS LLLLL]<space>
//   '['  direction(1)  msg_type(right-justified 4)  ' '  seq(9)  ' '  length(5)  ']'  ' '
// = 24 bytes, fixed. The message (length bytes) is memcpy'd immediately after, then '\n'.
class FixLog {
public:
    static constexpr std::size_t kHeader = 24;

    explicit FixLog(const std::string& path) {
        if (path.empty()) return;
        auto parent = std::filesystem::path(path).parent_path();
        if (!parent.empty()) std::filesystem::create_directories(parent);
        file_ = std::fopen(path.c_str(), "wb");
    }
    ~FixLog() { if (file_) std::fclose(file_); }

    FixLog(const FixLog&)            = delete;
    FixLog& operator=(const FixLog&) = delete;

    // Assemble [header][raw message]['\n'] in one buffer; single fwrite. The message bytes
    // are memcpy'd straight into place after the fixed header.
    void record(char direction, std::uint32_t sequence, char msg_type, std::string_view msg) {
        if (!file_) return;

        std::size_t len = msg.size() < kMaxMessage ? msg.size() : kMaxMessage;

        char buffer[kHeader + kMaxMessage + 1];
        char* p = buffer;

        *p++ = '[';
        *p++ = direction;
        *p++ = ' '; *p++ = ' '; *p++ = ' '; *p++ = msg_type;   // msg type right-justified in 4
        *p++ = ' ';
        put_u(p, sequence, 9);                       p += 9;
        *p++ = ' ';
        put_u(p, static_cast<std::uint32_t>(len), 5); p += 5;
        *p++ = ']';
        *p++ = ' ';

        std::memcpy(p, msg.data(), len);             p += len;
        *p++ = '\n';

        std::fwrite(buffer, 1, static_cast<std::size_t>(p - buffer), file_);
    }

private:
    // Write `value` right-justified, zero-padded, into dst[0..width). No division library
    // calls beyond the compiler's; digits filled back-to-front.
    static void put_u(char* dst, std::uint32_t value, int width) {
        for (int i = width - 1; i >= 0; --i) { dst[i] = static_cast<char>('0' + value % 10); value /= 10; }
    }

    static constexpr std::size_t kMaxMessage = 8192;
    std::FILE* file_ = nullptr;
    std::mutex lock_;
};

}