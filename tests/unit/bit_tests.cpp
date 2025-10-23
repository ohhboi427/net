#include <net/bit.hpp>

#include <gtest/gtest.h>

#include <array>
#include <bit>

using namespace net;

TEST(bit_tests, host_to_net) {
    constexpr u8 ref8 = 0x01UI8;
    constexpr u16 ref16 = 0x0123UI16;
    constexpr u32 ref32 = 0x01234567UI32;
    constexpr u64 ref64 = 0x0123456789ABCDEFUI64;

    constexpr u8 expected8 = std::endian::native == std::endian::big ? ref8 : std::byteswap(ref8);
    constexpr u16 expected16 = std::endian::native == std::endian::big ? ref16 : std::byteswap(ref16);
    constexpr u32 expected32 = std::endian::native == std::endian::big ? ref32 : std::byteswap(ref32);
    constexpr u64 expected64 = std::endian::native == std::endian::big ? ref64 : std::byteswap(ref64);

    EXPECT_EQ(host_to_net(ref8), expected8);
    EXPECT_EQ(host_to_net(ref16), expected16);
    EXPECT_EQ(host_to_net(ref32), expected32);
    EXPECT_EQ(host_to_net(ref64), expected64);
}

TEST(bit_tests, net_to_host) {
    constexpr u8 ref8 = 0x01UI8;
    constexpr u16 ref16 = 0x0123UI16;
    constexpr u32 ref32 = 0x01234567UI32;
    constexpr u64 ref64 = 0x0123456789ABCDEFUI64;

    constexpr u8 expected8 = std::endian::native == std::endian::big ? ref8 : std::byteswap(ref8);
    constexpr u16 expected16 = std::endian::native == std::endian::big ? ref16 : std::byteswap(ref16);
    constexpr u32 expected32 = std::endian::native == std::endian::big ? ref32 : std::byteswap(ref32);
    constexpr u64 expected64 = std::endian::native == std::endian::big ? ref64 : std::byteswap(ref64);

    EXPECT_EQ(net_to_host(ref8), expected8);
    EXPECT_EQ(net_to_host(ref16), expected16);
    EXPECT_EQ(net_to_host(ref32), expected32);
    EXPECT_EQ(net_to_host(ref64), expected64);
}

TEST(bit_tests, binary_string_literal) {
    const auto ref = "ABCD"_b;
    constexpr std::array expected{
        std::bit_cast<byte>('A'),
        std::bit_cast<byte>('B'),
        std::bit_cast<byte>('C'),
        std::bit_cast<byte>('D'),
    };

    EXPECT_EQ(ref.size(), expected.size());
    for(usize i = 0U; i < ref.size(); ++i) {
        EXPECT_EQ(ref[i], expected[i]);
    }
}
