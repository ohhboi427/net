#pragma once

#include <net/defines.hpp>

#include <array>
#include <expected>
#include <format>
#include <span>
#include <tuple>
#include <utility>

namespace net {
    enum class SocketProtocol {
        Tcp,
        Udp,
    };

    enum class SocketError {
        CreationFailed,
        BindingFailed,
        ConnectionFailed,
        ConnectionClosed,
        TimedOut,
    };

    struct SocketAddr {
        std::array<u8, 4U> address{};
        u16 port{};
    };

    class Socket {
    public:
        Socket(Socket&&) noexcept = default;

        [[nodiscard]] static auto create(SocketProtocol protocol) -> std::expected<Socket, SocketError>;

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

template<>
struct std::formatter<net::SocketAddr> {
    std::range_formatter<net::u8> fmt{};

    constexpr auto parse(format_parse_context& ctx) noexcept -> decltype(ctx.begin()) {
        const auto it = fmt.parse(ctx);

        fmt.set_separator(".");
        fmt.set_brackets({}, {});

        return it;
    }

    auto format(const net::SocketAddr& addr, format_context& ctx) const -> decltype(ctx.out()) {
        const auto out = fmt.format(addr.address, ctx);

        return std::format_to(out, ":{}", addr.port);
    }
};
