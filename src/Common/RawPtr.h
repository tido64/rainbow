// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_RAWPTR_H_
#define COMMON_RAWPTR_H_

namespace rainbow
{
    template <typename T>
    class RawPtr
    {
    public:
        using element_type = T;

        explicit RawPtr(T* p) : ptr_(p) {}

        explicit RawPtr(uint32_t upper, uint32_t lower)
            : ptr_(reinterpret_cast<T*>((static_cast<uintptr_t>(upper) << 32) +
                                        lower))
        {
        }

        auto get() const { return ptr_; }
        auto lower() const { return offset(0); }
        auto upper() const { return offset(32); }

    private:
        T* ptr_;

        auto offset(uintptr_t offset) const -> uint32_t
        {
            return static_cast<uint32_t>(
                (reinterpret_cast<uintptr_t>(ptr_) >> offset) & 0xffffffff);
        }
    };
}  // namespace rainbow

#endif
