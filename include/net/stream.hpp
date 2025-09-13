#pragma once

#include <net/address.hpp>
#include <net/socket.hpp>

#include <expected>

namespace net {
    class Stream {
    public:
        [[nodiscard]] static auto connect(IPv4Address address) noexcept -> std::expected<Stream, SocketError>;

    private:
        std::unique_ptr<void, void(*)(void*)> m_impl{ nullptr, nullptr };
    };
}
