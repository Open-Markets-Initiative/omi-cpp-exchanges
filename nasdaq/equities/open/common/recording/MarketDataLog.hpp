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
        path_ = path;
    }
    ~MarketDataLog() { if (file_) std::fclose(file_); if (marks_) std::fclose(marks_); }

    MarketDataLog(const MarketDataLog&)            = delete;
    MarketDataLog& operator=(const MarketDataLog&) = delete;

    // FeedServer Publisher hook: append one full segment, verbatim.
    void operator()(const char* segment, std::size_t len) {
        if (file_) std::fwrite(segment, 1, len, file_);
        ++segments_;
        bytes_ += len;
    }

    // The engine's book was said as of a request: where the feed stood when it heard the end of that
    // answer, written beside the recording as a line "request,segments,bytes" - the request's place
    // in its sequencer's order, how many segments the feed had published, and how many bytes of the
    // recording they are. The book the engine said is the book as of that point in this feed.
    void mark(unsigned long long request) {
        if (path_.empty()) return;
        if (marks_ == nullptr) {
            marks_ = std::fopen((path_ + ".books").c_str(), "w");
            if (marks_ == nullptr) return;
            std::fprintf(marks_, "request,segments,bytes\n");
        }
        std::fprintf(marks_, "%llu,%llu,%llu\n", request, segments_, bytes_);
        std::fflush(marks_);
    }

private:
    std::FILE* file_ = nullptr;
    std::FILE* marks_ = nullptr;          // where the feed stood at each answer of the engine's book
    std::string path_;
    unsigned long long segments_ = 0;     // the segments published so far
    unsigned long long bytes_ = 0;        // and their bytes
};

}