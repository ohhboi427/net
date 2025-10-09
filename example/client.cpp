#include <net/bit.hpp>
#include <net/defines.hpp>
#include <net/tcp/stream.hpp>

#include <print>
#include <utility>

using namespace net;

auto main() -> i32 {
    auto stream_result = TcpStream::connect({ .address = { 127, 0, 0, 1 }, .port = 8080 });
    if(!stream_result) {
        std::println(stderr, "{}", stream_result.error());

        return -1;
    }

    const auto stream = std::move(stream_result).value();

    if(const auto write_result = stream.write("Hello, World!"_b); !write_result) {
        std::println(stderr, "{}", write_result.error());
    }
}
