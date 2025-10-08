#pragma once

#include <net/defines.hpp>
#include <net/socket.hpp>
#include <net/tcp/stream.hpp>

#include <expected>

namespace net {
    class TcpListener {
    public:
        TcpListener(TcpListener&&) noexcept = default;

        [[nodiscard]] static auto bind() -> std::expected<TcpListener, SocketError>;

        [[nodiscard]] auto accept() const noexcept -> std::expected<TcpStream, SocketError>;

    private:
        Socket m_socket;

        explicit TcpListener(Socket&& socket) noexcept;
    };
}
