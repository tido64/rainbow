// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_RAWPTR_H_
#define COMMON_RAWPTR_H_

#include <cstdint>

namespace rainbow
{
    template <typename T>
    class RawPtr
    {
    public:
        using element_type = T;

        explicit RawPtr(element_type* p) : ptr_(p) {}

        explicit RawPtr(uint32_t upper, uint32_t lower)
            : ptr_(to_pointer(upper, lower))
        {
        }

        auto get() const { return ptr_; }
        auto lower() const { return offset(0); }
        auto upper() const { return offset(32); }

    private:
        element_type* ptr_;

        static auto to_pointer(uint32_t upper, uint32_t lower) -> element_type*
        {
            auto value = (static_cast<uintptr_t>(upper) << 32) + lower;
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
            return reinterpret_cast<element_type*>(value);
        }

        [[nodiscard]] auto offset(uintptr_t offset) const -> uint32_t
        {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
            auto value = reinterpret_cast<uintptr_t>(ptr_);
            return static_cast<uint32_t>((value >> offset) & 0xffffffff);
        }
    };
}  // namespace rainbow

#endif
