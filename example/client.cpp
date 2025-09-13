#include <net/address.hpp>
#include <net/defines.hpp>
#include <net/tcp/stream.hpp>

#include <print>
#include <span>

using namespace net::primitives;
using namespace net::literals;

[[nodiscard]] constexpr auto operator""_b(const char* ptr, const usize size) noexcept -> std::span<const u8> {
    return { reinterpret_cast<const u8*>(ptr), size + 1U /* Include the '\0' */ }; // NOLINT
}

auto main() -> i32 {
    auto stream_result = net::TcpStream::connect("127.0.0.1:8080"_v4);
    if(!stream_result) {
        std::println("{}", stream_result.error());

        return 0;
    }

    const auto stream = std::move(stream_result).value();

    auto bytes_sent = stream.write("Hello, World!"_b);
    if(!bytes_sent) {
        std::println("{}", bytes_sent.error());
    }
}
