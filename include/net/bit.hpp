#pragma once

#include <net/defines.hpp>

#include <bit>
#include <concepts>
#include <span>

namespace net {
    template<std::integral T>
    [[nodiscard]] constexpr auto net_to_host(const T value) noexcept -> T {
        if constexpr(std::endian::native != std::endian::little) {
            return value;
        }

        return std::byteswap(value);
    }

    template<std::integral T>
    [[nodiscard]] constexpr auto host_to_net(const T value) noexcept -> T {
        if constexpr(std::endian::native != std::endian::little) {
            return value;
        }

        return std::byteswap(value);
    }

    inline namespace literals {
        [[nodiscard]] constexpr auto operator""_b(
            const char* const ptr,
            const usize size
        ) noexcept -> std::span<const u8> {
            return { reinterpret_cast<const u8*>(ptr), size }; // NOLINT
        }
    }
}
