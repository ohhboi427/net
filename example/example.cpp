#include <net/defines.hpp>
#include <net/socket.hpp>

#include <print>

using namespace net;

auto main() -> i32 {
    const auto socket_result = Socket::create(SocketProtocol::Tcp);
    if(!socket_result) {
        std::println("Failed to create socket!");
    }
}
