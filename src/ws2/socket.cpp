#if defined(_WIN32)
#include <ws2/socket.hpp>

#include <net/bit.hpp>

#include <ws2/context.hpp>

#include <algorithm>
#include <bit>
#include <type_traits>
#include <variant>

#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

namespace net {
    namespace {
        struct WS2SocketAddr {
            std::variant<sockaddr_in, sockaddr_in6> addr;

            [[nodiscard]] auto as_generic() const noexcept -> std::tuple<const sockaddr*, usize> {
                return std::visit(
                    [](const auto& value) noexcept -> std::tuple<const sockaddr*, usize> {
                        return { reinterpret_cast<const sockaddr*>(&value), sizeof(value) };
                    },
                    addr
                );
            }
        };

        [[nodiscard]] constexpr auto convert_address_family(const SocketAddressFamily address_family) noexcept -> i32 {
            switch(address_family) {
            case SocketAddressFamily::Ipv4:
                return AF_INET;
            case SocketAddressFamily::Ipv6:
                return AF_INET6;
            }

            std::unreachable();
        }

        [[nodiscard]] constexpr auto convert_protocol(const SocketProtocol protocol) noexcept -> i32 {
            switch(protocol) {
            case SocketProtocol::Tcp:
                return IPPROTO_TCP;
            case SocketProtocol::Udp:
                return IPPROTO_UDP;
            }

            std::unreachable();
        }

        [[nodiscard]] constexpr auto protocol_type(const SocketProtocol protocol) noexcept -> i32 {
            switch(protocol) {
            case SocketProtocol::Tcp:
                return SOCK_STREAM;
            case SocketProtocol::Udp:
                return SOCK_DGRAM;
            }

            std::unreachable();
        }

        [[nodiscard]] constexpr auto convert_addr(const SocketAddr& addr) noexcept -> WS2SocketAddr {
            return std::visit(
                [&]<typename T>(const T& value) noexcept -> WS2SocketAddr {
                    if constexpr(std::is_same_v<T, Ipv4Addr>) {
                        return {
                            .addr = sockaddr_in{
                                .sin_family = AF_INET,
                                .sin_port = host_to_net(addr.port),
                                .sin_addr = std::bit_cast<IN_ADDR>(value),
                                .sin_zero = {},
                            }
                        };
                    } else {
                        Ipv6Addr addr_net{};
                        std::ranges::transform(
                            value,
                            addr_net.begin(),
                            [](const u16 hex) noexcept -> u16 {
                                return host_to_net(hex);
                            }
                        );

                        return {
                            .addr = sockaddr_in6{
                                .sin6_family = AF_INET6,
                                .sin6_port = host_to_net(addr.port),
                                .sin6_flowinfo = {},
                                .sin6_addr = std::bit_cast<IN6_ADDR>(addr_net),
                                .sin6_scope_id = {},
                            }
                        };
                    }
                },
                addr.addr
            );
        }
    }

    WS2Socket::WS2Socket(WS2Socket&& other) noexcept
        : m_handle{ std::exchange(other.m_handle, INVALID_SOCKET) } {}

    WS2Socket::~WS2Socket() noexcept {
        if(m_handle != INVALID_SOCKET) {
            closesocket(m_handle);
        }
    }

    auto WS2Socket::create(const SocketConfig config) -> std::expected<WS2Socket, SocketError> {
        static const auto s_context_result = WS2Context::instance();
        if(!s_context_result) {
            return std::unexpected{ SocketError::CreationFailed };
        }

        WS2Socket socket{};
        socket.m_handle = ::socket(
            convert_address_family(config.address_family),
            protocol_type(config.protocol),
            convert_protocol(config.protocol)
        );

        if(socket.m_handle == INVALID_SOCKET) {
            return std::unexpected{ SocketError::CreationFailed };
        }

        return socket;
    }

    auto WS2Socket::connect(const SocketAddr& addr) const noexcept -> std::expected<void, SocketError> {
        const auto addr_info = convert_addr(addr);
        const auto [ptr, size] = addr_info.as_generic();

        if(const auto result = ::connect(
            m_handle,
            ptr,
            size
        ); result == SOCKET_ERROR) {
            return std::unexpected{ SocketError::ConnectionFailed };
        }

        return {};
    }

    auto WS2Socket::bind(const SocketAddr& addr) const noexcept -> std::expected<void, SocketError> {
        const auto addr_info = convert_addr(addr);
        const auto [ptr, size] = addr_info.as_generic();

        if(const auto result = ::bind(
            m_handle,
            ptr,
            size
        ); result == SOCKET_ERROR) {
            return std::unexpected{ SocketError::BindingFailed };
        }

        if(const auto result = ::listen(m_handle, SOMAXCONN); result == SOCKET_ERROR) {
            return std::unexpected{ SocketError::BindingFailed };
        }

        return {};
    }

