#include <net/defines.hpp>
#include <net/socket.hpp>

#include <print>

using namespace net::primitives;

auto main() -> i32 {
    auto socket = net::Socket::create();
    if(!socket) {
        std::println("{}", socket.error());
    }
}
