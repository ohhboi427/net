#include <net/address.hpp>
#include <net/defines.hpp>
#include <net/listener.hpp>
#include <net/stream.hpp>

#include <print>

using namespace net::primitives;
using namespace net::literals;

auto main() -> i32 {
    auto listener = net::Listener::bind("0.0.0.0:8080"_v4);
    if(!listener) {
        std::println("{}", listener.error());
    }

    auto stream = net::Stream::connect("127.0.0.1:8080"_v4);
    if(!stream) {
        std::println("{}", stream.error());
    }
}