    auto WS2Socket::accept() const noexcept -> std::expected<std::tuple<WS2Socket, SocketAddr>, SocketError> {
        sockaddr_storage addr_info{};
        i32 addr_info_size = sizeof(addr_info);

        const SOCKET handle = ::accept(m_handle, std::bit_cast<sockaddr*>(&addr_info), &addr_info_size);
        if(handle == INVALID_SOCKET) {
            return std::unexpected{ SocketError::ConnectionFailed };
        }

        WS2Socket socket{};
        socket.m_handle = handle;

        SocketAddr addr{};
        if(addr_info.ss_family == AF_INET) {
            const auto addr_info_ipv4 = reinterpret_cast<const sockaddr_in*>(&addr_info);

            addr.addr = std::bit_cast<Ipv4Addr>(addr_info_ipv4->sin_addr);
            addr.port = net_to_host(addr_info_ipv4->sin_port);
        } else {
            const auto addr_info_ipv6 = reinterpret_cast<const sockaddr_in6*>(&addr_info);

            Ipv6Addr addr_host{};
            std::ranges::transform(
                std::bit_cast<Ipv6Addr>(addr_info_ipv6->sin6_addr),
                addr_host.begin(),
                [](const u16 hex) noexcept -> u16 {
                    return net_to_host(hex);
                }
            );

            addr.addr = addr_host;
            addr.port = net_to_host(addr_info_ipv6->sin6_port);
        }

        return std::tuple{ std::move(socket), addr };
    }

    auto WS2Socket::write(const std::span<const byte> data) const noexcept -> std::expected<usize, SocketError> {
        const auto bytes_written = ::send(
            m_handle,
            std::bit_cast<const char*>(data.data()),
            static_cast<i32>(data.size_bytes()),
            0
        );

        if(bytes_written == SOCKET_ERROR) {
            return std::unexpected{ SocketError::TimedOut };
        }

        return static_cast<usize>(bytes_written);
    }

    auto WS2Socket::read(const std::span<byte> data) const noexcept -> std::expected<usize, SocketError> {
        const auto bytes_read = ::recv(
            m_handle,
            std::bit_cast<char*>(data.data()),
            static_cast<i32>(data.size_bytes()),
            0
        );

        if(bytes_read == SOCKET_ERROR) {
            return std::unexpected{ SocketError::TimedOut };
        }

        if(bytes_read == 0) {
            return std::unexpected{ SocketError::ConnectionClosed };
        }

        return static_cast<usize>(bytes_read);
    }

    auto Socket::create(const SocketConfig config) -> std::expected<Socket, SocketError> {
        auto socket_impl_result = WS2Socket::create(config);
        if(!socket_impl_result) {
            return std::unexpected{ socket_impl_result.error() };
        }

        Socket socket{};
        socket.m_impl = make_impl<WS2Socket>(std::move(socket_impl_result).value());

        return socket;
    }

    auto Socket::connect(const SocketAddr& addr) const noexcept -> std::expected<void, SocketError> {
        const auto& impl = *static_cast<WS2Socket*>(m_impl.get());

        return impl.connect(addr);
    }

    auto Socket::bind(const SocketAddr& addr) const noexcept -> std::expected<void, SocketError> {
        const auto& impl = *static_cast<WS2Socket*>(m_impl.get());

        return impl.bind(addr);
    }

    auto Socket::accept() const noexcept -> std::expected<std::tuple<Socket, SocketAddr>, SocketError> {
        const auto& impl = *static_cast<WS2Socket*>(m_impl.get());

        auto socket_impl_result = impl.accept();
        if(!socket_impl_result) {
            return std::unexpected{ socket_impl_result.error() };
        }

        auto [socket_impl, addr] = std::move(socket_impl_result).value();

        Socket socket{};
        socket.m_impl = make_impl<WS2Socket>(std::move(socket_impl));

        return std::tuple{ std::move(socket), addr };
    }

    auto Socket::write(const std::span<const byte> data) const noexcept -> std::expected<usize, SocketError> {
        const auto& impl = *static_cast<WS2Socket*>(m_impl.get());

        return impl.write(data);
    }

    auto Socket::read(const std::span<byte> data) const noexcept -> std::expected<usize, SocketError> {
        const auto& impl = *static_cast<WS2Socket*>(m_impl.get());

        return impl.read(data);
    }
}

#endif
