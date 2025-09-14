#pragma once

#include <net/defines.hpp>

#include <bit>
#include <concepts>
#include <span>

namespace net {
    namespace detail {
        template<usize>
        struct SizedUint;

        template<>
        struct SizedUint<sizeof(u8)> {
            using Type = u8;
        };

        template<>
        struct SizedUint<sizeof(u16)> {
            using Type = u16;
        };

        template<>
        struct SizedUint<sizeof(u32)> {
            using Type = u32;
        };

        template<>
        struct SizedUint<sizeof(u64)> {
            using Type = u64;
        };
    }

    /**
     * An unsigned integral type with a specific size.
     *
     * @tparam Size The size of the unsigned integral type. Must be one of the size of the primitve types.
     */
    template<usize Size>
    using SizedUint = detail::SizedUint<Size>::Type;

    /**
     * Converts an integral value from the host's endianness to network byte order.
     *
     * @tparam T The type of the integral value.
     *
     * @param value The integral value.
     *
     * @return The value converted to network byte order.
     */
    template<std::integral T>
    [[nodiscard]] constexpr auto host_to_net(T value) noexcept -> T;

    template<std::integral T>
    constexpr auto host_to_net(const T value) noexcept -> T {
        if constexpr(std::endian::native == std::endian::little) {
            return std::byteswap(value);
        }

        return value;
    }

    /**
     * Converts an integral value from network byte order to the host's endianness.
     *
     * @tparam T The type of the integral value.
     *
     * @param value The integral value.
     *
     * @return The value converted to the host's endianness.
     */
    template<std::integral T>
    [[nodiscard]] constexpr auto net_to_host(T value) noexcept -> T;

    template<std::integral T>
    constexpr auto net_to_host(const T value) noexcept -> T {
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
        [[nodiscard]] constexpr auto operator""_b(const char* ptr, usize size) noexcept -> std::span<const u8>;

        [[nodiscard]] constexpr auto operator""_b(
            const char* const ptr,
            const usize size
        ) noexcept -> std::span<const u8> {
            return { reinterpret_cast<const u8*>(ptr), size + 1U /* Include the '\0' */ }; // NOLINT
        }
    }
}
