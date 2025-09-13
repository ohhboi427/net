#pragma once

#include <net/bit.hpp>
#include <net/defines.hpp>
#include <net/utils/parse.hpp>

#include <array>
#include <expected>
#include <format>
#include <string_view>
#include <utility>

namespace net {
    struct IPv4Address {
        union {
            std::array<u8, 4U> octets = { 0U, 0U, 0U, 0U };
            u32 value_be;
        } address;

        u16 port_be = 0U;

        [[nodiscard]] static constexpr auto parse(
            std::string_view str
        ) noexcept -> std::expected<IPv4Address, ParseError>;
    };

    constexpr auto IPv4Address::parse(std::string_view str) noexcept -> std::expected<IPv4Address, ParseError> {
        IPv4Address v4{};

        for(u8 octet = 0U; octet < 4U; ++octet) {
            constexpr std::array DELIMITERS = { '.', ':' };

            const auto address = parse_uint<u8, ParseBase::Decimal>(str, DELIMITERS);
            if(!address) {
                return std::unexpected(address.error());
            }

            if(octet < 3U) {
                if(str.empty() || str.front() != '.') {
                    return std::unexpected(ParseError::InvalidCharacter);
                }

                str.remove_prefix(1U);
            }

            v4.address.octets[octet] = address.value();
        }

        if(str.empty()) {
            return v4;
        }

        if(str.front() != ':') {
            return std::unexpected(ParseError::InvalidCharacter);
        }

        str.remove_prefix(1U);

        const auto port = parse_uint<u16, ParseBase::Decimal>(str);
        if(!port) {
            return std::unexpected(port.error());
        }

        v4.port_be = host_to_net(port.value());

        return v4;
    }

    namespace literals::inline address {
        [[nodiscard]] consteval auto operator""_v4(const char* str, usize length) -> IPv4Address;

        consteval auto operator""_v4(const char* str, [[maybe_unused]] const usize length) -> IPv4Address {
            const auto address = IPv4Address::parse(str);
            if(!address) {
                throw;
            }

            return address.value();
        }
    }
}

template<typename CharT>
struct std::formatter<net::IPv4Address, CharT> {
    enum class Mode {
        Default,
        AddressOnly,
        PortOnly,
    } mode = Mode::Default;

    constexpr auto parse(auto& ctx) -> decltype(ctx.begin()) {
        auto it = ctx.begin();
        if(it == ctx.end()) {
            return it;
        }

        switch(*it) {
        case 'a':
            mode = Mode::AddressOnly;
            ++it;
            break;

        case 'p':
            mode = Mode::PortOnly;
            ++it;
            break;

        default:
            break;
        }

        if(it != ctx.end() && *it != '}') {
            throw;
        }

        return it;
    }

    auto format(const net::IPv4Address& addr, auto& ctx) const -> decltype(ctx.out()) {
        switch(mode) {
        case Mode::AddressOnly:
            return std::format_to(
                ctx.out(),
                "{}.{}.{}.{}",
                addr.address.octets[0U],
                addr.address.octets[1U],
                addr.address.octets[2U],
                addr.address.octets[3U]
            );

        case Mode::PortOnly:
            return std::format_to(
                ctx.out(),
                "{}",
                net::net_to_host(addr.port_be)
            );

        case Mode::Default:
            return std::format_to(ctx.out(), "{0:a}:{0:p}", addr);
        }

        std::unreachable();
    }
};
