#include <net/defines.hpp>
#include <net/tcp/listener.hpp>

#include <array>
#include <bit>
#include <print>
#include <span>
#include <string>
#include <utility>

using namespace net;

auto main() -> i32 {
    auto listener_result = TcpListener::bind({ .addr = Ipv6Addr{}, .port = 8080 });
    if(!listener_result) {
        std::println(stderr, "{}", listener_result.error());

        return -1;
    }

    const auto listener = std::move(listener_result).value();

    while(true) {
        const auto [stream, addr] = listener.accept().value();

        while(true) {
            std::array<byte, 1024U> buffer{};
            const auto read_result = stream.read(buffer);

            if(!read_result) {
                if(read_result.error() == SocketError::ConnectionClosed) {
                    break;
                }

                std::println(stderr, "{}", read_result.error());

                return -1;
            }

            const std::string_view message(std::bit_cast<const char*>(&buffer[0U]), read_result.value());
            std::println("{}: {}", addr, message);
        }
    }
}
