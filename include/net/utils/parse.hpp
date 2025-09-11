#pragma once

#include <net/defines.hpp>

#include <concepts>
#include <expected>
#include <format>
#include <initializer_list>
#include <limits>
#include <string_view>
#include <utility>

namespace net {
    enum class ParseError {
        InvalidCharacter,
        ValueOutOfBounds,
    };

    template<std::unsigned_integral T>
    constexpr auto parse_uint(
        std::string_view& str,
        std::initializer_list<const char> delimiters = {}
    ) noexcept -> std::expected<T, ParseError>;

    template<std::unsigned_integral T>
    constexpr auto parse_uint(
        std::string_view& str,
        const std::initializer_list<const char> delimiters
    ) noexcept -> std::expected<T, ParseError> {
        if(str.empty() || str.front() < '0' || str.front() > '9') {
            return std::unexpected(ParseError::InvalidCharacter);
        }

        usize value = 0U;
        while(!str.empty()) {
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
            if(value > std::numeric_limits<T>::max()) {
                return std::unexpected(ParseError::ValueOutOfBounds);
            }

            str.remove_prefix(1U);
        }

        return value;
    }
}

template<typename CharT>
struct std::formatter<net::ParseError, CharT> {
    static constexpr auto parse(auto& ctx) noexcept -> decltype(ctx.begin()) {
        return ctx.begin();
    }

    static auto format(const net::ParseError error, auto& ctx) -> decltype(ctx.out()) {
        switch(error) {
        case net::ParseError::InvalidCharacter:
            return std::format_to(ctx.out(), "The input contains a character that is not a digit or a delimiter!");
        case net::ParseError::ValueOutOfBounds:
            return std::format_to(ctx.out(), "The number is too large to fit in the desired type!");
        }

        std::unreachable();
    }
};
