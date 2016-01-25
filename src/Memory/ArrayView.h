#ifndef MEMORY_ARRAYVIEW_H_
#define MEMORY_ARRAYVIEW_H_

#include <cstddef>

#include "Memory/NotNull.h"

template <typename T>
class ArrayView
{
public:
    constexpr ArrayView() : ArrayView(nullptr) {}

    constexpr ArrayView(std::nullptr_t) : size_(0), data_(nullptr) {}

    constexpr ArrayView(std::nullptr_t, size_t)
        : size_(0), data_(nullptr) {}

    constexpr ArrayView(T& data) : ArrayView(&data, 1) {}

    template <size_t N>
    constexpr ArrayView(const T (&data)[N])
        : ArrayView(data, N) {}

    constexpr ArrayView(NotNull<T*> data, size_t size)
        : size_(size), data_(data) {}

    constexpr ArrayView(const std::unique_ptr<T[]>& data, size_t size)
        : ArrayView(data.get(), size) {}

    auto data() const -> const T* { return data_; }
    bool empty() const { return size_ == 0; }
    auto size() const { return size_; }

    auto begin() const { return data_; }
    auto end() const { return data_ + size_; }

    auto cbegin() const -> const T* { return begin(); }
    auto cend() const -> const T* { return end(); }

    auto& operator[](size_t i) { return data_[i]; }
    auto& operator[](size_t i) const { return data_[i]; }

private:
    size_t size_;
    T* data_;
};

#endif
