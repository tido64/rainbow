#ifndef VECTOR_H_
#define VECTOR_H_

#include <cstring>
#include <new>

#include "Common/Debug.h"
#include "Common/NonCopyable.h"

/// Simple vector class, mimicking std::vector.
///
/// This class is mainly for the lack of full STL implementation on some
/// platforms, such as Android. (And because we don't like iterators.)
///
/// Copyright 2010-13 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
template<typename T>
class Vector : private NonCopyable<Vector<T> >
{
public:
	Vector(const int reserve = 8);

	~Vector();

	/// Return the element stored at index.
	inline T& at(const size_t i);

	/// Return a pointer to the first element.
	inline T* begin() const;

	/// Return size of allocated storage capacity.
	inline size_t capacity() const;

	/// Empties the vector.
	void clear();

	/// Erase the element at index while preserving the order.
	void erase(const size_t i);

	/// Find an element with given value.
	/// \param value  The value of the element to find.
	/// \return Index to the first element whose value matches; -1 otherwise.
	int find(const T &value) const;

	/// Add an element to the vector.
	void push_back(const T &value);

	/// Quick erase the element at index by swapping it out with the last
	/// element. Don't use this if you need to maintain the element order.
	void qerase(const size_t i);

	/// Quick remove a value from the vector by swapping it out with the last
	/// element. Deletes only the first element found by @c find(). Don't use
	/// this if you need to maintain the element order.
	/// \param value  The value to remove.
	void qremove(const T &value);

	/// Remove a value from the vector while preserving the order of elements.
	/// Deletes only the first element found by @c find().
	/// \param value  The value to remove.
	void remove(const T &value);

	/// Increase or decrease the capacity of the vector.
	/// \note On failure, the vector will remain untouched.
	/// \param i  The size of the new capacity. If less than the number of
	///           elements in the container, the container is simply tightened.
	void reserve(size_t i);

	/// Return the number of elements in this vector.
	inline size_t size() const;

	/// Return the element stored at index.
	inline T& operator[](const size_t i) const;

private:
	size_t count;     ///< Number of elements in the array.
	size_t reserved;  ///< Size of allocated memory.
	T *c_array;       ///< Actual C-array.
};

template<typename T>
Vector<T>::Vector(const int reserve) :
	count(0), reserved(0), c_array(nullptr)
{
	this->reserve(reserve);
}

template<typename T>
Vector<T>::~Vector()
{
	this->clear();
	operator delete(this->c_array);
}

template<typename T>
T& Vector<T>::at(const size_t i)
{
	return (*this)[i];
}

template<typename T>
T* Vector<T>::begin() const
{
	return this->c_array;
}

template<typename T>
size_t Vector<T>::capacity() const
{
	return this->reserved;
}

template<typename T>
void Vector<T>::clear()
{
	while (this->count)
		this->c_array[--this->count].~T();
	R_ASSERT(!this->count, "Failed to clear vector");
}

template<typename T>
void Vector<T>::erase(const size_t i)
{
	R_ASSERT(i < this->count, "Can't erase a non-existing element");
	this->c_array[i].~T();
	T *arr = this->c_array + i;
	memmove(arr, arr + 1, (--this->count - i) * sizeof(T));
}

template<typename T>
int Vector<T>::find(const T &element) const
{
	for (size_t i = 0; i < this->count; ++i)
		if (this->c_array[i] == element)
			return i;
	return -1;
}

template<typename T>
void Vector<T>::push_back(const T &element)
{
	// Verify that there is enough space
	if (this->count == this->reserved)
		this->reserve(this->reserved += (this->reserved + 1) >> 1);

	new (this->c_array + this->count) T(element);
	++this->count;
}

template<typename T>
void Vector<T>::qerase(const size_t i)
{
	R_ASSERT(i < this->count, "Can't erase a non-existing element");
	this->c_array[i].~T();
	if (--this->count != i)
		memcpy(this->c_array + i, this->c_array + this->count, sizeof(T));
}

template<typename T>
void Vector<T>::qremove(const T &element)
{
	const int i = this->find(element);
	if (i < 0)
		return;
	this->qerase(i);
}

template<typename T>
void Vector<T>::remove(const T &element)
{
	const int i = this->find(element);
	if (i < 0)
		return;
	this->erase(i);
}

template<typename T>
void Vector<T>::reserve(size_t i)
{
	if (i < this->count)
		i = this->count;

	R_ASSERT(i > 0, "Can't reserve an empty block of memory");

	T *arr = static_cast<T*>(operator new(i * sizeof(T)));
	if (!arr)
	{
		// FIXME: Handle exception.
		return;
	}

	memcpy(arr, this->c_array, this->count * sizeof(T));
	operator delete(this->c_array);
	this->c_array = arr;
	this->reserved = i;
}

template<typename T>
size_t Vector<T>::size() const
{
	return this->count;
}

template<typename T>
T& Vector<T>::operator[](const size_t i) const
{
	R_ASSERT(i < this->count, "Tried to access an element out of range");
	return this->c_array[i];
}

#endif
