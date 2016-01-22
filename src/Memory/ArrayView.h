#ifndef MEMORY_ARRAYVIEW_H_
#define MEMORY_ARRAYVIEW_H_

#include <cstddef>

#include "Memory/NotNull.h"

template <typename T>
class ArrayView
{
public:
    ArrayView() : ArrayView(nullptr) {}
    ArrayView(std::nullptr_t) : size_(0), array_(nullptr) {}
    ArrayView(size_t size, NotNull<T*> array) : size_(size), array_(array) {}
    ArrayView(size_t size, const std::unique_ptr<T[]>& array)
        : ArrayView(size, array.get()) {}

    auto data() const -> const T* { return array_; }
    bool empty() const { return size_ == 0; }
    auto size() const { return size_; }

    auto begin() const { return array_; }
    auto end() const { return array_ + size_; }

    auto cbegin() const -> const T* { return begin(); }
    auto cend() const -> const T*{ return end(); }

    auto& operator[](size_t i) { return array_[i]; }
    auto& operator[](size_t i) const { return array_[i]; }

private:
    size_t size_;
    T* array_;
};

#endif
