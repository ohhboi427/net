#include <net/addr.hpp>
#include <net/defines.hpp>
#include <net/socket.hpp>
#include <net/tcp/listener.hpp>
#include <net/tcp/stream.hpp>

#include <gtest/gtest.h>

#include <array>
#include <bit>
#include <thread>
#include <utility>
#include <variant>

using namespace net;

TEST(tcp_tests, connect_to_non_existing) {
    const auto stream_result = TcpStream::connect({ .addr = Ipv4Addr::loopback(), .port = 8080, });

    ASSERT_FALSE(stream_result.has_value());
    EXPECT_EQ(stream_result.error(), SocketError::ConnectionFailed);
}

TEST(tcp_tests, closing) {
    std::jthread listener_thread(
        [] -> void {
            auto listener_result = TcpListener::bind({ .addr = Ipv4Addr::unspecified(), .port = 8080, });
            ASSERT_TRUE(listener_result.has_value());

            const auto listener = std::move(listener_result).value();

            auto accept_result = listener.accept();
            ASSERT_TRUE(accept_result.has_value());

            const auto [stream, addr] = std::move(accept_result).value();

            std::array<byte, 1U> data{};
            const auto read_result = stream.read(data);
            ASSERT_FALSE(read_result.has_value());
            EXPECT_EQ(read_result.error(), SocketError::ConnectionClosed);
        }
    );

    const auto stream_result = TcpStream::connect({ .addr = Ipv4Addr::loopback(), .port = 8080, });
    ASSERT_TRUE(stream_result.has_value());
}

TEST(tcp_tests, dual_stack) {
    std::jthread listener_thread(
        [] -> void {
            auto listener_result = TcpListener::bind({ .addr = Ipv6Addr::unspecified(), .port = 8080, });
            ASSERT_TRUE(listener_result.has_value());

            const auto listener = std::move(listener_result).value();

            auto accept_result = listener.accept();
            ASSERT_TRUE(accept_result.has_value());

            const auto [stream, addr] = std::move(accept_result).value();
            ASSERT_TRUE(addr.addr.address_family() == AddressFamily::Ipv6);

            EXPECT_EQ(std::get<Ipv6Addr>(addr.addr), (Ipv6Addr{ 0U, 0U, 0U, 0U, 0U, 0xFFFFU, 0x7F00U, 0x0001U }));
        }
    );

    const auto stream_result = TcpStream::connect({ .addr = Ipv4Addr::loopback(), .port = 8080, });
    ASSERT_TRUE(stream_result.has_value());
}

TEST(tcp_tests, echo_server) {
    std::jthread listener_thread(
        [] -> void {
            auto listener_result = TcpListener::bind({ .addr = Ipv4Addr::unspecified(), .port = 8080, });
            ASSERT_TRUE(listener_result.has_value());

            const auto listener = std::move(listener_result).value();

            auto accept_result = listener.accept();
            ASSERT_TRUE(accept_result.has_value());

            const auto [stream, addr] = std::move(accept_result).value();

            std::array<byte, 4U> data{};
            const auto read_result = stream.read(data);
            ASSERT_TRUE(read_result.has_value());
            EXPECT_EQ(read_result.value(), 4U);

            const auto write_result = stream.write(data);
            ASSERT_TRUE(write_result.has_value());
            EXPECT_EQ(write_result.value(), 4U);
        }
    );

    auto stream_result = TcpStream::connect({ .addr = Ipv4Addr::loopback(), .port = 8080, });
    ASSERT_TRUE(stream_result.has_value());

    const auto stream = std::move(stream_result).value();

    constexpr std::array message{
        std::bit_cast<byte>('p'),
        std::bit_cast<byte>('i'),
        std::bit_cast<byte>('n'),
        std::bit_cast<byte>('g'),
    };

    const auto write_result = stream.write(message);
    ASSERT_TRUE(write_result.has_value());
    EXPECT_EQ(write_result.value(), 4U);

    std::array<byte, 4U> data{};
    const auto read_result = stream.read(data);
    ASSERT_TRUE(read_result.has_value());
    EXPECT_EQ(read_result.value(), 4U);

    EXPECT_EQ(data, message);
}
