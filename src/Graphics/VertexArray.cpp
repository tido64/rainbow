// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/VertexArray.h"

#include <utility>

#include "Graphics/OpenGL.h"
#include "Graphics/Renderer.h"

void VertexArray::unbind()
{
#ifdef USE_VERTEX_ARRAY_OBJECT
	glBindVertexArray(0);
#else
	glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
}

VertexArray::VertexArray()
#ifdef USE_VERTEX_ARRAY_OBJECT
    : array_(0)
#endif
{}

VertexArray::~VertexArray()
{
#ifdef USE_VERTEX_ARRAY_OBJECT
	if (!array_)
		return;

	glDeleteVertexArrays(1, &array_);
#endif
}

void VertexArray::bind() const
{
#ifdef USE_VERTEX_ARRAY_OBJECT
	glBindVertexArray(array_);
#else
	array_();
#endif
}

void VertexArray::reconfigure(std::function<void()> &&array_state)
{
#ifdef USE_VERTEX_ARRAY_OBJECT
	GLuint array;
	glGenVertexArrays(1, &array);
	glBindVertexArray(array);
	Renderer::Get()->bind_element_array();
	array_state();
	glBindVertexArray(0);
	if (array_)
		glDeleteVertexArrays(1, &array_);
	array_ = array;
#else
	array_ = std::forward<std::function<void()>>(array_state);
#endif
}
