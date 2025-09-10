#pragma once

#include <net/defines.hpp>

#include <array>
#include <print>

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
                if(*str < '0' || *str > '9') {
                    throw;
                }

                u16 address = 0U;
                for(u8 i = 0U; i < 3U; ++i) {
                    const char c = *str;

                    if(c == '.' || c == ':' || c == '\0') {
                        break;
                    }

                    if(c < '0' || c > '9') {
                        throw;
                    }

                    address = address * 10U + (c - '0');

                    ++str;
                }

                if(octet < 3U) {
                    if(*str != '.') {
                        throw;
                    }

                    ++str;
                }

                if(octet == 3U && !(*str == ':' || *str == '\0')) {
                    throw;
                }

                if(address > 0xFFU) {
                    throw;
                }

                v4.address[octet] = address;
            }

            if(*str == '\0') {
                return v4;
            }

            if(*str++ != ':') {
                throw;
            }

            if(*str < '0' || *str > '9') {
                throw;
            }

            u32 port = 0U;
            for(u8 i = 0U; i < 5U; ++i) {
                const char c = *str;

                if(c == '\0') {
                    break;
                }

                if(c < '0' || c > '9') {
                    throw;
                }

                port = port * 10U + (c - '0');

                ++str;
            }

            if(*str != '\0') {
                throw;
            }

            if(port > 0xFFFFU) {
                throw;
            }

            v4.port = port;

            return v4;
        }
    }
}
