#include <net/defines.hpp>
#include <net/tcp/listener.hpp>

#include <print>
#include <utility>

using namespace net;

auto main() -> i32 {
    auto listener_result = TcpListener::bind({ .address = { 0, 0, 0, 0 }, .port = 8080 });
    if(!listener_result) {
        std::println("Failed to bind");

        return -1;
    }

    const auto listener = std::move(listener_result).value();

    while(true) { // NOLINT
        const auto [stream, addr] = listener.accept().value();

        std::println("Connected to {}:{}", addr.address, addr.port);
    }
}
