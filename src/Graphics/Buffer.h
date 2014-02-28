// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_BUFFER_H_
#define GRAPHICS_BUFFER_H_

#include "Common/Vector.h"
#include "Graphics/OpenGL.h"

template<typename T>
class Buffer
{
public:
	Buffer(const size_t reserve = 4);
	~Buffer();

	Vector<T>& storage();
	const Vector<T>& storage() const;

	void bind() const;
	void bind(const unsigned int index) const;

	/// Sends committable data to the GPU. In effect, sending only data that has
	/// been explicitly pushed into storage.
	void commit() const;

	/// Sends \p count elements of the buffer to the GPU. Useful for objects
	/// that access the storage directly.
	void upload(const size_t count) const;

private:
	unsigned int id_;
	Vector<T> storage_;
};

template<typename T>
Buffer<T>::Buffer(const size_t reserve) : id_(0), storage_(reserve)
{
	glGenBuffers(1, &this->id_);
}

template<typename T>
Buffer<T>::~Buffer()
{
	glDeleteBuffers(1, &this->id_);
}

template<typename T>
Vector<T>& Buffer<T>::storage()
{
	return this->storage_;
}

template<typename T>
const Vector<T>& Buffer<T>::storage() const
{
	return this->storage_;
}

template<typename T>
void Buffer<T>::commit() const
{
	this->upload(this->storage_.size());
}

template<typename T>
void Buffer<T>::upload(const size_t count) const
{
	glBindBuffer(GL_ARRAY_BUFFER, this->id_);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(T),
	                              this->storage_.begin(),
	                              GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

#endif
