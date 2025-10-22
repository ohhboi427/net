#include <net/tcp/listener.hpp>

#include <utility>

namespace net {
    TcpListener::TcpListener(Socket&& socket) noexcept
        : m_socket{ std::move(socket) } {}

    auto TcpListener::bind(const SocketAddr& addr) -> std::expected<TcpListener, SocketError> {
        const SocketConfig config{
            .address_family = addr.addr.address_family(),
            .protocol = Protocol::Tcp,
        };

        auto socket_result = Socket::create(config);
        if(!socket_result) {
            return std::unexpected{ socket_result.error() };
        }

        TcpListener stream{ std::move(socket_result).value() };

        if(const auto result = stream.m_socket.bind(addr); !result) {
            return std::unexpected{ result.error() };
        }

        return stream;
    }

    auto TcpListener::accept() const noexcept -> std::expected<std::tuple<TcpStream, SocketAddr>, SocketError> {
        auto socket_result = m_socket.accept();
        if(!socket_result) {
            return std::unexpected{ socket_result.error() };
        }

        auto [socket, addr] = std::move(socket_result).value();

        return std::tuple{ TcpStream{ std::move(socket) }, addr };
    }
}
