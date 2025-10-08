#include <net/tcp/listener.hpp>

#include <utility>

namespace net {
    TcpListener::TcpListener(Socket&& socket) noexcept
        : m_socket{ std::move(socket) } {}

    auto TcpListener::create() -> std::expected<TcpListener, SocketError> {
        auto socket_result = Socket::create(SocketProtocol::Tcp);
        if(!socket_result) {
            return std::unexpected(socket_result.error());
        }

        TcpListener stream(std::move(socket_result).value());

        return stream;
    }
}
