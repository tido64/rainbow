// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_VERTEXARRAY_H_
#define GRAPHICS_VERTEXARRAY_H_

#include <functional>

#include "Graphics/OpenGL.h"

/// Manages a vertex array object for any drawable type. On Android, vertex
/// array objects are emulated.
class VertexArray
{
public:
	/// Unbinds any bound vertex array object.
	static void unbind();

	VertexArray();
	VertexArray(VertexArray&&);
	~VertexArray();

	/// Binds this vertex array object.
	void bind() const;

	/// Reconfigures this vertex array object with a new set of states.
	void reconfigure(std::function<void()> &&array_state);

	/// Returns whether this vertex array object is valid.
	inline explicit operator bool() const;

private:
#ifdef USE_VERTEX_ARRAY_OBJECT
	unsigned int array_;
#else
	std::function<void()> array_;
#endif
};

VertexArray::operator bool() const
{
	return static_cast<const bool>(array_);
}

#endif
