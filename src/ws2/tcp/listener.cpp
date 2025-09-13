#if defined(_WIN32)
#include <ws2/tcp/listener.hpp>

#include <bit>
#include <utility>

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

namespace net {
    WS2TcpListener::WS2TcpListener(WS2Socket&& socket) noexcept
        : m_socket{ std::move(socket) } {}

    auto WS2TcpListener::bind(const IPv4Address address) noexcept -> std::expected<WS2TcpListener, SocketError> {
        WS2TcpListener listener{};

        auto socket = WS2Socket::create(SocketType::Tcp);
        if(!socket) {
            return std::unexpected(socket.error());
        }

        listener.m_socket = std::move(socket).value();

        const sockaddr_in addr{
            .sin_family = AF_INET,
            .sin_port = address.port_be,
            .sin_addr = std::bit_cast<IN_ADDR>(address.address),
            .sin_zero = {},
        };

        if(const int status = ::bind(
            static_cast<SOCKET>(listener.m_socket),
            reinterpret_cast<const sockaddr*>(&addr),
            sizeof(addr)
        ); status != 0) {
            return std::unexpected(SocketError::BindingFailed);
        }

        if(const int status = ::listen(static_cast<SOCKET>(listener.m_socket), SOMAXCONN); status != 0) {
            return std::unexpected(SocketError::BindingFailed);
        }

        return listener;
    }

    auto WS2TcpListener::accept() const noexcept -> std::expected<WS2TcpStream, SocketError> {
        const SOCKET socket = ::accept(static_cast<SOCKET>(m_socket), nullptr, nullptr);
        if(socket == INVALID_SOCKET) {
            return std::unexpected(SocketError::ConnectionFailed);
        }

        return WS2TcpStream(WS2Socket(socket));
    }

    auto TcpListener::bind(const IPv4Address address) noexcept -> std::expected<TcpListener, SocketError> {
        TcpListener listener{};

        auto ws2_listener = WS2TcpListener::bind(address);
        if(!ws2_listener) {
            return std::unexpected(ws2_listener.error());
        }

        listener.m_impl = {
            new WS2TcpListener(std::move(ws2_listener).value()),
            [](void* const ptr) noexcept -> void {
                delete static_cast<WS2TcpListener*>(ptr);
            }
        };

        return listener;
    }

    auto TcpListener::accept() const noexcept -> std::expected<TcpStream, SocketError> {
        auto& listener = *static_cast<WS2TcpListener*>(m_impl.get());

        auto ws2_stream = listener.accept();
        if(!ws2_stream) {
            return std::unexpected(ws2_stream.error());
        }

        TcpStream stream{};
        stream.m_impl = {
            new WS2TcpStream(std::move(ws2_stream).value()),
            [](void* const ptr) noexcept -> void {
                delete static_cast<WS2TcpStream*>(ptr);
            }
        };

        return stream;
    }
}

#endif
