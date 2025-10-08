#pragma once

#include <net/defines.hpp>

#include <array>
#include <expected>
#include <tuple>

namespace net {
    enum class SocketProtocol {
        Tcp,
        Udp,
    };

    enum class SocketError {
        CreationFailed,
        ConnectionFailed,
        BindingFailed,
    };

    struct SocketAddr {
        std::array<u8, 4U> address{};
        u16 port{};
    };

    class Socket {
    public:
        Socket(Socket&&) noexcept = default;

        [[nodiscard]] static auto create(SocketProtocol protocol) -> std::expected<Socket, SocketError>;

        [[nodiscard]] auto connect(const SocketAddr& addr) const noexcept -> std::expected<void, SocketError>;
        [[nodiscard]] auto bind(const SocketAddr& addr) const noexcept -> std::expected<void, SocketError>;
        [[nodiscard]] auto accept() const noexcept -> std::expected<std::tuple<Socket, SocketAddr>, SocketError>;

    private:
        Impl m_impl{ nullptr, nullptr };

        Socket() noexcept = default;
    };
}
