#pragma once

#include <net/defines.hpp>
#include <net/utils/parse.hpp>

#include <array>

namespace net {
    struct IPv4Address {
        std::array<u8, 4U> address = { 0U, 0U, 0U, 0U };
        u16 port = 0U;
    };

    namespace literals {
        constexpr auto operator""_v4(const char* str, usize length) -> IPv4Address;

        constexpr auto operator""_v4(const char* str, [[maybe_unused]] const usize length) -> IPv4Address {
            IPv4Address v4{};

            for(u8 octet = 0U; octet < 4U; ++octet) {
                const auto address_result = parse_uint(str, 3U, { '.', ':' });
                if(!address_result) {
                    throw;
                }

                const auto address = address_result.value();
                if(address > 0xFFU) {
                    throw;
                }

                if(octet < 3U) {
                    if(*str != '.') {
                        throw;
                    }

                    ++str;
                }

                v4.address[octet] = address;
            }

            if(*str == '\0') {
                return v4;
            }

            if(*str++ != ':') {
                throw;
            }

            const auto port_result = parse_uint(str, 5U);
            if(!port_result) {
                throw;
            }

            const auto port = port_result.value();
            if(port > 0xFFFFU) {
                throw;
            }

            v4.port = port;

            if(*str != '\0') {
                throw;
            }

            return v4;
        }
    }
}
