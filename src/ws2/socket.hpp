#pragma once
#if defined(_WIN32)

#include <net/defines.hpp>
#include <net/socket.hpp>

#include <expected>
#include <span>
#include <tuple>
#include <variant>

#include <WinSock2.h>
#include <WS2tcpip.h>

namespace net {
    struct WS2SocketAddr {
        std::variant<sockaddr_in, sockaddr_in6> addr;

        explicit WS2SocketAddr(const sockaddr_storage& addr_info) noexcept;
        explicit WS2SocketAddr(const SocketAddr& addr) noexcept;

        [[nodiscard]] explicit operator SocketAddr() const noexcept;

        [[nodiscard]] auto as_generic() const noexcept -> std::tuple<const sockaddr*, usize>;
    };

    class WS2Socket {
    public:
        WS2Socket(WS2Socket&& other) noexcept;
        ~WS2Socket() noexcept;

        [[nodiscard]] static auto create(SocketConfig config) -> std::expected<WS2Socket, SocketError>;

        [[nodiscard]] auto connect(const SocketAddr& addr) const noexcept -> std::expected<void, SocketError>;
        [[nodiscard]] auto bind(const SocketAddr& addr) const noexcept -> std::expected<void, SocketError>;
        [[nodiscard]] auto accept() const noexcept -> std::expected<std::tuple<WS2Socket, SocketAddr>, SocketError>;

        [[nodiscard]] auto write(std::span<const byte> data) const noexcept -> std::expected<usize, SocketError>;
        [[nodiscard]] auto read(std::span<byte> data) const noexcept -> std::expected<usize, SocketError>;

    private:
        SOCKET m_handle = INVALID_SOCKET;

        WS2Socket() noexcept = default;
    };
}

#endif
