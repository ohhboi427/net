#if defined(_WIN32)
#include <ws2/socket.hpp>

#include <ws2/context.hpp>

#include <utility>

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
        : m_socket{ std::exchange(other.m_socket, INVALID_SOCKET) } {}

    WS2Socket::~WS2Socket() noexcept {
        if(m_socket != INVALID_SOCKET) {
            closesocket(m_socket);
        }
    }

    auto WS2Socket::create([[maybe_unused]] const SocketProtocol protocol) -> std::expected<WS2Socket, SocketError> {
        static const auto s_context_result = WS2Context::create();
        if(!s_context_result) {
            return std::unexpected(SocketError::CreationFailed);
        }

        WS2Socket socket{};
        socket.m_socket = ::socket(AF_INET, protocol_socket_type(protocol), protocol_socket_protocol(protocol));
        if(socket.m_socket == INVALID_SOCKET) {
            return std::unexpected(SocketError::CreationFailed);
        }

        return socket;
    }

    auto Socket::create(const SocketProtocol protocol) -> std::expected<Socket, SocketError> {
        auto socket_impl_result = WS2Socket::create(protocol);
        if(!socket_impl_result) {
            return std::unexpected(socket_impl_result.error());
        }

        Socket socket{};
        socket.m_impl = make_impl<WS2Socket>(std::move(socket_impl_result).value());

        return socket;
    }
}

#endif
