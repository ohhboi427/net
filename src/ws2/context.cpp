#if defined(_WIN32)
#include <ws2/context.hpp>

#include <utility>

#pragma comment(lib, "ws2_32.lib")

namespace net {
    auto WS2Context::create() noexcept -> std::expected<WS2Context, int> {
        WS2Context context{};

        if(const int status = WSAStartup(MAKEWORD(2, 2), &context.m_wsa_data); status != 0U) {
            return std::unexpected(status);
        }

        return context;
    }

    WS2Context::WS2Context(WS2Context&& other) noexcept
        : m_wsa_data{ other.m_wsa_data } {
        other.m_wsa_data = {};
    }

    WS2Context::~WS2Context() noexcept {
        if(m_wsa_data.wVersion != 0U) {
            WSACleanup();
        }
    }

    auto WS2Context::operator=(WS2Context&& other) noexcept -> WS2Context& {
        if(const auto old = std::exchange(m_wsa_data, other.m_wsa_data); old.wVersion != 0U) {
            WSACleanup();
        }

        other.m_wsa_data = {};

        return *this;
    }
}

#endif
