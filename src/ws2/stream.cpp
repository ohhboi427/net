#if defined(_WIN32)
#include <ws2/stream.hpp>

#include <bit>
#include <format>
#include <utility>

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

namespace net {
    auto WS2Stream::connect(const IPv4Address address) noexcept -> std::expected<WS2Stream, SocketError> {
        WS2Stream stream{};

        auto socket = WS2Socket::create();
        if(!socket) {
            return std::unexpected(socket.error());
        }

        stream.m_socket = std::move(socket).value();

        const sockaddr_in addr{
            .sin_family = AF_INET,
            .sin_port = address.port_be,
            .sin_addr = std::bit_cast<IN_ADDR>(address.address),
            .sin_zero = {},
        };

        if(const int status = ::connect(
            static_cast<SOCKET>(stream.m_socket),
            reinterpret_cast<const sockaddr*>(&addr),
            sizeof(addr)
        ); status != 0) {
            return std::unexpected(SocketError::ConnectionFailed);
        }

        return stream;
    }

    auto Stream::connect(const IPv4Address address) noexcept -> std::expected<Stream, SocketError> {
        Stream stream{};

        auto ws2_stream = WS2Stream::connect(address);
        if(!ws2_stream) {
            return std::unexpected(ws2_stream.error());
        }

        stream.m_impl = {
            new WS2Stream(std::move(ws2_stream).value()),
            [](void* const ptr) noexcept -> void {
                delete static_cast<WS2Socket*>(ptr);
            }
        };

        return stream;
    }
}

#endif
