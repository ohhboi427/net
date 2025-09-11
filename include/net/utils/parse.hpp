#pragma once

#include <net/defines.hpp>

#include <algorithm>
#include <concepts>
#include <expected>
#include <format>
#include <limits>
#include <span>
#include <string_view>
#include <utility>

namespace net {
    enum class ParseError {
        InvalidCharacter,
        ValueOutOfBounds,
    };

    enum class ParseBase {
        Decimal,
        Octal,
        Hexadecimal,
    };

    namespace detail {
        template<ParseBase>
        struct ParseRules;

        template<>
        struct ParseRules<ParseBase::Decimal> {
            static constexpr usize VALUE = 10U;

            static constexpr auto is_char_valid(const char c) noexcept -> bool {
                if(c >= '0' && c <= '9') {
                    return true;
                }

                return false;
            }

            static constexpr auto char_value(const char c) noexcept -> usize {
                return c - '0';
            }
        };

        template<>
        struct ParseRules<ParseBase::Octal> {
            static constexpr usize VALUE = 8U;

            static constexpr auto is_char_valid(const char c) noexcept -> bool {
                if(c >= '0' && c <= '7') {
                    return true;
                }

                return false;
            }

            static constexpr auto char_value(const char c) noexcept -> usize {
                return c - '0';
            }
        };

        template<>
        struct ParseRules<ParseBase::Hexadecimal> {
            static constexpr usize VALUE = 16U;

            static constexpr auto is_char_valid(const char c) noexcept -> bool {
                if(c >= '0' && c <= '9') {
                    return true;
                }

                if((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) {
                    return true;
                }

                return false;
            }

            static constexpr auto char_value(const char c) noexcept -> usize {
                if(c >= '0' && c <= '9') {
                    return c - '0';
                }

                if(c >= 'A' && c <= 'F') {
                    return c - 'A' + 10U;
                }

                if(c >= 'a' && c <= 'f') {
                    return c - 'a' + 10U;
                }

                std::unreachable();
            }
        };
    }

    template<std::unsigned_integral T, ParseBase B>
    constexpr auto parse_uint(
        std::string_view& str,
        std::span<const char> delimiters = {}
    ) noexcept -> std::expected<T, ParseError>;

    template<std::unsigned_integral T, ParseBase B>
    constexpr auto parse_uint(
        std::string_view& str,
        const std::span<const char> delimiters
    ) noexcept -> std::expected<T, ParseError> {
        using Rules = detail::ParseRules<B>;

        if(str.empty()|| !Rules::is_char_valid(str.front())) {
            return std::unexpected(ParseError::InvalidCharacter);
        }

        usize value = 0U;
        while(!str.empty()) {
            const char c = str.front();

            if(std::ranges::contains(delimiters, c)) {
                break;
            }

            if(!Rules::is_char_valid(c)) {
                return std::unexpected(ParseError::InvalidCharacter);
            }

            value = value * Rules::VALUE + Rules::char_value(c);
            if(value > std::numeric_limits<T>::max()) {
                return std::unexpected(ParseError::ValueOutOfBounds);
            }

            str.remove_prefix(1U);
        }

        return value;
    }

    template<std::unsigned_integral T>
    constexpr auto parse_uint_auto(
        std::string_view& str,
        std::span<const char> delimiters = {}
    ) noexcept -> std::expected<T, ParseError>;

    template<std::unsigned_integral T>
    constexpr auto parse_uint_auto(
        std::string_view& str,
        const std::span<const char> delimiters
    ) noexcept -> std::expected<T, ParseError> {
        if(str.empty()) {
            return std::unexpected(ParseError::InvalidCharacter);
        }

        if(str.starts_with("0x") || str.starts_with("0X")) {
            str.remove_prefix(2U);

            return parse_uint<T, ParseBase::Hexadecimal>(str, delimiters);
        }

        if(str.starts_with('0')) {
            return parse_uint<T, ParseBase::Octal>(str, delimiters);
        }

        return parse_uint<T, ParseBase::Decimal>(str, delimiters);
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
