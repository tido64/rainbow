// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_VERTEXARRAY_H_
#define GRAPHICS_VERTEXARRAY_H_

#include <functional>

#include "Graphics/OpenGL.h"

/// <summary>
///   Manages a vertex array object for any drawable type. On Android, vertex
///   array objects are emulated.
/// </summary>
class VertexArray
{
public:
	/// <summary>Unbinds any bound vertex array object.</summary>
	static void unbind();

	VertexArray();
	VertexArray(VertexArray&&);
	~VertexArray();

	/// <summary>Binds this vertex array object.</summary>
	void bind() const;

	/// <summary>
	///   Reconfigures this vertex array object with a new set of states.
	/// </summary>
	void reconfigure(std::function<void()> &&array_state);

	/// <summary>Returns whether this vertex array object is valid.</summary>
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
