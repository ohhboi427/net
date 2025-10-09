#pragma once

#include <net/socket.hpp>

#include <expected>

namespace net {
    class TcpStream {
        friend class TcpListener;

    public:
        TcpStream(TcpStream&&) noexcept = default;

        [[nodiscard]] static auto connect(const SocketAddr& addr) -> std::expected<TcpStream, SocketError>;

    private:
        Socket m_socket;

        explicit TcpStream(Socket&& socket) noexcept;
    };
}
