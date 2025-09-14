#pragma once

#include <net/address.hpp>
#include <net/bit.hpp>
#include <net/defines.hpp>
#include <net/serialize.hpp>
#include <net/socket.hpp>

#include <array>
#include <bit>
#include <expected>
#include <memory>
#include <span>
#include <vector>

namespace net {
    class TcpStream {
        friend class TcpListener;

    public:
        [[nodiscard]] static auto connect(IPv4Address address) noexcept -> std::expected<TcpStream, SocketError>;

        [[nodiscard]] auto write(std::span<const u8> data) const noexcept -> std::expected<usize, SocketError>;
        [[nodiscard]] auto write_all(std::span<const u8> data) const noexcept -> std::expected<void, SocketError>;

        template<Serializable T>
        [[nodiscard]] auto write_object(const T& object) const noexcept -> std::expected<void, SocketError>;

        [[nodiscard]] auto read(std::span<u8> buffer) const noexcept -> std::expected<usize, SocketError>;
        [[nodiscard]] auto read_exact(std::span<u8> buffer) const noexcept -> std::expected<void, SocketError>;

        template<Serializable T>
        [[nodiscard]] auto read_object() const noexcept -> std::expected<T, SocketError>;

    private:
        std::unique_ptr<void, void(*)(void*)> m_impl{ nullptr, nullptr };
    };

    template<Serializable T>
    auto TcpStream::write_object(const T& object) const noexcept -> std::expected<void, SocketError> {
        using Ser = Serializer<T>;

        if constexpr(Ser::SIZE != 0U) { // Fixed size.
            const auto data = Ser::serialize(object);

            if(const auto result = write_all(data); !result) {
                return std::unexpected(result.error());
            }

            return {};
        }

        const auto data = Ser::serialize(object);
        const std::span<const u8> object_data = data;

        const usize size = host_to_net(object_data.size());
        const auto size_data = std::bit_cast<std::array<u8, sizeof(size)>>(size);

        if(const auto result = write_all(size_data); !result) {
            return std::unexpected(result.error());
        }

        if(const auto result = write_all(object_data); !result) {
            return std::unexpected(result.error());
        }

        return {};
    }

    template<Serializable T>
    auto TcpStream::read_object() const noexcept -> std::expected<T, SocketError> {
        using Ser = Serializer<T>;

        if constexpr(Ser::SIZE != 0U) { // Fixed size
            std::array<u8, Ser::SIZE> buffer{};

            if(const auto result = read_exact(buffer); !result) {
                return std::unexpected(result.error());
            }

            return Ser::deserialize(buffer);
        }

        std::array<u8, sizeof(usize)> size_buffer{};
        if(const auto result = read_exact(size_buffer); !result) {
            return std::unexpected(result.error());
        }

        const usize size = net_to_host(std::bit_cast<usize>(size_buffer));

        std::vector<u8> object_buffer(size);
        if(const auto result = read_exact(object_buffer); !result) {
            return std::unexpected(result.error());
        }

        return Ser::deserialize(object_buffer);
    }
}
