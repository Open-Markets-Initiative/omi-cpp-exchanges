#pragma once

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
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

    // Accept any pending connections, then read from each client and dispatch.
    template <typename Handler>
    void poll(Handler handler) {
        if (fd_ < 0) return;

        for (int client; (client = ::accept(fd_, nullptr, nullptr)) >= 0; ) {
            set_nonblocking(client);
            clients_.push_back(client);
        }

        char buffer[4096];
        for (std::size_t i = 0; i < clients_.size(); ) {
            int  client = clients_[i];
            auto n      = ::recv(client, buffer, sizeof buffer, 0);
            if (n > 0) {
                handler(client, std::string_view(buffer, static_cast<std::size_t>(n)));
                ++i;
            } else if (n == 0) {
                ::close(client);
                clients_.erase(clients_.begin() + static_cast<long>(i));   // client hung up
            } else {
                ++i;   // EWOULDBLOCK: nothing pending right now
            }
        }
    }

    static void send(int client, const char* data, std::size_t len) {
        ::send(client, data, len, MSG_NOSIGNAL);   // never SIGPIPE us if the client hung up
    }

private:
    static void set_nonblocking(int fd) {
        ::fcntl(fd, F_SETFL, ::fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
    }

    int              fd_ = -1;
    std::vector<int> clients_;
};

}