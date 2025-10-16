#pragma once
#if defined(_WIN32)

#include <net/defines.hpp>

#include <expected>

#include <WinSock2.h>

namespace net {
    class WS2Context {
    public:
        WS2Context(WS2Context&& other) noexcept;
        ~WS2Context() noexcept;

        [[nodiscard]] static auto instance() noexcept -> std::expected<void, i32>;

    private:
        WSADATA m_data{};

        WS2Context() noexcept = default;

        [[nodiscard]] static auto create() noexcept -> std::expected<WS2Context, i32>;
    };
}

#endif
