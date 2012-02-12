#ifndef VECTOR_H_
#define VECTOR_H_

#include <cstring>

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

	Vector(const Vector &v);

	~Vector();

	/// Return the element stored at index.
	T& at(const unsigned int i);

	/// Return a pointer to the first element.
	T* begin() const;

	/// Return size of allocated storage capacity.
	unsigned int capacity() const;

	/// Empties the vector.
	void clear();

	/// Add an element to the vector.
	void push_back(const T &element);

	/// Increase or decrease the capacity of the vector.
	/// \param i  The size of the new capacity. If less than the number of
	///           elements in the container, the container is simply tightened.
	void reserve(unsigned int i);

	/// Return the number of elements in this vector.
	unsigned int size() const;

	Vector& operator=(const Vector &v);

	/// Return the element stored at index.
	T& operator[](const unsigned int i) const;

protected:
	unsigned int count;     ///< Number of elements in the array.
	unsigned int reserved;  ///< Size of allocated memory.
	T *c_array;             ///< Actual C-array.
};

template<class T>
Vector<T>::Vector(const int reserve) :
	count(0), reserved(reserve), c_array(new T[reserve]) { }

template<class T>
Vector<T>::Vector(const Vector &v) :
	count(v.count), reserved(v.reserved), c_array(new T[v.reserved])
{
	memcpy(this->c_array, v.c_array, v.count * sizeof(T));
}

template<class T>
Vector<T>::~Vector()
{
	delete[] this->c_array;
}

template<class T>
inline T& Vector<T>::at(const unsigned int i)
{
	R_ASSERT(i < this->count, "at: Tried to access an element out of range.");
	return this->c_array[i];
}

template<class T>
inline T* Vector<T>::begin() const
{
	return this->c_array;
}

template<class T>
inline unsigned int Vector<T>::capacity() const
{
	return this->reserved;
}

template<class T>
inline void Vector<T>::clear()
{
	this->count = 0;
}

template<class T>
void Vector<T>::push_back(const T &element)
{
	// Verify that there is enough space
	if (this->count == this->reserved)
		this->reserve(this->reserved <<= 1);
	this->c_array[this->count] = element;
	++this->count;
}

template<class T>
void Vector<T>::reserve(unsigned int i)
{
	if (i < this->count)
		i = this->count;

	T *a = new T[i];
	this->reserved = i;
	memcpy(a, this->c_array, this->count * sizeof(T));
	delete[] this->c_array;
	this->c_array = a;
}

template<class T>
inline unsigned int Vector<T>::size() const
{
	return this->count;
}

template<class T>
Vector<T>& Vector<T>::operator=(const Vector<T> &v)
{
	this->count = v.count;
	this->reserved = v.reserved;
	delete[] this->c_array;
	this->c_array = new T[this->reserved];
	memcpy(this->c_array, v.c_array, this->count * sizeof(T));
	return *this;
}

template<class T>
T& Vector<T>::operator[](const unsigned int i) const
{
	R_ASSERT(i < this->count, "operator[]: Tried to access an element out of range.");
	return this->c_array[i];
}

#endif
