// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <utility>

#include "Graphics/OpenGL.h"
#include "Graphics/Renderer.h"
#include "Graphics/VertexArray.h"

namespace
{
	/// Currently bound vertex array.
	const VertexArray *g_active_array = nullptr;
}

void VertexArray::unbind()
{
	if (!g_active_array)
		return;

	g_active_array = nullptr;

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
{ }

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
	if (this == g_active_array)
		return;

	g_active_array = this;

#ifdef USE_VERTEX_ARRAY_OBJECT
	glBindVertexArray(array_);
#else
	array_();
#endif
}

void VertexArray::reconfigure(std::function<int()> &&array_state)
{
#ifdef USE_VERTEX_ARRAY_OBJECT
	if (!array_)
		glGenVertexArrays(1, &array_);
	else
	{
		unbind();
		const unsigned int old_array = array_;
		glGenVertexArrays(1, &array_);
		glDeleteVertexArrays(1, &old_array);
	}
	glBindVertexArray(array_);
	Renderer::Instance->bind_element_array();
	switch (array_state())
	{
		case Shader::kAttributeNormal:
			glEnableVertexAttribArray(Shader::kAttributeNormal);
		case Shader::kAttributeTexCoord:
			glEnableVertexAttribArray(Shader::kAttributeTexCoord);
		case Shader::kAttributeColor:
			glEnableVertexAttribArray(Shader::kAttributeColor);
		case Shader::kAttributeVertex:
			glEnableVertexAttribArray(Shader::kAttributeVertex);
			break;
	}
	glBindVertexArray(0);
#else
	array_ = std::move(array_state);
#endif
}
