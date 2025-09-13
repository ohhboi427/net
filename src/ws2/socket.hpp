#pragma once
#if defined(_WIN32)

#include <net/socket.hpp>

#include <WinSock2.h>

#include <expected>

namespace net {
    class WS2Socket {
    public:
        WS2Socket() = default;

        WS2Socket(WS2Socket&& other) noexcept;

        ~WS2Socket() noexcept;

        auto operator=(WS2Socket&& other) noexcept -> WS2Socket&;

        [[nodiscard]] static auto create() noexcept -> std::expected<WS2Socket, SocketError>;

    private:
        SOCKET m_handle = INVALID_SOCKET;
    };
}

#endif
