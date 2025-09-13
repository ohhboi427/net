#include <net/address.hpp>
#include <net/defines.hpp>
#include <net/tcp/listener.hpp>

#include <array>
#include <print>

using namespace net::primitives;
using namespace net::literals;

auto main() -> i32 {
    auto listener_result = net::TcpListener::bind("0.0.0.0:8080"_v4);
    if(!listener_result) {
        std::println("{}", listener_result.error());

        return 0;
    }

    const auto listener = std::move(listener_result).value();

    while(true) {
        auto stream_result = listener.accept();
        if(!stream_result) {
            std::println("{}", stream_result.error());

            break;
        }

        const auto stream = std::move(stream_result).value();

        while(true) {
            std::array<u8, 512U> buffer{};

            if(const auto result = stream.read(buffer); !result) {
                std::println("{}", result.error());

                break;
            }

            std::println("{}", reinterpret_cast<const char*>(buffer.data()));
        }
    }
}
