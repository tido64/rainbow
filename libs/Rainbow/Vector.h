/// Simple vector class. Mimics STL::vector.

/// This class is mainly for the lack of full STL implementation.
///
/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef VECTOR_H_
#define VECTOR_H_

#include <Rainbow/Hardware/Platform.h>

#if 1//def RAINBOW_ANDROID
#	include <cassert>
#	include <cstring>

template<class T>
class Vector
{
public:
	Vector(const int reserve = 8) : count(0), reserved(reserve)
	{
		assert(reserve > 0);
		this->array = new T[this->reserved];
	}

	~Vector() { delete[] this->array; }

	inline T& at(const unsigned int i)
	{
		assert(i < this->count);
		return this->array[i];
	}

	/// Returns a pointer to the first element.
	inline T* begin() const { return this->array; }

	/// Adds an element to the vector.
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

	/// Returns the number of elements in this vector.
	inline unsigned int size() const { return this->count; }

	/// Returns the element stored at index.
	inline T& operator[](const unsigned int i)
	{
		assert(i < this->count);
		return this->array[i];
	}

private:
	unsigned int count;
	unsigned int reserved;
	T *array;
};

#else
#	include <vector>
#	define Vector std::vector
#endif

#endif
