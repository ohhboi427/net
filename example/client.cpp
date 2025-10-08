#include <net/defines.hpp>
#include <net/tcp/stream.hpp>

#include <print>
#include <utility>

using namespace net;

auto main() -> i32 {
    auto stream_result = TcpStream::connect({ .address = { 127, 0, 0, 1 }, .port = 8080 });
    if(!stream_result) {
        std::println("Failed to connect to server");

        return -1;
    }

    [[maybe_unused]] const auto stream = std::move(stream_result).value();
}
