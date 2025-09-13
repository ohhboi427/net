#pragma once
#if defined(_WIN32)

#include <net/address.hpp>
#include <net/listener.hpp>
#include <ws2/socket.hpp>

#include <expected>

namespace net {
    class WS2Listener {
    public:
        [[nodiscard]] static auto bind(IPv4Address address) noexcept -> std::expected<WS2Listener, SocketError>;

    private:
        WS2Socket m_socket{};
    };
}

#endif
