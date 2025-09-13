#pragma once

#include <net/address.hpp>
#include <net/defines.hpp>
#include <net/socket.hpp>

#include <expected>
#include <memory>
#include <span>

namespace net {
    class TcpStream {
        friend class TcpListener;

    public:
        [[nodiscard]] static auto connect(IPv4Address address) noexcept -> std::expected<TcpStream, SocketError>;

        [[nodiscard]] auto write(std::span<const u8> data) const noexcept -> std::expected<usize, SocketError>;

        [[nodiscard]] auto read(std::span<u8> buffer) const noexcept -> std::expected<usize, SocketError>;

    private:
        std::unique_ptr<void, void(*)(void*)> m_impl{ nullptr, nullptr };
    };
}
