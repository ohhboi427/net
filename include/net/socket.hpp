#pragma once

#include <expected>
#include <format>
#include <memory>
#include <utility>

namespace net {
    enum class SocketError {
        CreationFailed,
    };

    class Socket {
    public:
        Socket() = default;

        [[nodiscard]] static auto create() noexcept -> std::expected<Socket, SocketError>;

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
        }

        std::unreachable();
    }
};
