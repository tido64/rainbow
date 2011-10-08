#ifndef VECTOR_H_
#define VECTOR_H_

#include <cassert>
#include <cstring>

/// Simple vector class, mimicking STL::vector.
///
/// This class is mainly for the lack of full STL implementation on some
/// platforms, such as Android. And for some bizarre reason, it is also faster
/// than STL::vector.
///
/// Copyright 2010-11 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
template<class T>
class Vector
{
public:
	Vector(const int reserve = 8) :
		count(0), reserved(reserve), c_array(new T[reserve])
	{ }

	Vector(const Vector &v) :
		count(v.count), reserved(v.reserved), c_array(new T[v.reserved])
	{
		memcpy(this->c_array, v.c_array, v.count * sizeof(T));
	}

	~Vector() { delete[] this->c_array; }

	T& at(const unsigned int i)
	{
		assert(i < this->count || !"Rainbow::Vector::at: Tried to access an element out of range");
		return this->c_array[i];
	}

	/// Return a pointer to the first element.
	T* begin() const { return this->c_array; }

	/// Return size of allocated storage capacity.
	unsigned int capacity() const { return this->reserved; }

	/// Empties the vector.
	void clear() { this->count = 0; }

	/// Add an element to the vector.
	void push_back(const T &element)
	{
		// Check that there is enough space
		if (this->count == this->reserved)
			this->reserve(this->reserved <<= 1);
		this->c_array[this->count] = element;
		++this->count;
	}

	/// Increase or decrease the capacity of the vector.
	/// \param i  The size of the new capacity. If less than the number of
	///           elements in the container, the container is simply tightened.
	void reserve(unsigned int i)
	{
		if (i < this->count)
			i = this->count;

		T *a = new T[i];
		this->reserved = i;
		memcpy(a, this->c_array, this->count * sizeof(T));
		delete[] this->c_array;
		this->c_array = a;
	}

	/// Return the number of elements in this vector.
	unsigned int size() const { return this->count; }

	Vector& operator=(const Vector &v)
	{
		this->count = v.count;
		this->reserved = v.reserved;
		delete[] this->c_array;
		this->c_array = new T[this->reserved];
		memcpy(this->c_array, v.c_array, this->count * sizeof(T));
		return *this;
	}

	/// Return the element stored at index.
	T& operator[](const unsigned int i) const
	{
		assert(i < this->count || !"Rainbow::Vector::operator[]: Tried to access an element out of range");
		return this->c_array[i];
	}

private:
	unsigned int count;
	unsigned int reserved;
	T *c_array;
};

#endif
