#pragma once
#if defined(_WIN32)

#include <net/socket.hpp>

#include <expected>

#include <WinSock2.h>

namespace net {
    class WS2Socket {
    public:
        WS2Socket(WS2Socket&& other) noexcept;
        ~WS2Socket() noexcept;

        [[nodiscard]] static auto create(SocketProtocol protocol) -> std::expected<WS2Socket, SocketError>;

    private:
        SOCKET m_socket = INVALID_SOCKET;

        WS2Socket() noexcept = default;
    };
}

#endif
