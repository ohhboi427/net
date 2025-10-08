#pragma once

#include <net/defines.hpp>
#include <net/socket.hpp>
#include <net/tcp/stream.hpp>

#include <expected>
#include <tuple>

namespace net {
    class TcpListener {
    public:
        TcpListener(TcpListener&&) noexcept = default;

        [[nodiscard]] static auto bind(const SocketAddr& addr) -> std::expected<TcpListener, SocketError>;

        [[nodiscard]] auto accept() const noexcept -> std::expected<std::tuple<TcpStream, SocketAddr>, SocketError>;

    private:
        Socket m_socket;

        explicit TcpListener(Socket&& socket) noexcept;
    };
}
