#if defined(_WIN32)
#include <ws2/socket.hpp>

#include <net/bit.hpp>

#include <ws2/context.hpp>

#include <algorithm>
#include <bit>
#include <utility>

#pragma comment(lib, "ws2_32.lib")

namespace net {
    namespace {
        [[nodiscard]] constexpr auto convert_address_family(const SocketAddressFamily address_family) noexcept -> i32 {
            switch(address_family) {
            case SocketAddressFamily::Ipv4:
                return AF_INET;
            case SocketAddressFamily::Ipv6:
                return AF_INET6;
            }

            std::unreachable();
        }

        [[nodiscard]] constexpr auto protocol_socket_properties(const SocketProtocol protocol) -> std::tuple<i32, i32> {
            switch(protocol) {
            case SocketProtocol::Tcp:
                return std::tuple{ SOCK_STREAM, IPPROTO_TCP };
            case SocketProtocol::Udp:
                return std::tuple{ SOCK_DGRAM, IPPROTO_UDP };
            }

            std::unreachable();
        }
    }

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

        const auto [type, protocol] = protocol_socket_properties(config.protocol);

        WS2Socket socket{};
        socket.m_handle = ::socket(
            convert_address_family(config.address_family),
            type,
            protocol
        );

        if(socket.m_handle == INVALID_SOCKET) {
            return std::unexpected{ SocketError::CreationFailed };
        }

        if(constexpr i32 OFF = 0; setsockopt(
            socket.m_handle,
            IPPROTO_IPV6,
            IPV6_V6ONLY,
            reinterpret_cast<const char*>(&OFF),
            sizeof(OFF)
        ) == SOCKET_ERROR) {
            return std::unexpected{ SocketError::CreationFailed };
        }

        return socket;
    }

    auto WS2Socket::connect(const SocketAddr& addr) const noexcept -> std::expected<void, SocketError> {
        const auto addr_impl = static_cast<WS2SocketAddr>(addr);
        const auto [ptr, size] = addr_impl.as_generic(); // NOLINT

        if(::connect(m_handle, ptr, static_cast<int>(size)) == SOCKET_ERROR) {
            return std::unexpected{ SocketError::ConnectionFailed };
        }

        return {};
    }

    auto WS2Socket::bind(const SocketAddr& addr) const noexcept -> std::expected<void, SocketError> {
        const auto addr_impl = static_cast<WS2SocketAddr>(addr);
        const auto [ptr, size] = addr_impl.as_generic(); // NOLINT

        if(::bind(m_handle, ptr, static_cast<int>(size)) == SOCKET_ERROR) {
            return std::unexpected{ SocketError::BindingFailed };
        }

        if(::listen(m_handle, SOMAXCONN) == SOCKET_ERROR) {
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

        const auto addr_impl = static_cast<WS2SocketAddr>(addr_info);
        const auto addr = static_cast<SocketAddr>(addr_impl);

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
