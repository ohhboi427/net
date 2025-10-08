#include <net/tcp/listener.hpp>

#include <utility>

namespace net {
    TcpListener::TcpListener(Socket&& socket) noexcept
        : m_socket{ std::move(socket) } {}

    auto TcpListener::bind() -> std::expected<TcpListener, SocketError> {
        auto socket_result = Socket::create(SocketProtocol::Tcp);
        if(!socket_result) {
            return std::unexpected(socket_result.error());
        }

        TcpListener stream(std::move(socket_result).value());

        return stream;
    }

    auto TcpListener::accept() const noexcept -> std::expected<TcpStream, SocketError> {
        auto socket_result = m_socket.accept();
        if(!socket_result) {
            return std::unexpected(socket_result.error());
        }

        return TcpStream(std::move(socket_result).value());
    }
}
