#include <net/addr.hpp>

#include <gtest/gtest.h>

#include <format>

using namespace net;

TEST(address_tests, unspecified_v4) {
    constexpr Ipv4Addr ref = Ipv4Addr::unspecified();
    constexpr Ipv4Addr expected{ 0U, 0U, 0U, 0U };

    EXPECT_EQ(ref, expected);
}

TEST(address_tests, unspecified_v6) {
    constexpr Ipv6Addr ref = Ipv6Addr::unspecified();
    constexpr Ipv6Addr expected{ 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };

    EXPECT_EQ(ref, expected);
}

TEST(address_tests, loopback_v4) {
    constexpr Ipv4Addr ref = Ipv4Addr::loopback();
    constexpr Ipv4Addr expected{ 127U, 0U, 0U, 1U };

    EXPECT_EQ(ref, expected);
}

TEST(address_tests, loopback_v6) {
    constexpr Ipv6Addr ref = Ipv6Addr::loopback();
    constexpr Ipv6Addr expected{ 0U, 0U, 0U, 0U, 0U, 0U, 0U, 1U };

    EXPECT_EQ(ref, expected);
}

TEST(address_tests, address_family) {
    constexpr IpAddr ref1 = Ipv4Addr{};
    constexpr IpAddr ref2 = Ipv6Addr{};

    constexpr auto expected1 = AddressFamily::Ipv4;
    constexpr auto expected2 = AddressFamily::Ipv6;

    EXPECT_EQ(ref1.address_family(), expected1);
    EXPECT_EQ(ref2.address_family(), expected2);
}

TEST(address_tests, formatting_v4) {
    constexpr IpAddr ref = Ipv4Addr{};
    constexpr SocketAddr ref_sock{ .addr = ref, .port = 0U };

    constexpr auto expected = "0.0.0.0";
    constexpr auto expected_sock = "0.0.0.0:0";

    EXPECT_EQ(std::format("{}", ref), expected);
    EXPECT_EQ(std::format("{}", ref_sock), expected_sock);
}

TEST(address_tests, formatting_v6) {
    // TODO: Update once an actual IPv6 formatting is implemented

    constexpr IpAddr ref = Ipv6Addr{};
    constexpr SocketAddr ref_sock{ .addr = ref, .port = 0U };

    constexpr auto expected = "[0000:0000:0000:0000:0000:0000:0000:0000]";
    constexpr auto expected_sock = "[0000:0000:0000:0000:0000:0000:0000:0000]:0";

    EXPECT_EQ(std::format("{}", ref), expected);
    EXPECT_EQ(std::format("{}", ref_sock), expected_sock);
}
