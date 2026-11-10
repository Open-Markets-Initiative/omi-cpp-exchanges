#pragma once

#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <string>

namespace sim {

// Captures raw IEX-TP segments published by a FeedServer. One instance per feed.
class MarketDataLog {
public:
    explicit MarketDataLog(const std::string& path) {
        if (path.empty()) return;
        auto parent = std::filesystem::path(path).parent_path();
        if (!parent.empty()) std::filesystem::create_directories(parent);
        file_ = std::fopen(path.c_str(), "wb");
    }
    ~MarketDataLog() { if (file_) std::fclose(file_); }

    MarketDataLog(const MarketDataLog&)            = delete;
    MarketDataLog& operator=(const MarketDataLog&) = delete;

    // FeedServer Publisher hook: append one full segment, verbatim.
    void operator()(const char* segment, std::size_t len) {
        if (file_) std::fwrite(segment, 1, len, file_);
    }

private:
    std::FILE* file_ = nullptr;
};

}