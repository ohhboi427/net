#pragma once

#include <net/address.hpp>
#include <net/socket.hpp>

#include <expected>
#include <memory>

namespace net {
    class TcpListener {
    public:
        [[nodiscard]] static auto bind(IPv4Address address) noexcept -> std::expected<TcpListener, SocketError>;

    private:
        std::unique_ptr<void, void(*)(void*)> m_impl{ nullptr, nullptr };
    };
}
