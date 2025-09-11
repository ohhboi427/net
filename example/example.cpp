#include <net/address.hpp>
#include <net/defines.hpp>

#include <print>

using namespace net::primitives;
using namespace net::literals;

auto main() -> i32 {
    auto address = net::IPv4Address::parse("192.168.0.1:25565");
    if(address) {
        std::println("{}", address.value());
    }
}
