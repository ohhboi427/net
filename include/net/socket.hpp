#pragma once

#include <net/defines.hpp>

#include <array>
#include <expected>
#include <format>
#include <span>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

namespace net {
    enum class SocketAddressFamily {
        Ipv4,
        Ipv6,
    };

    enum class SocketProtocol {
        Tcp,
        Udp,
    };

    using Ipv4Addr = std::array<u8, 4U>;
    using Ipv6Addr = std::array<u16, 8U>;

    using IpAddr = std::variant<Ipv4Addr, Ipv6Addr>;

    struct SocketAddr {
        IpAddr addr{};
        u16 port{};

        [[nodiscard]] auto address_family() const noexcept -> SocketAddressFamily;
    };

    struct SocketConfig {
        SocketAddressFamily address_family{};
        SocketProtocol protocol{};
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

template<>
struct std::formatter<net::IpAddr> {
    std::range_formatter<net::u8> fmt_ipv4{};
    std::range_formatter<net::u16> fmt_ipv6{};

    constexpr auto parse(format_parse_context& ctx) noexcept -> decltype(ctx.begin()) {
        fmt_ipv4.set_separator(".");
        fmt_ipv4.set_brackets({}, {});

        format_parse_context ipv6_ctx(":x");
        fmt_ipv6.parse(ipv6_ctx);
        fmt_ipv6.set_separator(":");

        return ctx.begin();
    }

    auto format(const net::IpAddr& addr, format_context& ctx) const -> decltype(ctx.out()) {
        return std::visit(
            [&]<typename T>(const T& value) -> decltype(ctx.out()) {
                if constexpr(std::is_same_v<T, net::Ipv4Addr>) {
                    return fmt_ipv4.format(value, ctx);
                } else {
                    return fmt_ipv6.format(value, ctx);
                }
            },
            addr
        );
    }
};

template<>
struct std::formatter<net::SocketAddr> {
    static constexpr auto parse(format_parse_context& ctx) noexcept -> decltype(ctx.begin()) {
        return ctx.begin();
    }

    static auto format(const net::SocketAddr& addr, format_context& ctx) -> decltype(ctx.out()) {
        return std::format_to(ctx.out(), "{}:{}", addr.addr, addr.port);
    }
};
