#pragma once

#include <net/address.hpp>
#include <net/socket.hpp>

#include <expected>
#include <memory>

namespace net {
    class TcpStream {
    public:
        [[nodiscard]] static auto connect(IPv4Address address) noexcept -> std::expected<TcpStream, SocketError>;

    private:
        std::unique_ptr<void, void(*)(void*)> m_impl{ nullptr, nullptr };
    };
}
