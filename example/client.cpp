#include <net/address.hpp>
#include <net/bit.hpp>
#include <net/defines.hpp>
#include <net/tcp/stream.hpp>

#include <print>

#include <cstdlib>

using namespace net::primitives;
using namespace net::literals::address;
using namespace net::literals::bit;

auto main() -> i32 {
    auto stream_result = net::TcpStream::connect("127.0.0.1:8080"_v4);
    if(!stream_result) {
        std::println("{}", stream_result.error());

        return EXIT_FAILURE;
    }

    const auto stream = std::move(stream_result).value();

    auto bytes_sent = stream.write("Hello, World!"_b);
    if(!bytes_sent) {
        std::println("{}", bytes_sent.error());
    }
}
