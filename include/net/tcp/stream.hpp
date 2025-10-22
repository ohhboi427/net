#pragma once

#include <net/addr.hpp>
#include <net/defines.hpp>
#include <net/socket.hpp>

#include <expected>
#include <span>

namespace net {
    class TcpStream {
        friend class TcpListener;

    public:
        TcpStream(TcpStream&&) noexcept = default;

        [[nodiscard]] static auto connect(const SocketAddr& addr) -> std::expected<TcpStream, SocketError>;

        [[nodiscard]] auto write(std::span<const byte> data) const noexcept -> std::expected<usize, SocketError>;
        [[nodiscard]] auto read(std::span<byte> data) const noexcept -> std::expected<usize, SocketError>;

    private:
        Socket m_socket;

        explicit TcpStream(Socket&& socket) noexcept;
    };
}
