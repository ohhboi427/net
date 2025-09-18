#include <net/tcp/stream.hpp>

namespace net {
    auto TcpStream::write_all(const std::span<const u8> data) const noexcept -> std::expected<void, SocketError> {
        usize all_bytes_sent = 0U;

        while(all_bytes_sent < data.size()) {
            const auto bytes_sent = write(data.subspan(all_bytes_sent));
            if(!bytes_sent) {
                return std::unexpected(bytes_sent.error());
            }

            all_bytes_sent += bytes_sent.value();
        }

        return {};
    }

    auto TcpStream::read_exact(const std::span<u8> buffer) const noexcept -> std::expected<void, SocketError> {
        usize all_bytes_received = 0U;

        while(all_bytes_received < buffer.size()) {
            const auto bytes_received = read(buffer.subspan(all_bytes_received));
            if(!bytes_received) {
                if(
                    bytes_received.error() == SocketError::ConnectionClosed
                    && all_bytes_received < buffer.size()
                    && all_bytes_received != 0U
                ) {
                    return std::unexpected(SocketError::ConnectionInterrupted);
                }

                return std::unexpected(bytes_received.error());
            }

            all_bytes_received += bytes_received.value();
        }

        return {};
    }
}
