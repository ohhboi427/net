#include <net/defines.hpp>
#include <net/tcp/listener.hpp>

using namespace net;

auto main() -> i32 {
    const auto listener = TcpListener::bind().value();

    while(true) { // NOLINT
        const auto stream = listener.accept().value();
    }
}
