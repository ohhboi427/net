#include <net/socket.hpp>

namespace net {
    auto SocketAddr::address_family() const noexcept -> SocketAddressFamily {
        return std::visit(
            [&]<typename T>([[maybe_unused]] const T& value) noexcept -> SocketAddressFamily {
                if constexpr(std::is_same_v<T, Ipv4Addr>) {
                    return SocketAddressFamily::Ipv4;
                } else {
                    return SocketAddressFamily::Ipv6;
                }
            },
            addr
        );
    }
}
