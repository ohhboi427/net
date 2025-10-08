#pragma once

#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>

namespace net {
    inline namespace primitives {
        using u8 = std::uint8_t;
        using u16 = std::uint16_t;
        using u32 = std::uint32_t;
        using u64 = std::uint64_t;
        using usize = std::uintptr_t;

        using i8 = std::int8_t;
        using i16 = std::int16_t;
        using i32 = std::int32_t;
        using i64 = std::int64_t;
        using isize = std::intptr_t;

        using f32 = float;
        using f64 = double;
    }

    using Impl = std::unique_ptr<void, void(*)(void*)>;

    template<typename T, typename... Args>
        requires (std::is_constructible_v<T, Args...> && !std::is_array_v<T>)
    [[nodiscard]] constexpr auto make_impl(Args&&... args) -> Impl {
        return {
            new T(std::forward<Args>(args)...),
            [](void* const ptr) noexcept -> void {
                delete static_cast<T*>(ptr);
            }
        };
    }
}
