/// Simple vector class. Mimics STL::vector.

/// This class is mainly for the lack of full STL implementation on some
/// platforms, such as Android. And for some bizarre reason, it is also faster
/// than STL::vector.
///
/// Copyright 2010-11 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef VECTOR_H_
#define VECTOR_H_

#include <cassert>
#include <cstring>

template<class T>
class Vector
{
public:
	Vector(const int reserve = 8) :
		count(0), reserved(reserve), array(new T[reserve])
	{ }

	Vector(const Vector &v) :
		count(v.count), reserved(v.reserved), array(new T[v.reserved])
	{
		memcpy(this->array, v.array, v.count * sizeof(T));
	}

	~Vector() { delete[] this->array; }

	inline T& at(const unsigned int i)
	{
		assert(i < this->count);
		return this->array[i];
	}

	/// Return a pointer to the first element.
	inline T* begin() const { return this->array; }

	/// Add an element to the vector.
	void push_back(const T &element)
	{
		// Check that there is enough space
		if (this->count == this->reserved)
		{
			this->reserved <<= 1;
			T *a = new T[this->reserved];
			memcpy(a, this->array, this->count * sizeof(T));
			delete[] this->array;
			this->array = a;
		}
		this->array[this->count++] = element;
	}

	/// Return the number of elements in this vector.
	inline unsigned int size() const { return this->count; }

	Vector& operator=(const Vector &v)
	{
		this->count = v.count;
		this->reserved = v.reserved;
		delete[] this->array;
		this->array = new T[this->reserved];
		memcpy(this->array, v.array, this->count * sizeof(T));
		return *this;
	}

	/// Return the element stored at index.
	inline T& operator[](const unsigned int i) const
	{
		return this->array[i];
	}

private:
	unsigned int count;
	unsigned int reserved;
	T *array;
};

#endif
