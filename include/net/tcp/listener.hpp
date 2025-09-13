#pragma once

#include <net/address.hpp>
#include <net/socket.hpp>
#include <net/tcp/stream.hpp>

#include <expected>
#include <memory>

namespace net {
    class TcpListener {
    public:
        [[nodiscard]] static auto bind(IPv4Address address) noexcept -> std::expected<TcpListener, SocketError>;

        [[nodiscard]] auto accept() const noexcept -> std::expected<TcpStream, SocketError>;

    private:
        std::unique_ptr<void, void(*)(void*)> m_impl{ nullptr, nullptr };
    };
}
