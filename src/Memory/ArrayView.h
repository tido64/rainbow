#ifndef MEMORY_ARRAYVIEW_H_
#define MEMORY_ARRAYVIEW_H_

#include <cstddef>

#include "Memory/NotNull.h"

template <typename T>
class ArrayView
{
public:
	ArrayView() : ArrayView(0, nullptr) {}
	ArrayView(std::nullptr_t) : size_(0), array_(nullptr) {}
	ArrayView(size_t size, NotNull<T*> array) : size_(size), array_(array) {}
	ArrayView(size_t size, const std::unique_ptr<T[]>& array)
	    : ArrayView(size, array.get()) {}

	const T* data() const { return array_; }
	bool empty() const { return size_ == 0; }
	size_t size() const { return size_; }

	T* begin() const { return array_; }
	T* end() const { return array_ + size_; }

	const T* cbegin() const { return begin(); }
	const T* cend() const { return end(); }

	T& operator[](size_t i) { return array_[i]; }
	const T& operator[](size_t i) const { return array_[i]; }

private:
	size_t size_;
	T* array_;
};

#endif
