#if defined(_WIN32)
#include <ws2/socket.hpp>

#include <ws2/context.hpp>

#include <utility>

#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

namespace net {
    auto WS2Socket::create() noexcept -> std::expected<WS2Socket, SocketError> {
        static auto context = WS2Context::create();
        if(!context) {
            return std::unexpected(SocketError::CreationFailed);
        }

        WS2Socket socket{};

        socket.m_handle = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(socket.m_handle == INVALID_SOCKET) {
            return std::unexpected(SocketError::CreationFailed);
        }

        return socket;
    }

    WS2Socket::WS2Socket(WS2Socket&& other) noexcept
        : m_handle{ other.m_handle } {
        other.m_handle = INVALID_SOCKET;
    }

    WS2Socket::~WS2Socket() noexcept {
        if(m_handle != INVALID_SOCKET) {
            closesocket(m_handle);
        }
    }

    auto WS2Socket::operator=(WS2Socket&& other) noexcept -> WS2Socket& {
        if(const auto old = std::exchange(m_handle, other.m_handle); old != INVALID_SOCKET) {
            closesocket(old);
        }

        other.m_handle = INVALID_SOCKET;

        return *this;
    }

    auto Socket::create() noexcept -> std::expected<Socket, SocketError> {
        Socket socket{};

        auto ws2_socket = WS2Socket::create();
        if(!ws2_socket) {
            return std::unexpected(ws2_socket.error());
        }

        socket.m_impl = {
            new WS2Socket(std::move(ws2_socket).value()),
            [](void* const ptr) noexcept -> void {
                delete static_cast<WS2Socket*>(ptr);
            }
        };

        return socket;
    }
}

#endif
