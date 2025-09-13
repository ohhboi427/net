#if defined(_WIN32)
#include <ws2/listener.hpp>

#include <bit>
#include <utility>

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

namespace net {
    auto WS2Listener::bind(const IPv4Address address) noexcept -> std::expected<WS2Listener, SocketError> {
        WS2Listener listener{};

        auto socket = WS2Socket::create();
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

    auto Listener::bind(const IPv4Address address) noexcept -> std::expected<Listener, SocketError> {
        Listener listener{};

        auto ws2_listener = WS2Listener::bind(address);
        if(!ws2_listener) {
            return std::unexpected(ws2_listener.error());
        }

        listener.m_impl = {
            new WS2Listener(std::move(ws2_listener).value()),
            [](void* const ptr) noexcept -> void {
                delete static_cast<WS2Listener*>(ptr);
            }
        };

        return listener;
    }
}

#endif
