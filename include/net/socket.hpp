#pragma once

#include <net/defines.hpp>

#include <expected>

namespace net {
    enum class SocketProtocol {
        Tcp,
        Udp,
    };

    enum class SocketError {
        CreationFailed,
    };

    class Socket {
    public:
        Socket(Socket&&) noexcept = default;

        [[nodiscard]] static auto create(SocketProtocol protocol) -> std::expected<Socket, SocketError>;

    private:
        Impl m_impl{ nullptr, nullptr };

        Socket() noexcept = default;
    };
}
