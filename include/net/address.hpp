#pragma once

#include <net/defines.hpp>
#include <net/utils/parse.hpp>

#include <array>
#include <expected>
#include <format>
#include <string_view>

namespace net {
    struct IPv4Address {
        std::array<u8, 4U> address = { 0U, 0U, 0U, 0U };
        u16 port = 0U;

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

            v4.address[octet] = address.value();
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

        v4.port = port.value();

        return v4;
    }

    namespace literals {
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
    static constexpr auto parse(auto& ctx) noexcept -> decltype(ctx.begin()) {
        return ctx.begin();
    }

    static auto format(const net::IPv4Address& addr, auto& ctx) -> decltype(ctx.out()) {
        return std::format_to(
            ctx.out(),
            "{}.{}.{}.{}:{}",
            addr.address[0U],
            addr.address[1U],
            addr.address[2U],
            addr.address[3U],
            addr.port
        );
    }
};
