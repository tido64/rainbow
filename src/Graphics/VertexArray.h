// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_VERTEXARRAY_H_
#define GRAPHICS_VERTEXARRAY_H_

#include <functional>

#include "Graphics/OpenGL.h"
#undef USE_VERTEX_ARRAY_OBJECT  // TODO

/// Implements the equivalent of OpenGL's vertex array object.
class VertexArray
{
public:
	static void unbind();

	VertexArray(std::function<void()> &&f);
	~VertexArray();

	void bind() const;

	inline operator bool() const;

private:
	bool valid_;
#ifdef USE_VERTEX_ARRAY_OBJECT
	unsigned int array_;
#else
	std::function<void()> bind_;
#endif
};

VertexArray::operator bool() const
{
	return this->valid_;
}

#endif
