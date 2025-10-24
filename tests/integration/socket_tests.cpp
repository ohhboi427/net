#include <net/addr.hpp>
#include <net/socket.hpp>

#include <gtest/gtest.h>

#include <utility>

using namespace net;

TEST(socket_tests, invalid_address_family) {
    auto socket_result = Socket::create(
        SocketConfig{ .address_family = AddressFamily::Ipv4, .protocol = Protocol::Tcp }
    );

    ASSERT_TRUE(socket_result.has_value());

    const auto socket = std::move(socket_result).value();
    const auto connect_result = socket.connect({ .addr = Ipv6Addr::loopback(), .port = 8080 });

    ASSERT_FALSE(connect_result.has_value());
    EXPECT_EQ(connect_result.error(), SocketError::ConnectionFailed);
}
