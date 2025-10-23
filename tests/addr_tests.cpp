#include <net/addr.hpp>

#include <gtest/gtest.h>

#include <format>

using namespace net;

TEST(AddressTests, Unspecified_v4) {
    constexpr Ipv4Addr addr = Ipv4Addr::unspecified();

    EXPECT_EQ(addr, (Ipv4Addr{0U, 0U, 0U, 0U}));
}

TEST(AddressTests, Unspecified_v6) {
    constexpr Ipv6Addr addr = Ipv6Addr::unspecified();

    EXPECT_EQ(addr, (Ipv6Addr{0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U}));
}

TEST(AddressTests, Loopback_v4) {
    constexpr Ipv4Addr addr = Ipv4Addr::loopback();

    EXPECT_EQ(addr, (Ipv4Addr{127U, 0U, 0U, 1U}));
}

TEST(AddressTests, Loopback_v6) {
    constexpr Ipv6Addr addr = Ipv6Addr::loopback();

    EXPECT_EQ(addr, (Ipv6Addr{0U, 0U, 0U, 0U, 0U, 0U, 0U, 1U}));
}

TEST(AddressTests, AddressFamily) {
    constexpr IpAddr addr1 = Ipv4Addr{};
    constexpr IpAddr addr2 = Ipv6Addr{};

    EXPECT_EQ(addr1.address_family(), AddressFamily::Ipv4);
    EXPECT_EQ(addr2.address_family(), AddressFamily::Ipv6);
}

TEST(AddressTests, Formatting_v4) {
    constexpr IpAddr addr = Ipv4Addr{};
    constexpr SocketAddr sock_addr{ .addr = addr, .port = 0 };

    EXPECT_EQ(std::format("{}", addr), "0.0.0.0");
    EXPECT_EQ(std::format("{}", sock_addr), "0.0.0.0:0");
}

TEST(AddressTests, Formatting_v6) {
    // TODO: Update once an actual IPv6 formatting is implemented

    constexpr IpAddr addr = Ipv6Addr{};
    constexpr SocketAddr sock_addr{ .addr = addr, .port = 0 };

    EXPECT_EQ(std::format("{}", addr), "[0000:0000:0000:0000:0000:0000:0000:0000]");
    EXPECT_EQ(std::format("{}", sock_addr), "[0000:0000:0000:0000:0000:0000:0000:0000]:0");
}
