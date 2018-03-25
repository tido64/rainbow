// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef MEMORY_STABLEARRAY_H_
#define MEMORY_STABLEARRAY_H_

#include <algorithm>
#include <memory>
#include <new>

#include "Common/Logging.h"
#include "Common/NonCopyable.h"
#include "Memory/SmallBuffer.h"

namespace rainbow
{
    /// <summary>
    ///   A fixed-size, heap-allocated array whose indices are stable.
    /// </summary>
    template <typename T>
    class StableArray : private NonCopyable<StableArray<T>>
    {
    public:
        using size_type = uint32_t;
        using value_type = T;

        StableArray(size_type count) noexcept : size_(count)
        {
            auto aligned_sizeof = [](auto bytes, auto align) noexcept
            {
                return ((bytes / align) + (bytes % align != 0)) * align;
            };

            const size_t header_size =
                aligned_sizeof(count * sizeof(size_type), alignof(value_type));
            const size_t bytes = header_size + count * sizeof(value_type);
            auto ptr =
                static_cast<uint8_t*>(::operator new(bytes, std::nothrow));

            indices_ = reinterpret_cast<size_type*>(ptr);
            data_ = reinterpret_cast<value_type*>(ptr + header_size);

            R_ASSERT(
                reinterpret_cast<uintptr_t>(data_) % alignof(value_type) == 0,
                "Memory allocated for type T must be aligned");

            auto seq = [i = -1]() mutable noexcept -> size_type { return ++i; };
            std::generate_n(indices_, count, seq);
        }

        StableArray(StableArray&& array) noexcept
            : indices_(array.indices_), data_(array.data_), size_(array.size_)
        {
            array.indices_ = nullptr;
            array.data_ = nullptr;
            array.size_ = 0;
        }

        ~StableArray() { ::operator delete(indices_); }

        auto data() { return data_; }
        auto data() const -> const value_type* { return data_; }
        auto size() const { return size_; }

        auto find_iterator(size_type offset) const
        {
            for (size_type i = 0; i < size(); ++i)
            {
                if (index_of(i) == offset)
                    return i;
            }

            return size();
        }

        void move(size_type element, size_type new_index)
        {
            R_ASSERT(element < size(), "Index out of bounds");
            R_ASSERT(new_index < size(), "Index out of bounds");

            const auto element_index = index_of(element);
            if (element_index == new_index)
                return;

            if (element_index < new_index)
            {
                const auto lower = element_index + 1;
                rotate(element, lower, new_index, [lower](size_type i) {
                    return i - lower;
                });
            }
            else
            {
                const auto upper = element_index - 1;
                rotate(element, new_index, upper, [upper](size_type i) {
                    return upper - i;
                });
            }
        }

        void swap(size_type i, size_type j)
        {
            if (i == j)
                return;

            R_ASSERT(i < size(), "Index out of bounds");
            R_ASSERT(j < size(), "Index out of bounds");

            std::swap(indices_[i], indices_[j]);
            std::swap(at(i), at(j));
        }

        auto operator[](size_type i) -> value_type& { return at(i); }

        auto operator[](size_type i) const -> const value_type&
        {
            return at(i);
        }

        template <typename F>
        friend void for_each(StableArray& array, F&& func)
        {
            auto data = array.data();
            for (size_type i = 0; i < array.size(); ++i)
                func(data[i]);
        }

    private:
        size_type* indices_;
        value_type* data_;
        size_type size_;

        auto at(size_type i) const -> value_type& { return data_[index_of(i)]; }

        auto index_of(size_type element) const -> size_type
        {
            return indices_[element];
        }

        template <typename F>
        void rotate(size_type element,
                    size_type lower,
                    size_type upper,
                    F&& ordered_index)
        {
            const size_type count = upper - lower + 1;

            std::unique_ptr<size_type[]> scoped_buffer;
            size_type* sorted_indices = get_small_buffer<size_type>(count);
            if (sorted_indices == nullptr)
            {
                scoped_buffer = std::make_unique<size_type[]>(count);
                sorted_indices = scoped_buffer.get();
            }

            for (size_type i = 0; i < size(); ++i)
            {
                const auto j = index_of(i);
                if (j < lower || j > upper)
                    continue;

                sorted_indices[ordered_index(j)] = i;
            }

            auto rotate = [this, element](size_type i) { swap(element, i); };
            std::for_each(sorted_indices, sorted_indices + count, rotate);
        }
    };
}

#endif
