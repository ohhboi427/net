#pragma once
#if defined(_WIN32)

#include <net/address.hpp>
#include <net/stream.hpp>
#include <ws2/socket.hpp>

#include <expected>

namespace net {
    class WS2Stream {
    public:
        [[nodiscard]] static auto connect(IPv4Address address) noexcept -> std::expected<WS2Stream, SocketError>;

    private:
        WS2Socket m_socket{};
    };
}

#endif
