#pragma once

#include <net/address.hpp>
#include <net/socket.hpp>

#include <expected>

namespace net {
    class Listener {
    public:
        [[nodiscard]] static auto bind(IPv4Address address) noexcept -> std::expected<Listener, SocketError>;

    private:
        std::unique_ptr<void, void(*)(void*)> m_impl{ nullptr, nullptr };
    };
}
