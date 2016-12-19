// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef MEMORY_ARRAY_H_
#define MEMORY_ARRAY_H_

#include <array>
#include <cstddef>

#include "Memory/NotNull.h"

template <typename T>
class ArraySpan
{
public:
    constexpr ArraySpan() : ArraySpan(nullptr) {}

    constexpr ArraySpan(std::nullptr_t) : size_(0), data_(nullptr) {}

    constexpr ArraySpan(NotNull<T*> data, size_t size)
        : size_(size), data_(data) {}

    constexpr ArraySpan(T& data) : ArraySpan(&data, 1) {}

    template <size_t N>
    constexpr ArraySpan(T (&data)[N]) : ArraySpan(data, N) {}

    template <typename U, size_t N>
    constexpr ArraySpan(const std::array<U, N>& arr)
        : ArraySpan(arr.data(), arr.size())
    {
    }

    auto data() const { return data_; }
    bool empty() const { return size_ == 0; }
    auto size() const { return size_; }

    auto begin() const { return data_; }
    auto end() const { return data_ + size_; }

    auto cbegin() const -> const T* { return begin(); }
    auto cend() const -> const T* { return end(); }

    auto operator[](size_t i) -> T& { return data_[i]; }
    auto operator[](size_t i) const -> T& { return data_[i]; }

private:
    size_t size_;
    T* data_;
};

template <typename T>
using ArrayView = ArraySpan<const T>;

#endif
