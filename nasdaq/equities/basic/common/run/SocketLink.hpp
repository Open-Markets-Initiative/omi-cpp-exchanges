#pragma once

#include "Components.hpp"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <deque>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>

namespace sim {

// A link between two components on different machines: the same framed messages the other two
// adapters carry, over Tcp.
//
// The reader listens and the writer dials it, because the reader is the one with an address worth
// publishing - an engine is somewhere a gateway sends orders to. Either end may come and go: the
// writer keeps trying to connect and buffers what it cannot send yet, and the reader goes back to
// waiting when its peer disappears.
//
// On the wire each message is [length u32][sequence u64][bytes]. The sequence is what lets a
// reader say that something is missing rather than carrying on as though the stream were whole.
template <std::size_t Slot>
class SocketLink final : public Link {
public:
    static constexpr std::size_t slot_bytes = Slot;

    enum class End { Writes, Reads };

    SocketLink(std::string name, End end, std::string host, std::uint16_t port)
        : name_(std::move(name)), end_(end), host_(std::move(host)), port_(port) {
        if (end_ == End::Reads) { listen_on(); }
    }

    SocketLink(const SocketLink&)            = delete;
    SocketLink& operator=(const SocketLink&) = delete;

    ~SocketLink() override {
        if (peer_ >= 0)     { ::close(peer_); }
        if (listener_ >= 0) { ::close(listener_); }
    }

    const std::string& name() const override { return name_; }

    // Writer side. Buffers rather than blocking: a component should go on doing its work while a
    // peer is away. False when the buffer is full, which is this adapter's backpressure.
    bool write(const char* data, std::size_t length) {
        if (length > Slot) { return false; }
        if (pending_bytes_ >= buffer_limit) { flush(); }
        if (pending_bytes_ >= buffer_limit) { return false; }

        std::string frame;
        frame.resize(4 + 8);

        const std::uint32_t size = static_cast<std::uint32_t>(8 + length);
        const std::uint64_t at   = ++written_;

        std::memcpy(frame.data(), &size, 4);
        std::memcpy(frame.data() + 4, &at, 8);
        frame.append(data, length);

        pending_bytes_ += frame.size();
        pending_.push_back(std::move(frame));

        flush();
        return true;
    }

    // Reader side: hands one whole message to `reader`. False when nothing has arrived.
    //
    // A sequence that is not the one expected means messages were lost - the peer restarted, or
    // the connection broke mid-stream. The reader is told through `gaps()` rather than the message
    // being dropped silently, because a venue that has lost orders should say so.
    //
    // The first message on a link is never a gap: a reader that joins a writer already running has
    // not lost anything, it simply was not there. After that every discontinuity counts, including
    // the one a restarted peer causes by numbering from the beginning again.
    template <typename Reader>
    bool read(Reader&& reader) {
        accept();
        receive();

        if (buffer_.size() < 4) { return false; }

        std::uint32_t size = 0;
        std::memcpy(&size, buffer_.data(), 4);

        if (size < 8 || size > Slot + 8)    { reset("a frame states an impossible length"); return false; }
        if (buffer_.size() < 4 + size)      { return false; }

        std::uint64_t at = 0;
        std::memcpy(&at, buffer_.data() + 4, 8);

        if (expected_ != 0 && at != expected_) { gaps_ += 1; }
        expected_ = at + 1;

        reader(std::string_view(buffer_.data() + 4 + 8, size - 8));
        buffer_.erase(0, 4 + size);
        ++read_;
        return true;
    }

    // Carries on sending, and keeps trying to reach a peer that is not there yet. Called once a
    // turn by whichever component writes to this link, because a writer with nothing new to write
    // would otherwise never touch its socket again - and then a message buffered before its peer
    // was up would wait for ever. That is not a theory: a venue whose components all start at once
    // lost a trader's login exactly so, and the trader sat waiting to be admitted.
    void deliver() override { flush(); }

    // Everything written has been handed to the kernel. It is not a promise that the far end has
    // it: on a socket there is no such promise to make, which is a difference from the other two
    // adapters worth knowing rather than hiding.
    bool drained() const override { return pending_.empty(); }

    std::uint64_t written() const { return written_; }
    std::uint64_t read()    const { return read_; }

    /// How many times the reader saw a sequence it was not expecting.
    std::uint64_t gaps()    const { return gaps_; }

    /// Is a peer connected right now?
    bool joined() const { return peer_ >= 0; }

private:
    // Bounded, so a component with an absent peer slows down rather than growing without end.
    static constexpr std::size_t buffer_limit = 64u * 1024u * 1024u;

    // How often a writer tries a peer that is not there.
    static constexpr auto retry_interval = std::chrono::milliseconds(100);

