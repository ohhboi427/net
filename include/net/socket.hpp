#pragma once

#include <net/addr.hpp>
#include <net/defines.hpp>

#include <expected>
#include <format>
#include <span>
#include <tuple>
#include <utility>

namespace net {
    enum class Protocol {
        Tcp,
        Udp,
    };

    struct SocketConfig {
        AddressFamily address_family{};
        Protocol protocol{};
    };

    enum class SocketError {
        CreationFailed,
        BindingFailed,
        ConnectionFailed,
        ConnectionClosed,
        TimedOut,
    };

    class Socket {
    public:
        Socket(Socket&&) noexcept = default;

        [[nodiscard]] static auto create(SocketConfig config) -> std::expected<Socket, SocketError>;

        [[nodiscard]] auto connect(const SocketAddr& addr) const noexcept -> std::expected<void, SocketError>;
        [[nodiscard]] auto bind(const SocketAddr& addr) const noexcept -> std::expected<void, SocketError>;
        [[nodiscard]] auto accept() const noexcept -> std::expected<std::tuple<Socket, SocketAddr>, SocketError>;

        [[nodiscard]] auto write(std::span<const byte> data) const noexcept -> std::expected<usize, SocketError>;
        [[nodiscard]] auto read(std::span<byte> data) const noexcept -> std::expected<usize, SocketError>;

    private:
        Impl m_impl{ nullptr, nullptr };

        Socket() noexcept = default;
    };
}

template<>
struct std::formatter<net::SocketError> {
    static constexpr auto parse(std::format_parse_context& ctx) noexcept -> decltype(ctx.begin()) {
        return ctx.begin();
    }

    static auto format(const net::SocketError err, format_context& ctx) -> decltype(ctx.out()) {
        switch(err) {
        case net::SocketError::CreationFailed:
            return std::format_to(ctx.out(), "Socket creation failed");
        case net::SocketError::BindingFailed:
            return std::format_to(ctx.out(), "Socket binding failed");
        case net::SocketError::ConnectionFailed:
            return std::format_to(ctx.out(), "Socket connection failed");
        case net::SocketError::ConnectionClosed:
            return std::format_to(ctx.out(), "Socket connection closed");
        case net::SocketError::TimedOut:
            return std::format_to(ctx.out(), "Socket operation timed out");
        }

        std::unreachable();
    }
};
