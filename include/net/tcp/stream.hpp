#pragma once

#include <net/defines.hpp>
#include <net/socket.hpp>

#include <expected>

namespace net {
    class TcpStream {
    public:
        TcpStream(TcpStream&&) noexcept = default;

        [[nodiscard]] static auto create() -> std::expected<TcpStream, SocketError>;

    private:
        Socket m_socket;

        explicit TcpStream(Socket&& socket) noexcept;
    };
}
