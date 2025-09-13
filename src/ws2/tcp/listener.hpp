#pragma once
#if defined(_WIN32)

#include <net/address.hpp>
#include <net/socket.hpp>
#include <net/tcp/listener.hpp>

#include <ws2/socket.hpp>

#include <expected>

namespace net {
    class WS2TcpListener {
    public:
        [[nodiscard]] static auto bind(IPv4Address address) noexcept -> std::expected<WS2TcpListener, SocketError>;

    private:
        WS2Socket m_socket{};
    };
}

#endif
