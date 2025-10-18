#include <net/socket.hpp>

namespace net {
    auto IpAddr::address_family() const noexcept -> SocketAddressFamily {
        constexpr Visitor visitor{
            []([[maybe_unused]] const Ipv4Addr& addr) noexcept-> SocketAddressFamily {
                return SocketAddressFamily::Ipv4;
            },

            []([[maybe_unused]] const Ipv6Addr& addr) noexcept-> SocketAddressFamily {
                return SocketAddressFamily::Ipv6;
            },
        };

        return std::visit(visitor, *this);
    }
}
