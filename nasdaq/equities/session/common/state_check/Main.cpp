// Generated Nasdaq Stock Market Session priority check: captures of the venue's TotalView feed, each order it showed
// held in the engine's book, and each execution it reported set beside the order the engine would have
// traded first.

#include "PriorityCheck.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>

namespace {

const char* const usage =
    "usage: %s [--port <udp port>] <capture.pcap | -> ...\n"
    "       a pcap of the feed as it was sent, Ethernet frames (a VLAN tag allowed) of IPv4 Udp; '-' reads\n"
    "       standard input, where several captures may follow one another (zstd -dc day/*.pcap.zst | ...)\n";

std::uint32_t magic_of(const unsigned char* bytes) {
    std::uint32_t magic = 0;
    std::memcpy(&magic, bytes, 4);
    return magic;
}

// A pcap's own header: the classic format, in this machine's order, stamped in micro- or nanoseconds.
bool is_capture(std::uint32_t magic) { return magic == 0xa1b2c3d4u || magic == 0xa1b23c4du; }

// Every Udp payload of one capture stream, to the port where one is named. False where it is no pcap of Ethernet.
template <typename Payload>
bool read(std::FILE* in, int port, Payload payload, std::uint64_t& frames) {
    unsigned char header[24];
    if (std::fread(header, 1, 24, in) != 24 || !is_capture(magic_of(header))) return false;
    std::uint32_t link = 0;
    std::memcpy(&link, header + 20, 4);
    if (link != 1) return false;   // Ethernet alone
    std::vector<unsigned char> frame(1 << 16);
    unsigned char record[16];
    while (std::fread(record, 1, 16, in) == 16) {
        if (is_capture(magic_of(record))) {   // the next capture of several, one after another: its header's rest
            if (std::fread(header + 16, 1, 8, in) != 8) break;
            continue;
        }
        std::uint32_t length = 0;
        std::memcpy(&length, record + 8, 4);
        if (length > frame.size()) frame.resize(length);
        if (std::fread(frame.data(), 1, length, in) != length) break;
        ++frames;
        std::size_t at = 12;
        while (at + 4 <= length && ((frame[at] == 0x81 && frame[at + 1] == 0x00) || (frame[at] == 0x88 && frame[at + 1] == 0xa8))) at += 4;   // VLAN tags
        if (at + 2 > length || frame[at] != 0x08 || frame[at + 1] != 0x00) continue;   // IPv4 alone
        at += 2;
        if (at + 20 > length || frame[at + 9] != 17) continue;   // Udp alone
        at += static_cast<std::size_t>(frame[at] & 15) * 4;
        if (at + 8 > length) continue;
        const int to = (frame[at + 2] << 8) | frame[at + 3];
        const std::size_t udp = static_cast<std::size_t>((frame[at + 4] << 8) | frame[at + 5]);
        if (port != 0 && to != port) continue;
        if (udp < 8 || at + udp > length) continue;
        payload(reinterpret_cast<const std::byte*>(frame.data() + at + 8), udp - 8);
    }
    return true;
}

}

int main(int argc, char** argv) {
    int port = 0;
    std::vector<std::string> captures;
    for (int i = 1; i < argc; ++i) {
        const std::string_view given = argv[i];
        if (given == "--port" && i + 1 < argc) { port = std::stoi(argv[++i]); continue; }
        captures.emplace_back(given);
    }
    if (captures.empty()) {
        std::fprintf(stderr, usage, argv[0]);
        return 2;
    }

    auto check = std::make_unique<sim::state_check::PriorityCheck>();
    std::uint64_t frames = 0;
    for (const auto& capture : captures) {
        std::FILE* in = capture == "-" ? stdin : std::fopen(capture.c_str(), "rb");
        if (in == nullptr) {
            std::fprintf(stderr, "cannot read %s\n", capture.c_str());
            return 2;
        }
        const bool read_it = read(in, port, [&](const std::byte* data, std::size_t size) { check->packet(data, size); }, frames);
        if (in != stdin) std::fclose(in);
        if (!read_it) {
            std::fprintf(stderr, "%s: no pcap of Ethernet frames\n", capture.c_str());
            return 2;
        }
    }

    const auto& report = check->report();
    std::printf("capture     %llu frames, %llu packets of the feed (%llu the other feed's copies), %llu gaps, %llu sessions\n",
                (unsigned long long)frames, (unsigned long long)report.packets, (unsigned long long)report.repeated,
                (unsigned long long)report.gaps, (unsigned long long)report.sessions);
    std::printf("applied     %llu messages, %llu of them for an order the engine's book did not hold\n",
                (unsigned long long)report.applied, (unsigned long long)report.unknown);
    std::uint64_t executions = 0, first = 0;
    for (std::size_t kind = 0; kind < report.kinds.size(); ++kind) {
        const auto& tally = report.kinds[kind];
        executions += tally.executions;
        first += tally.first;
        std::printf("%-32.*s %llu executions: the engine's first %llu; at the best price behind others %llu (%.1f ahead on average); at a worse price %llu; of an order not held %llu\n",
                    static_cast<int>(sim::state_check::kinds[kind].size()), sim::state_check::kinds[kind].data(), (unsigned long long)tally.executions,
                    (unsigned long long)tally.first, (unsigned long long)tally.behind, tally.behind ? static_cast<double>(tally.ahead) / static_cast<double>(tally.behind) : 0.0,
                    (unsigned long long)tally.worse, (unsigned long long)tally.absent);
    }
    for (const auto& difference : check->differences()) {
        std::printf("   %.*s: security %llu, order %llu taken at %lld, where the engine's first is %llu, the best %lld, %zu ahead of it\n",
                    static_cast<int>(sim::state_check::kinds[difference.kind].size()), sim::state_check::kinds[difference.kind].data(),
                    (unsigned long long)difference.key, (unsigned long long)difference.taken, (long long)difference.price,
                    (unsigned long long)difference.first, (long long)difference.best, difference.ahead);
    }
    std::printf("%llu of %llu executions took the order the engine would have traded first\n", (unsigned long long)first, (unsigned long long)executions);
    return 0;
}
