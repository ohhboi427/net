#pragma once

#include <net/bit.hpp>
#include <net/defines.hpp>
#include <net/socket.hpp>

#include <algorithm>
#include <array>
#include <bit>
#include <concepts>
#include <expected>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace net {
    template<typename>
    struct Serializer;

    template<typename T>
    concept Serializable = requires(const T object, const std::span<const u8> buffer) {
        { Serializer<T>::serialize(object) } -> std::convertible_to<std::span<const u8>>;
        { Serializer<T>::deserialize(buffer) } -> std::same_as<std::expected<T, SocketError>>;
    };

    template<typename T>
    concept SizedSerializable = Serializable<T> && requires {
        { Serializer<T>::SIZE } -> std::convertible_to<usize>;
    };

    template<typename T>
        requires (std::is_arithmetic_v<T> && (sizeof(T) <= sizeof(u64)))
    struct Serializer<T> {
        static constexpr usize SIZE = sizeof(T);

        [[nodiscard]] static auto serialize(T object) noexcept -> std::array<u8, SIZE>;
        [[nodiscard]] static auto deserialize(std::span<const u8> buffer) noexcept -> std::expected<T, SocketError>;
    };

    template<typename T>
        requires (std::is_arithmetic_v<T> && (sizeof(T) <= sizeof(u64)))
    auto Serializer<T>::serialize(T object) noexcept -> std::array<u8, SIZE> {
        const auto data = host_to_net(std::bit_cast<SizedUint<SIZE>>(object));

        return std::bit_cast<std::array<u8, SIZE>>(data);
    }

    template<typename T>
        requires (std::is_arithmetic_v<T> && (sizeof(T) <= sizeof(u64)))
    auto Serializer<T>::deserialize(const std::span<const u8> buffer) noexcept -> std::expected<T, SocketError> {
        if(buffer.size() != SIZE) {
            return std::unexpected(SocketError::InvalidMessage);
        }

        std::array<u8, SIZE> data{};
        std::ranges::copy(buffer, data.begin());

        const auto data_uint = net_to_host(std::bit_cast<SizedUint<SIZE>>(data));

        return std::bit_cast<T>(data_uint);
    }

    template<>
    struct Serializer<std::string> {
        [[nodiscard]] static auto serialize(std::string_view str) noexcept -> std::vector<u8>;
        [[nodiscard]] static auto deserialize(
            std::span<const u8> buffer
        ) noexcept -> std::expected<std::string, SocketError>;
    };
}
