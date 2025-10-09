#include <net/bit.hpp>
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

    const auto stream = std::move(stream_result).value();
    [[maybe_unused]] const auto write_result = stream.write("Hello, World!"_b);
}
