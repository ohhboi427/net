#include <net/address.hpp>
#include <net/defines.hpp>

using namespace net::primitives;
using namespace net::literals;

auto main() -> i32 {
    [[maybe_unused]] constexpr auto addr = "192.168.0.1:25565"_v4;
}
