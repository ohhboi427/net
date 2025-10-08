#if defined(_WIN32)
#include <ws2/context.hpp>

#include <utility>

#pragma comment(lib, "ws2_32.lib")

namespace net {
    WS2Context::WS2Context(WS2Context&& other) noexcept
        : m_data{ std::exchange(other.m_data, {}) } {}

    WS2Context::~WS2Context() noexcept {
        if(m_data.wVersion != 0U) {
            WSACleanup();
        }
    }

    auto WS2Context::create() noexcept -> std::expected<WS2Context, i32> {
        WS2Context context{};

        if(const auto result = WSAStartup(MAKEWORD(2U, 2U), &context.m_data); result != 0) {
            return std::unexpected(result);
        }

        return context;
    }
}

#endif
