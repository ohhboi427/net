#if defined(_WIN32)
#include <ws2/tcp/stream.hpp>

#include <bit>
#include <utility>

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

namespace net {
    WS2TcpStream::WS2TcpStream(WS2Socket&& socket) noexcept
        : m_socket{ std::move(socket) } {}

    auto WS2TcpStream::connect(const IPv4Address address) noexcept -> std::expected<WS2TcpStream, SocketError> {
        WS2TcpStream stream{};

        auto socket = WS2Socket::create(SocketType::Tcp);
        if(!socket) {
            return std::unexpected(socket.error());
        }

        stream.m_socket = std::move(socket).value();

        const sockaddr_in addr{
            .sin_family = AF_INET,
            .sin_port = address.port_be,
            .sin_addr = std::bit_cast<IN_ADDR>(address.address),
            .sin_zero = {},
        };

        if(const int status = ::connect(
            static_cast<SOCKET>(stream.m_socket),
            reinterpret_cast<const sockaddr*>(&addr),
            sizeof(addr)
        ); status != 0) {
            return std::unexpected(SocketError::ConnectionFailed);
        }

        return stream;
    }

    auto WS2TcpStream::write(const std::span<const u8> data) const noexcept -> std::expected<usize, SocketError> {
        const isize bytes_sent = send(
            static_cast<SOCKET>(m_socket),
            reinterpret_cast<const char*>(data.data()),
            static_cast<i32>(data.size()),
            0
        );

        if(bytes_sent == SOCKET_ERROR) {
            // TODO: Handle specific errors differently.

            return std::unexpected(SocketError::TimedOut);
        }

        return static_cast<usize>(bytes_sent);
    }

    auto WS2TcpStream::read(const std::span<u8> buffer) const noexcept -> std::expected<usize, SocketError> {
        const isize bytes_received = recv(
            static_cast<SOCKET>(m_socket),
            reinterpret_cast<char*>(buffer.data()),
            static_cast<i32>(buffer.size()),
            0
        );

        if(bytes_received == SOCKET_ERROR) {
            return std::unexpected(SocketError::TimedOut);
        }

        if(bytes_received == 0) {
            return std::unexpected(SocketError::ConnectionClosed);
        }

        return static_cast<usize>(bytes_received);
    }

    auto TcpStream::connect(const IPv4Address address) noexcept -> std::expected<TcpStream, SocketError> {
        TcpStream stream{};

        auto ws2_stream = WS2TcpStream::connect(address);
        if(!ws2_stream) {
            return std::unexpected(ws2_stream.error());
        }

        stream.m_impl = {
            new WS2TcpStream(std::move(ws2_stream).value()),
            [](void* const ptr) noexcept -> void {
                delete static_cast<WS2TcpStream*>(ptr);
            }
        };

        return stream;
    }

    auto TcpStream::write(const std::span<const u8> data) const noexcept -> std::expected<usize, SocketError> {
        const auto& stream = *static_cast<WS2TcpStream*>(m_impl.get());

        return stream.write(data);
    }

    auto TcpStream::read(const std::span<u8> buffer) const noexcept -> std::expected<usize, SocketError> {
        const auto& stream = *static_cast<WS2TcpStream*>(m_impl.get());

        return stream.read(buffer);
    }
}

#endif
