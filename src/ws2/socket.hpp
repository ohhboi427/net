#pragma once
#if defined(_WIN32)

#include <net/socket.hpp>

#include <expected>

#include <WinSock2.h>

namespace net {
    class WS2Socket {
    public:
        WS2Socket() = default;

        WS2Socket(WS2Socket&& other) noexcept;

        ~WS2Socket() noexcept;

        auto operator=(WS2Socket&& other) noexcept -> WS2Socket&;

        [[nodiscard]] static auto create(SocketType type) noexcept -> std::expected<WS2Socket, SocketError>;

        [[nodiscard]] explicit operator SOCKET() const noexcept {
            return m_handle;
        }

    private:
        SOCKET m_handle = INVALID_SOCKET;
    };
}

#endif
