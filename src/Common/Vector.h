// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_VECTOR_H_
#define COMMON_VECTOR_H_

#include <utility>

#include "Common/Arena.h"
#include "Common/Debug.h"

/// Simple vector class, mimicking std::vector.
///
/// This class is mainly for the lack of full STL implementation on some
/// platforms, such as Android. (And because we don't like iterators.)
template<typename T>
class Vector : private NonCopyable<Vector<T>>
{
public:
	Vector(const size_t capacity = 4);
	Vector(Vector<T> &&);
	~Vector();

	/// Returns the element stored at index.
	inline T& at(const size_t i) const;

	/// Returns the last element.
	inline T& back();

	/// Returns a pointer to the first element.
	inline T* begin() const;

	/// Returns size of allocated storage capacity.
	inline size_t capacity() const;

	/// Empties the vector.
	void clear();

	/// Returns a pointer to the end of the vector.
	inline T* end() const;

	/// Erases the element at index while preserving the order.
	void erase(const size_t i);

	/// Finds an element with given value.
	/// \param value  The value of the element to find.
	/// \return Index to the first element whose value matches; -1 otherwise.
	int find(const T &value) const;

	/// Adds an element to the vector.
	void push_back(const T &element);

	/// Adds an element to the vector.
	void push_back(T &&element);

	/// Quick erases the element at index by swapping it out with the last
	/// element. Don't use this if you need to maintain the element order.
	void qerase(const size_t i);

	/// Quick removes a value from the vector by swapping it out with the last
	/// element. Deletes only the first element found by \c find(). Don't use
	/// this if you need to maintain the element order.
	/// \param value  The value to remove.
	void qremove(const T &value);

	/// Removes a value from the vector while preserving the order of elements.
	/// Deletes only the first element found by \c find().
	/// \param value  The value to remove.
	void remove(const T &value);

	/// Increases or decreases the capacity of the vector.
	/// \note On failure, the vector will remain untouched.
	/// \param capacity  The size of the new capacity. If less than the number
	///                  of elements in the container, the container is simply
	///                  tightened.
	void reserve(size_t capacity);

	/// Returns the number of elements in this vector.
	inline size_t size() const;

	inline bool operator==(const Vector<T> &) const;

	/// Returns the element stored at index.
	inline T& operator[](const size_t i) const;

private:
	Arena<T> arena_;   ///< Memory arena.
	size_t count_;     ///< Number of elements in the array.
	size_t reserved_;  ///< Size of allocated memory.
};

template<typename T>
Vector<T>::Vector(const size_t capacity) : count_(0), reserved_(0)
{
	reserve(capacity);
}

template<typename T>
Vector<T>::Vector(Vector<T> &&v)
    : count_(v.count_), reserved_(v.reserved_),
      arena_(std::forward<Arena<T>>(v.arena_))
{
	v.count_ = 0;
	v.reserved_ = 0;
}

template<typename T>
Vector<T>::~Vector()
{
	clear();
}

template<typename T>
T& Vector<T>::at(const size_t i) const
{
	return (*this)[i];
}

template<typename T>
T& Vector<T>::back()
{
	R_ASSERT(count_ > 0, "Tried to access an empty vector");
	return (*this)[count_ - 1];
}

template<typename T>
T* Vector<T>::begin() const
{
	return arena_.get();
}

template<typename T>
size_t Vector<T>::capacity() const
{
	return reserved_;
}

template<typename T>
void Vector<T>::clear()
{
	arena_.release(count_);
	count_ = 0;
}

template<typename T>
T* Vector<T>::end() const
{
	return arena_ + count_;
}

template<typename T>
void Vector<T>::erase(const size_t i)
{
	R_ASSERT(i < count_, "Can't erase a non-existing element");

	arena_[i].~T();
	T *ptr = arena_ + i;
	memcpy(ptr, ptr + 1, (--count_ - i) * sizeof(T));
}

template<typename T>
int Vector<T>::find(const T &value) const
{
	for (size_t i = 0; i < count_; ++i)
		if (arena_[i] == value)
			return i;
	return -1;
}

template<typename T>
void Vector<T>::push_back(const T &element)
{
	// Verify that there is enough space
	if (count_ == reserved_)
		reserve(reserved_ += (reserved_ + 1) >> 1);

	new (arena_ + count_) T(element);
	++count_;
}

template<typename T>
void Vector<T>::push_back(T &&element)
{
	// Verify that there is enough space
	if (count_ == reserved_)
		reserve(reserved_ += (reserved_ + 1) >> 1);

	new (arena_ + count_) T(std::move(element));
	++count_;
}

template<typename T>
void Vector<T>::qerase(const size_t i)
{
	R_ASSERT(i < count_, "Can't erase a non-existing element");

	arena_[i].~T();
	if (--count_ != i)
		memcpy(arena_ + i, arena_ + count_, sizeof(T));
}

template<typename T>
void Vector<T>::qremove(const T &value)
{
	const int i = find(value);
	if (i < 0)
		return;
	qerase(i);
}

template<typename T>
void Vector<T>::remove(const T &value)
{
	const int i = find(value);
	if (i < 0)
		return;
	erase(i);
}

template<typename T>
void Vector<T>::reserve(size_t capacity)
{
	if (capacity < count_)
		capacity = count_;

	R_ASSERT(capacity > 0, "Can't reserve an empty block of memory");

	arena_.resize(count_, capacity);
	reserved_ = capacity;
}

template<typename T>
size_t Vector<T>::size() const
{
	return count_;
}

template<typename T>
bool Vector<T>::operator==(const Vector<T> &v) const
{
	return arena_ == v.arena_;
}

template<typename T>
T& Vector<T>::operator[](const size_t i) const
{
	R_ASSERT(i < count_, "Index out of range");

	return arena_[i];
}

#endif
