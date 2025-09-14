#include <net/serialize.hpp>

namespace net {
    auto Serializer<std::string>::serialize(const std::string_view str) noexcept -> std::vector<u8> {
        return { str.begin(), str.end() };
    }

    auto Serializer<std::string>::deserialize(
        const std::span<const u8> buffer
    ) noexcept -> std::expected<std::string, SocketError> {
        return std::string{ buffer.begin(), buffer.end() };
    }
}
