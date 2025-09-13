#pragma once
#if defined(_WIN32)

#include <WinSock2.h>

#include <expected>

namespace net {
    class WS2Context {
    public:
        WS2Context() = default;

        WS2Context(WS2Context&& other) noexcept;

        ~WS2Context() noexcept;

        auto operator=(WS2Context&& other) noexcept -> WS2Context&;

        [[nodiscard]] static auto create() noexcept -> std::expected<WS2Context, int>;

    private:
        WSADATA m_wsa_data{};
    };
}

#endif
