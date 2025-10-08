#pragma once

#include <net/defines.hpp>
#include <net/socket.hpp>

#include <expected>

namespace net {
    class TcpListener {
    public:
        TcpListener(TcpListener&&) noexcept = default;

        [[nodiscard]] static auto create() -> std::expected<TcpListener, SocketError>;

    private:
        Socket m_socket;

        explicit TcpListener(Socket&& socket) noexcept;
    };
}
