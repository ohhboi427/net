#include <net/addr.hpp>

namespace net {
    auto IpAddr::address_family() const noexcept -> AddressFamily {
        constexpr Visitor visitor{
            []([[maybe_unused]] const Ipv4Addr& addr) noexcept-> AddressFamily {
                return AddressFamily::Ipv4;
            },

            []([[maybe_unused]] const Ipv6Addr& addr) noexcept-> AddressFamily {
                return AddressFamily::Ipv6;
            },
        };

        return std::visit(visitor, *this);
    }
}
