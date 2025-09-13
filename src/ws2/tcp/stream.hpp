#pragma once
#if defined(_WIN32)

#include <net/address.hpp>
#include <net/defines.hpp>
#include <net/socket.hpp>
#include <net/tcp/stream.hpp>

#include <ws2/socket.hpp>

#include <expected>
#include <span>

namespace net {
    class WS2TcpStream {
    public:
        WS2TcpStream() = default;
        explicit WS2TcpStream(WS2Socket&& socket) noexcept;

        [[nodiscard]] static auto connect(IPv4Address address) noexcept -> std::expected<WS2TcpStream, SocketError>;

        [[nodiscard]] auto write(std::span<const u8> data) const noexcept -> std::expected<usize, SocketError>;

        [[nodiscard]] auto read(std::span<u8> buffer) const noexcept -> std::expected<usize, SocketError>;

    private:
        WS2Socket m_socket{};
    };
}

#endif
