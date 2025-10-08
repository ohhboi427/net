#include <net/tcp/stream.hpp>

#include <utility>

namespace net {
    TcpStream::TcpStream(Socket&& socket) noexcept
        : m_socket{ std::move(socket) } {}

    auto TcpStream::create() -> std::expected<TcpStream, SocketError> {
        auto socket_result = Socket::create(SocketProtocol::Tcp);
        if(!socket_result) {
            return std::unexpected(socket_result.error());
        }

        TcpStream stream(std::move(socket_result).value());

        return stream;
    }
}
