#pragma once

#include <expected>
#include <format>
#include <memory>
#include <utility>

namespace net {
    enum class SocketError {
        CreationFailed,
        BindingFailed,
        ConnectionFailed,
        TimedOut,
        ConnectionClosed,
    };

    enum class SocketType {
        Tcp,
        Udp,
    };

    class Socket {
    public:
        [[nodiscard]] static auto create(SocketType type) noexcept -> std::expected<Socket, SocketError>;

    private:
        std::unique_ptr<void, void(*)(void*)> m_impl{ nullptr, nullptr };
    };
}

template<typename CharT>
struct std::formatter<net::SocketError, CharT> {
    static constexpr auto parse(auto& ctx) noexcept -> decltype(ctx.begin()) {
        return ctx.begin();
    }

    static auto format(const net::SocketError error, auto& ctx) noexcept -> decltype(ctx.out()) {
        switch(error) {
        case net::SocketError::CreationFailed:
            return std::format_to(ctx.out(), "Failed to create the socket!");
        case net::SocketError::BindingFailed:
            return std::format_to(ctx.out(), "Failed to bind to the specified address!");
        case net::SocketError::ConnectionFailed:
            return std::format_to(ctx.out(), "Failed to connect to the specified address!");
        case net::SocketError::TimedOut:
            return std::format_to(ctx.out(), "The operation timed out!");
        case net::SocketError::ConnectionClosed:
            return std::format_to(ctx.out(), "The connection has been closed!");
        }

        std::unreachable();
    }
};