    void listen_on() {
        listener_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (listener_ < 0) { throw std::runtime_error("link '" + name_ + "': no socket"); }

        int yes = 1;
        ::setsockopt(listener_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

        // Every interface, always. The configured host is where the writer dials - on a container
        // network that is the reader's service name, which is not an address at all and cannot be
        // bound. The reader is the thing being dialled, so it listens wherever it can be reached.
        sockaddr_in where{};
        where.sin_family      = AF_INET;
        where.sin_port        = ::htons(port_);
        where.sin_addr.s_addr = INADDR_ANY;

        if (::bind(listener_, reinterpret_cast<sockaddr*>(&where), sizeof where) != 0)
            throw std::runtime_error("link '" + name_ + "': cannot bind port " + std::to_string(port_));

        if (::listen(listener_, 1) != 0)
            throw std::runtime_error("link '" + name_ + "': cannot listen on " + std::to_string(port_));

        ::fcntl(listener_, F_SETFL, ::fcntl(listener_, F_GETFL, 0) | O_NONBLOCK);
    }

    void accept() {
        if (peer_ >= 0 || listener_ < 0) { return; }

        const int joined = ::accept(listener_, nullptr, nullptr);
        if (joined < 0) { return; }

        peer_ = joined;
        ::fcntl(peer_, F_SETFL, ::fcntl(peer_, F_GETFL, 0) | O_NONBLOCK);
        int yes = 1;
        ::setsockopt(peer_, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof yes);
    }

    // Dials the reader, resolving its host by name rather than parsing it as an address: a
    // component's host is a machine, and on a container network it is reached by the name of the
    // service it runs. Resolved on every attempt, not once - a peer that is restarted somewhere
    // else comes back under the same name at a different address, and a name resolved once would
    // keep dialling the machine it used to be.
    void dial() {
        if (peer_ >= 0) { return; }

        // A peer that is not listening refuses at once, so dialling on every turn would be
        // thousands of connects a second to no purpose. Once in a while is enough to come up in
        // step with a venue whose components are all starting together.
        const auto now = std::chrono::steady_clock::now();
        if (now - dialled_ < retry_interval) { return; }
        dialled_ = now;

        addrinfo wanted{};
        wanted.ai_family   = AF_INET;
        wanted.ai_socktype = SOCK_STREAM;

        addrinfo* found = nullptr;
        if (::getaddrinfo(host_.c_str(), std::to_string(port_).c_str(), &wanted, &found) != 0 || found == nullptr) {
            return;
        }

        peer_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (peer_ < 0) { ::freeaddrinfo(found); return; }

        const int joined = ::connect(peer_, found->ai_addr, found->ai_addrlen);
        ::freeaddrinfo(found);

        if (joined != 0) {
            ::close(peer_);
            peer_ = -1;
            return;
        }

        ::fcntl(peer_, F_SETFL, ::fcntl(peer_, F_GETFL, 0) | O_NONBLOCK);
        int yes = 1;
        ::setsockopt(peer_, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof yes);
    }

    // Hands as much of the buffer to the kernel as it will take. A short write is normal on a
    // socket, so what is left stays at the front of the queue for next time.
    void flush() {
        dial();
        if (peer_ < 0) { return; }

        while (!pending_.empty()) {
            auto& frame = pending_.front();
            const auto sent = ::send(peer_, frame.data(), frame.size(), MSG_NOSIGNAL);

            if (sent < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) { return; }
                reset("the peer went away mid-write");
                return;
            }

            if (static_cast<std::size_t>(sent) < frame.size()) {
                frame.erase(0, static_cast<std::size_t>(sent));
                pending_bytes_ -= static_cast<std::size_t>(sent);
                return;
            }

            pending_bytes_ -= frame.size();
            pending_.pop_front();
        }
    }

    void receive() {
        if (peer_ < 0) { return; }

        char bytes[65536];
        for (;;) {
            const auto got = ::recv(peer_, bytes, sizeof bytes, 0);

            if (got > 0) { buffer_.append(bytes, static_cast<std::size_t>(got)); continue; }
            if (got == 0) { reset("the peer closed the connection"); return; }
            if (errno == EAGAIN || errno == EWOULDBLOCK) { return; }

            reset("the connection failed");
            return;
        }
    }

    // The peer is gone. The socket goes and what was half-read goes with it.
    //
    // What is deliberately kept is the sequence expected next. A reader that forgot it would
    // accept whatever arrived after a reconnection as though the stream had been whole - and since
    // Tcp does not deliver messages out of order or lose them mid-stream, forgetting it is the
    // same as never reporting a gap at all. Keeping it is what makes the next message say what
    // happened: a lower sequence is a peer that restarted and lost whatever it had not yet sent,
    // a higher one is messages that went missing between the two of them.
    void reset(const char* why) {
        (void)why;
        if (peer_ >= 0) { ::close(peer_); peer_ = -1; }
        buffer_.clear();
    }

    std::string              name_;
    End                      end_;
    std::string              host_;
    std::uint16_t            port_;
    int                      listener_ = -1;
    int                      peer_     = -1;
    std::string              buffer_;
    std::deque<std::string>  pending_;
    std::size_t              pending_bytes_ = 0;
    std::uint64_t            written_  = 0;
    std::uint64_t            read_     = 0;
    std::uint64_t            expected_ = 0;
    std::uint64_t            gaps_     = 0;
    std::chrono::steady_clock::time_point dialled_{};
};

// The same two link kinds the other adapters have, between machines instead.
using IpcSocket     = SocketLink<ipc_slot_bytes>;
using MessageSocket = SocketLink<512>;

}