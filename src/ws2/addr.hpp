#pragma once
#if defined(_WIN32)

#include <net/addr.hpp>
#include <net/defines.hpp>

#include <tuple>
#include <variant>

#include <WS2tcpip.h>

namespace net {
    struct WS2SocketAddr {
        std::variant<sockaddr_in, sockaddr_in6> addr;

        explicit WS2SocketAddr(const sockaddr_storage& addr_info) noexcept;
        explicit WS2SocketAddr(const SocketAddr& addr) noexcept;

        [[nodiscard]] explicit operator SocketAddr() const noexcept;

        [[nodiscard]] auto as_generic() const noexcept -> std::tuple<const sockaddr*, usize>;
    };
}

#endif
