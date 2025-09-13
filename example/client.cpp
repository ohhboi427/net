#include <net/address.hpp>
#include <net/defines.hpp>
#include <net/tcp/stream.hpp>

#include <print>

using namespace net::primitives;
using namespace net::literals;

auto main() -> i32 {
    auto stream_result = net::TcpStream::connect("127.0.0.1:8080"_v4);
    if(!stream_result) {
        std::println("{}", stream_result.error());

        return 0;
    }

    auto stream = std::move(stream_result).value();
}
