#if defined(_WIN32)
#include <ws2/addr.hpp>

#include <net/bit.hpp>

#include <algorithm>
#include <bit>

#pragma comment(lib, "ws2_32.lib")

namespace net {
    WS2SocketAddr::WS2SocketAddr(const sockaddr_storage& addr_info) noexcept {
        if(addr_info.ss_family == AF_INET) {
            addr = *reinterpret_cast<const sockaddr_in*>(&addr_info);
        } else {
            addr = *reinterpret_cast<const sockaddr_in6*>(&addr_info);
        }
    }

    WS2SocketAddr::WS2SocketAddr(const SocketAddr& addr) noexcept {
        const Visitor visitor{
            [&](const Ipv4Addr& ipv4) noexcept -> std::variant<sockaddr_in, sockaddr_in6> {
                return sockaddr_in{
                    .sin_family = AF_INET,
                    .sin_port = host_to_net(addr.port),
                    .sin_addr = std::bit_cast<IN_ADDR>(ipv4),
                    .sin_zero = {},
                };
            },

            [&](const Ipv6Addr& ipv6) noexcept -> std::variant<sockaddr_in, sockaddr_in6> {
                Ipv6Addr addr_net{};
                std::ranges::transform(
                    ipv6,
                    addr_net.begin(),
                    [](const u16 hex) noexcept -> u16 {
                        return host_to_net(hex);
                    }
                );

                return sockaddr_in6{
                    .sin6_family = AF_INET6,
                    .sin6_port = host_to_net(addr.port),
                    .sin6_flowinfo = {},
                    .sin6_addr = std::bit_cast<IN6_ADDR>(addr_net),
                    .sin6_scope_id = {},
                };
            },
        };

        this->addr = std::visit(visitor, addr.addr);
    }

    WS2SocketAddr::operator SocketAddr() const noexcept {
        constexpr Visitor visitor{
            [](const sockaddr_in& ipv4) {
                return SocketAddr{
                    .addr = std::bit_cast<Ipv4Addr>(ipv4.sin_addr),
                    .port = net_to_host(ipv4.sin_port),
                };
            },

            [](const sockaddr_in6& ipv6) {
                Ipv6Addr addr_host{};
                std::ranges::transform(
                    std::bit_cast<Ipv6Addr>(ipv6.sin6_addr),
                    addr_host.begin(),
                    [](const u16 hex) noexcept -> u16 {
                        return net_to_host(hex);
                    }
                );

                return SocketAddr{
                    .addr = addr_host,
                    .port = net_to_host(ipv6.sin6_port)
                };
            },
        };

        return std::visit(visitor, addr);
    }

    auto WS2SocketAddr::as_generic() const noexcept -> std::tuple<const sockaddr*, usize> {
        return std::visit(
            [](const auto& value) noexcept -> std::tuple<const sockaddr*, usize> {
                return { reinterpret_cast<const sockaddr*>(&value), sizeof(value) };
            },
            addr
        );
    }
}

#endif
