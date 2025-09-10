#pragma once

#include <net/address.hpp>

#include <concepts>
#include <expected>
#include <initializer_list>
#include <limits>
#include <string_view>

namespace net {
    enum class ParseError {
        InvalidCharacter,
        ValueOutOfBounds,
    };

    template<std::unsigned_integral T>
    constexpr auto parse_uint(
        std::string_view& str,
        usize max_length,
        std::initializer_list<char> delimiters = {}
    ) noexcept -> std::expected<T, ParseError>;

    template<std::unsigned_integral T>
    constexpr auto parse_uint(
        std::string_view& str,
        const usize max_length,
        const std::initializer_list<char> delimiters
    ) noexcept -> std::expected<T, ParseError> {
        if(str.empty() || str.front() < '0' || str.front() > '9') {
            return std::unexpected(ParseError::InvalidCharacter);
        }

        usize value = 0U;
        for(usize i = 0U; i < max_length; ++i) {
            if(str.empty()) {
                break;
            }

            const char c = str.front();

            bool is_delimiter = false;
            for(const auto d : delimiters) {
                if(c == d) {
                    is_delimiter = true;

                    break;
                }
            }

            if(is_delimiter) {
                break;
            }

            if(c < '0' || c > '9') {
                return std::unexpected(ParseError::InvalidCharacter);
            }

            value = value * 10U + (c - '0');

            str.remove_prefix(1U);
        }

        if(value > std::numeric_limits<T>::max()) {
            return std::unexpected(ParseError::ValueOutOfBounds);
        }

        return value;
    }
}
