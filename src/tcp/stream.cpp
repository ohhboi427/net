#include <net/tcp/stream.hpp>

#include <utility>

namespace net {
    TcpStream::TcpStream(Socket&& socket) noexcept
        : m_socket{ std::move(socket) } {}

    auto TcpStream::connect(const SocketAddr& addr) -> std::expected<TcpStream, SocketError> {
        const SocketConfig config{
            .address_family = addr.address_family(),
            .protocol = SocketProtocol::Tcp,
        };

        auto socket_result = Socket::create(config);
        if(!socket_result) {
            return std::unexpected{ socket_result.error() };
        }

        TcpStream stream{ std::move(socket_result).value() };

        if(const auto result = stream.m_socket.connect(addr); !result) {
            return std::unexpected{ result.error() };
        }

        return stream;
    }

    auto TcpStream::write(const std::span<const byte> data) const noexcept -> std::expected<usize, SocketError> {
        return m_socket.write(data);
    }

    auto TcpStream::read(const std::span<byte> data) const noexcept -> std::expected<usize, SocketError> {
        return m_socket.read(data);
    }
}
