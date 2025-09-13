#pragma once
#if defined(_WIN32)

#include <net/address.hpp>
#include <net/socket.hpp>
#include <net/tcp/listener.hpp>

#include <ws2/socket.hpp>
#include <ws2/tcp/stream.hpp>

#include <expected>

namespace net {
    class WS2TcpListener {
    public:
        WS2TcpListener() = default;
        explicit WS2TcpListener(WS2Socket&& socket) noexcept;

        [[nodiscard]] static auto bind(IPv4Address address) noexcept -> std::expected<WS2TcpListener, SocketError>;

        [[nodiscard]] auto accept() const noexcept -> std::expected<WS2TcpStream, SocketError>;

    private:
        WS2Socket m_socket{};
    };
}

#endif
