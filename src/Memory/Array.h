// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
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
    constexpr ArraySpan() = default;

    constexpr ArraySpan(std::nullptr_t) {}

    constexpr ArraySpan(NotNull<T*> data, size_t size)
        : size_(size), data_(data)
    {
    }

    constexpr ArraySpan(T& data) : ArraySpan(&data, 1) {}

    template <size_t N>
    constexpr ArraySpan(T (&data)[N]) : ArraySpan(data, N)
    {
    }

    template <typename U, size_t N>
    constexpr ArraySpan(const std::array<U, N>& arr)
        : ArraySpan(arr.data(), arr.size())
    {
    }

    [[nodiscard]] auto data() const { return data_; }
    [[nodiscard]] auto empty() const { return size_ == 0; }
    [[nodiscard]] auto size() const { return size_; }

    [[nodiscard]] auto begin() const { return data_; }
    [[nodiscard]] auto end() const { return data_ + size_; }

    [[nodiscard]] auto cbegin() const -> const T* { return begin(); }
    [[nodiscard]] auto cend() const -> const T* { return end(); }

    [[nodiscard]] auto operator[](size_t i) -> T& { return data_[i]; }
    [[nodiscard]] auto operator[](size_t i) const -> T& { return data_[i]; }

private:
    size_t size_ = 0;
    T* data_ = nullptr;
};

template <typename T>
using ArrayView = ArraySpan<const T>;

#endif
