#ifndef VECTOR_H_
#define VECTOR_H_

#include <cstring>
#include <new>

#include "Common/RainbowAssert.h"

/// Simple vector class, mimicking STL::vector.
///
/// This class is mainly for the lack of full STL implementation on some
/// platforms, such as Android. And for some bizarre reason, it is also faster
/// than STL::vector.
///
/// Copyright 2010-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
template<class T>
class Vector
{
public:
	Vector(const int reserve = 8);

	~Vector();

	/// Return the element stored at index.
	inline T& at(const unsigned int i);

	/// Return a pointer to the first element.
	inline T* begin() const;

	/// Return size of allocated storage capacity.
	inline unsigned int capacity() const;

	/// Empties the vector.
	void clear();

	/// Add an element to the vector.
	void push_back(const T &element);

	/// Increase or decrease the capacity of the vector.
	/// \note On failure, the vector will remain untouched.
	/// \param i  The size of the new capacity. If less than the number of
	///           elements in the container, the container is simply tightened.
	void reserve(unsigned int i);

	/// Return the number of elements in this vector.
	inline unsigned int size() const;

	/// Return the element stored at index.
	inline T& operator[](const unsigned int i) const;

protected:
	unsigned int count;     ///< Number of elements in the array.
	unsigned int reserved;  ///< Size of allocated memory.
	T *c_array;             ///< Actual C-array.

	Vector(const Vector &v);
	Vector& operator=(const Vector &v);
};

template<class T>
Vector<T>::Vector(const int reserve) :
	count(0), reserved(0), c_array(nullptr)
{
	R_ASSERT(reserve > 0, "Can't reserve an empty block of memory");
	this->reserve(reserve);
}

template<class T>
Vector<T>::~Vector()
{
	this->clear();
	free(this->c_array);
}

template<class T>
T& Vector<T>::at(const unsigned int i)
{
	R_ASSERT(i < this->count, "at: Tried to access an element out of range");
	return this->c_array[i];
}

template<class T>
T* Vector<T>::begin() const
{
	return this->c_array;
}

template<class T>
unsigned int Vector<T>::capacity() const
{
	return this->reserved;
}

template<class T>
void Vector<T>::clear()
{
	for (unsigned int i = 0; i < this->count; ++i)
		this->c_array[i].~T();
	this->count = 0;
}

template<class T>
void Vector<T>::push_back(const T &element)
{
	// Verify that there is enough space
	if (this->count == this->reserved)
		this->reserve(this->reserved <<= 1);

	new (this->c_array + this->count) T(element);
	++this->count;
}

template<class T>
void Vector<T>::reserve(unsigned int i)
{
	if (i < this->count)
		i = this->count;

	R_ASSERT(i > 0, "reserve: Can't reserve an empty block of memory");

	T *arr = static_cast<T*>(malloc(i * sizeof(T)));
	if (!arr)
	{
		// FIXME: Handle exception.
		return;
	}

	memcpy(arr, this->c_array, this->count * sizeof(T));
	free(this->c_array);
	this->c_array = arr;
	this->reserved = i;
}

template<class T>
unsigned int Vector<T>::size() const
{
	return this->count;
}

template<class T>
T& Vector<T>::operator[](const unsigned int i) const
{
	R_ASSERT(i < this->count, "operator[]: Tried to access an element out of range");
	return this->c_array[i];
}

#endif
