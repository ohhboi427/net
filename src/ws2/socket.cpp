#if defined(_WIN32)
#include <ws2/socket.hpp>

#include <net/bit.hpp>

#include <ws2/context.hpp>

#include <bit>

#pragma comment(lib, "ws2_32.lib")

namespace net {
    namespace {
        [[nodiscard]] constexpr auto protocol_socket_type(const SocketProtocol protocol) noexcept -> i32 {
            switch(protocol) {
            case SocketProtocol::Tcp:
                return SOCK_STREAM;
            case SocketProtocol::Udp:
                return SOCK_DGRAM;
            }

            std::unreachable();
        }

        [[nodiscard]] constexpr auto protocol_socket_protocol(const SocketProtocol protocol) noexcept -> i32 {
            switch(protocol) {
            case SocketProtocol::Tcp:
                return IPPROTO_TCP;
            case SocketProtocol::Udp:
                return IPPROTO_UDP;
            }

            std::unreachable();
        }
    }

    WS2Socket::WS2Socket(WS2Socket&& other) noexcept
        : m_handle{ std::exchange(other.m_handle, INVALID_SOCKET) } {}

    WS2Socket::~WS2Socket() noexcept {
        if(m_handle != INVALID_SOCKET) {
            closesocket(m_handle);
        }
    }

    auto WS2Socket::create(const SocketProtocol protocol) -> std::expected<WS2Socket, SocketError> {
        static const auto s_context_result = WS2Context::create();
        if(!s_context_result) {
            return std::unexpected{ SocketError::CreationFailed };
        }

        WS2Socket socket{};
        socket.m_handle = ::socket(AF_INET, protocol_socket_type(protocol), protocol_socket_protocol(protocol));
        if(socket.m_handle == INVALID_SOCKET) {
            return std::unexpected{ SocketError::CreationFailed };
        }

        return socket;
    }

    auto WS2Socket::connect(const SocketAddr& addr) const noexcept -> std::expected<void, SocketError> {
        const sockaddr_in addr_info{
            .sin_family = AF_INET,
            .sin_port = host_to_net(addr.port),
            .sin_addr = std::bit_cast<IN_ADDR>(addr.address),
            .sin_zero = {},
        };

        if(const auto result = ::connect(
            m_handle,
            std::bit_cast<const sockaddr*>(&addr_info),
            sizeof(addr_info)
        ); result == SOCKET_ERROR) {
            return std::unexpected{ SocketError::ConnectionFailed };
        }

        return {};
    }

    auto WS2Socket::bind(const SocketAddr& addr) const noexcept -> std::expected<void, SocketError> {
        const sockaddr_in addr_info{
            .sin_family = AF_INET,
            .sin_port = host_to_net(addr.port),
            .sin_addr = std::bit_cast<IN_ADDR>(addr.address),
            .sin_zero = {},
        };

        if(const auto result = ::bind(
            m_handle,
            std::bit_cast<const sockaddr*>(&addr_info),
            sizeof(addr_info)
        ); result == SOCKET_ERROR) {
            return std::unexpected{ SocketError::BindingFailed };
        }

        if(const auto result = ::listen(m_handle, SOMAXCONN); result == SOCKET_ERROR) {
            return std::unexpected{ SocketError::BindingFailed };
        }

        return {};
    }

    auto WS2Socket::accept() const noexcept -> std::expected<std::tuple<WS2Socket, SocketAddr>, SocketError> {
        sockaddr_in addr_info{};
        i32 addr_info_size = sizeof(addr_info);

        const SOCKET handle = ::accept(m_handle, std::bit_cast<sockaddr*>(&addr_info), &addr_info_size);
        if(handle == INVALID_SOCKET) {
            return std::unexpected{ SocketError::ConnectionFailed };
        }

        WS2Socket socket{};
        socket.m_handle = handle;

        const SocketAddr addr{
            .address = std::bit_cast<std::array<u8, 4U>>(addr_info.sin_addr),
            .port = net_to_host(addr_info.sin_port),
        };

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

    auto Socket::create(const SocketProtocol protocol) -> std::expected<Socket, SocketError> {
        auto socket_impl_result = WS2Socket::create(protocol);
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
