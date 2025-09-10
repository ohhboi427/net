#pragma once

#include <net/address.hpp>

#include <concepts>
#include <expected>
#include <initializer_list>
#include <limits>

namespace net {
    enum class ParseError {
        InvalidCharacter,
        ValueOutOfBounds,
    };

    template<std::unsigned_integral T>
    constexpr auto parse_uint(
        const char*& str,
        usize max_length,
        std::initializer_list<char> delimiters = {}
    ) noexcept -> std::expected<T, ParseError>;

    template<std::unsigned_integral T>
    constexpr auto parse_uint(
        const char*& str,
        const usize max_length,
        const std::initializer_list<char> delimiters
    ) noexcept -> std::expected<T, ParseError> {
        if(*str < '0' || *str > '9') {
            return std::unexpected(ParseError::InvalidCharacter);
        }

        usize value = 0U;
        for(usize i = 0U; i < max_length; ++i) {
            const char c = *str;

            bool is_delimiter = false;
            for(const auto d : delimiters) {
                if(c == d) {
                    is_delimiter = true;
                }
            }

            if(is_delimiter || c == '\0') {
                break;
            }

            if(c < '0' || c > '9') {
                return std::unexpected(ParseError::InvalidCharacter);
            }

            value = value * 10U + (c - '0');

            ++str;
        }

        if(value > std::numeric_limits<T>::max()) {
            return std::unexpected(ParseError::ValueOutOfBounds);
        }

        return value;
    }
}
