#pragma once

#include <net/defines.hpp>

#include <bit>
#include <concepts>
#include <span>

namespace net {
    /**
     * Converts an integral value from the host's endianness to network byte order.
     *
     * @param value The integral value.
     *
     * @return The value converted to network byte order.
     */
    [[nodiscard]] constexpr auto host_to_net(const std::integral auto value) noexcept -> decltype(value) {
        if constexpr(std::endian::native == std::endian::little) {
            return std::byteswap(value);
        }

        return value;
    }

    /**
     * Converts an integral value from network byte order to the host's endianness.
     *
     * @param value The integral value.
     *
     * @return The value converted to the host's endianness.
     */
    [[nodiscard]] constexpr auto net_to_host(const std::integral auto value) noexcept -> decltype(value) {
        if constexpr(std::endian::native == std::endian::little) {
            return std::byteswap(value);
        }

        return value;
    }

    namespace literals::inline bit {
        /**
         * Converts a string literal to a span of bytes. The length of the span includes the null terminator.
         *
         * @param ptr The pointer to the string literal.
         * @param size The length of the string literal excluding the null terminator.
         *
         * @return A span over the string literal.
         */
        [[nodiscard]] constexpr auto operator""_b(const char* ptr, const usize size) noexcept -> std::span<const u8> {
            return { reinterpret_cast<const u8*>(ptr), size + 1U /* Include the '\0' */ }; // NOLINT
        }
    }
}
