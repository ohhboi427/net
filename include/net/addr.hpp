#pragma once

#include <net/defines.hpp>

#include <array>
#include <format>
#include <variant>

namespace net {
    enum class AddressFamily {
        Ipv4,
        Ipv6,
    };

    struct Ipv4Addr : std::array<u8, 4U> {
        [[nodiscard]] static constexpr auto unspecified() noexcept -> Ipv4Addr {
            return {};
        }

        [[nodiscard]] static constexpr auto loopback() noexcept -> Ipv4Addr {
            return { 127U, 0U, 0U, 1U };
        }
    };

    struct Ipv6Addr : std::array<u16, 8U> {
        [[nodiscard]] static constexpr auto unspecified() noexcept -> Ipv6Addr {
            return {};
        }

        [[nodiscard]] static constexpr auto loopback() noexcept -> Ipv6Addr {
            return { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 1U };
        }
    };

    struct IpAddr : std::variant<Ipv4Addr, Ipv6Addr> {
        using variant::variant;

        [[nodiscard]] auto address_family() const noexcept -> AddressFamily;
    };

    struct SocketAddr {
        IpAddr addr{};
        u16 port{};
    };
}

template<>
struct std::formatter<net::IpAddr> {
    std::range_formatter<net::u8> fmt_ipv4{};
    std::range_formatter<net::u16> fmt_ipv6{};

    constexpr auto parse(format_parse_context& ctx) noexcept -> decltype(ctx.begin()) {
        fmt_ipv4.set_separator(".");
        fmt_ipv4.set_brackets({}, {});

        format_parse_context ipv6_ctx(":04x");
        fmt_ipv6.parse(ipv6_ctx);
        fmt_ipv6.set_separator(":");

        return ctx.begin();
    }

    auto format(const net::IpAddr& addr, format_context& ctx) const -> decltype(ctx.out()) {
        const net::Visitor visitor{
            [&](const net::Ipv4Addr& ipv4) noexcept -> decltype(ctx.out()) {
                return fmt_ipv4.format(ipv4, ctx);
            },

            [&](const net::Ipv6Addr& ipv6) noexcept -> decltype(ctx.out()) {
                return fmt_ipv6.format(ipv6, ctx);
            },
        };

        return std::visit(visitor, addr);
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
