#pragma once

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace net {

// UDP multicast publisher — sendto() one datagram per published segment. Satisfies the
// FeedServer Publisher concept (callable(const char*, std::size_t)).
class UdpMulticastPublisher {
public:
    UdpMulticastPublisher(const std::string& group, std::uint16_t port) {
        fd_ = ::socket(AF_INET, SOCK_DGRAM, 0);
        unsigned char ttl = 8;
        ::setsockopt(fd_, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof ttl);
        std::memset(&addr_, 0, sizeof addr_);
        addr_.sin_family = AF_INET;
        addr_.sin_port   = htons(port);
        ::inet_pton(AF_INET, group.c_str(), &addr_.sin_addr);
    }
    ~UdpMulticastPublisher() { if (fd_ >= 0) ::close(fd_); }

    UdpMulticastPublisher(UdpMulticastPublisher&& o) noexcept : fd_(o.fd_), addr_(o.addr_) { o.fd_ = -1; }
    UdpMulticastPublisher(const UdpMulticastPublisher&)            = delete;
    UdpMulticastPublisher& operator=(const UdpMulticastPublisher&) = delete;

    void operator()(const char* data, std::size_t len) {
        if (fd_ >= 0) ::sendto(fd_, data, len, 0, reinterpret_cast<const sockaddr*>(&addr_), sizeof addr_);
    }

private:
    int         fd_ = -1;
    sockaddr_in addr_{};
};

// Non-blocking TCP server — accepts connections and buffers per-connection receive data.
// poll() drains readable sockets and invokes a handler(int fd, std::string_view chunk)
// with whatever bytes arrived; the handler frames/parses. send() writes to one client.
class TcpServer {
public:
    TcpServer(const std::string& host, std::uint16_t port) {
        fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
        int one = 1;
        ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one);
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port   = htons(port);
        ::inet_pton(AF_INET, host.c_str(), &addr.sin_addr);
        if (::bind(fd_, reinterpret_cast<sockaddr*>(&addr), sizeof addr) < 0) { ::close(fd_); fd_ = -1; return; }
        ::listen(fd_, 16);
        set_nonblocking(fd_);
    }
    ~TcpServer() { if (fd_ >= 0) ::close(fd_); for (int c : clients_) ::close(c); }

    TcpServer(const TcpServer&)            = delete;
    TcpServer& operator=(const TcpServer&) = delete;

    bool listening() const { return fd_ >= 0; }

    // Accept any pending connections, then read from each client and dispatch. Returns
    // whether anything happened, so a component thread knows when it may back off.
    //
    // `gone` is told about a client that hung up. It has to be: a descriptor is a number the
    // kernel hands out again, so whoever keeps state per connection - a session, a parse buffer -
    // must forget it before that number comes round. A session inherited by the next client
    // reads its first bytes as the end of whatever the last one left half-said.
    template <typename Handler, typename Gone>
    bool poll(Handler handler, Gone gone) {
        if (fd_ < 0) return false;

        bool worked = flush();
        for (const int client : std::set<int>(slow_)) {   // too slow to keep up: let go, as a venue lets a slow consumer go
            for (std::size_t i = 0; i < clients_.size(); ++i) {
                if (clients_[i] != client) continue;
                ::close(client);
                clients_.erase(clients_.begin() + static_cast<long>(i));
                break;
            }
            pending_.erase(client);
            slow_.erase(client);
            gone(client);
            worked = true;
        }
        for (int client; (client = ::accept(fd_, nullptr, nullptr)) >= 0; ) {
            set_nonblocking(client);
            clients_.push_back(client);
            worked = true;
        }

        char buffer[4096];
        for (std::size_t i = 0; i < clients_.size(); ) {
            int  client = clients_[i];
            auto n      = ::recv(client, buffer, sizeof buffer, 0);
            if (n > 0) {
                handler(client, std::string_view(buffer, static_cast<std::size_t>(n)));
                worked = true;
                ++i;
            } else if (n == 0) {
                ::close(client);
                clients_.erase(clients_.begin() + static_cast<long>(i));   // client hung up
                pending_.erase(client);
                slow_.erase(client);
                gone(client);
                worked = true;
            } else {
                ++i;   // EWOULDBLOCK: nothing pending right now
            }
        }
        return worked;
    }

    // Close one client from this side: its session has ended - it went silent past what the
    // session allows. The caller forgets the session itself, as `gone` would have it do.
    void close(int client) {
        for (std::size_t i = 0; i < clients_.size(); ++i) {
            if (clients_[i] != client) continue;
            ::close(client);
            clients_.erase(clients_.begin() + static_cast<long>(i));
            pending_.erase(client);
            slow_.erase(client);
            return;
        }
    }

    // For a caller that keeps nothing per connection.
    template <typename Handler>
    bool poll(Handler handler) { return poll(handler, [](int) {}); }

    // Send to a client: what its socket takes now, and the rest - a frame cut short included - kept, in
    // order, for the polls that follow to send as the socket takes it. Nothing is dropped. A client
    // whose kept bytes pass `most` is too slow to keep up, and the next poll lets it go.
    void send(int client, const char* data, std::size_t len) {
        if (const auto kept = pending_.find(client); kept != pending_.end()) {
            kept->second.bytes.append(data, len);
            if (kept->second.bytes.size() - kept->second.at > most) slow_.insert(client);
            return;
        }
        const auto sent = ::send(client, data, len, MSG_NOSIGNAL | MSG_DONTWAIT);   // never SIGPIPE us if the client hung up
        if (sent == static_cast<ssize_t>(len)) return;
        if (sent < 0 && errno != EAGAIN && errno != EWOULDBLOCK) return;   // gone: the next poll sees it hang up
        const auto took = sent > 0 ? static_cast<std::size_t>(sent) : 0;
        pending_[client].bytes.assign(data + took, len - took);
    }

    // Has a client more kept for it than a producer should add to? One that sends a stream - TIP's
    // basic data, a replay - waits for it to drain rather than piling more on.
    bool backed_up(int client) const {
        const auto kept = pending_.find(client);
        return kept != pending_.end() && kept->second.bytes.size() - kept->second.at >= backlog;
    }

    // What a client's socket would not take yet, and how much of it has gone since.
    struct Pending { std::string bytes; std::size_t at = 0; };

    static constexpr std::size_t backlog = std::size_t{ 1 } << 20;   // 1 MiB: a producer waits
    static constexpr std::size_t most    = std::size_t{ 64 } << 20;  // 64 MiB: the client is let go

private:
    // Send what was kept for each client, as far as its socket takes it.
    bool flush() {
        bool worked = false;
        for (auto kept = pending_.begin(); kept != pending_.end(); ) {
            auto& waiting = kept->second;
            const auto sent = ::send(kept->first, waiting.bytes.data() + waiting.at, waiting.bytes.size() - waiting.at, MSG_NOSIGNAL | MSG_DONTWAIT);
            if (sent > 0) {
                waiting.at += static_cast<std::size_t>(sent);
                worked = true;
            } else if (sent < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
                waiting.at = waiting.bytes.size();   // gone: the poll sees it hang up
            }
            if (waiting.at == waiting.bytes.size()) {
                kept = pending_.erase(kept);
            } else {
                if (waiting.at > (std::size_t{ 1 } << 16) && waiting.at * 2 > waiting.bytes.size()) {
                    waiting.bytes.erase(0, waiting.at);   // what has gone, given back now and then rather than each time
                    waiting.at = 0;
                }
                ++kept;
            }
        }
        return worked;
    }

    std::map<int, Pending> pending_;   // what each client's socket has not taken yet
    std::set<int>          slow_;      // clients past `most`, let go at the next poll

    static void set_nonblocking(int fd) {
        ::fcntl(fd, F_SETFL, ::fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
    }

    int              fd_ = -1;
    std::vector<int> clients_;
};

}