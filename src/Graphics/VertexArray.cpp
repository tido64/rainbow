// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <utility>

#include "Common/Debug.h"
#include "Graphics/ShaderDetails.h"
#include "Graphics/VertexArray.h"

namespace
{
	/// Currently bound vertex array.
	const Renderer::VertexArray *g_active_array = nullptr;
}

namespace Renderer
{
	extern unsigned int g_index_buffer;

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

	VertexArray::VertexArray(std::function<void()> &&f)
	    : valid_(false)
#ifdef USE_VERTEX_ARRAY_OBJECT
	    , array_(0)
#endif
	{
	#ifdef USE_VERTEX_ARRAY_OBJECT
		if (!this->valid)
			glGenVertexArrays(1, &this->array_);
		else
		{
			unbind();
			unsigned int old_array = this->array_;
			glGenVertexArrays(1, &this->array_);
			glDeleteVertexArrays(1, &old_array);
		}
		glBindVertexArray(this->array_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_index_buffer);
		f();
		// TODO: Allow arbitrary attributes.
		glEnableVertexAttribArray(Shader::kAttributeVertex);
		glEnableVertexAttribArray(Shader::kAttributeColor);
		glEnableVertexAttribArray(Shader::kAttributeTexCoord);
		glBindVertexArray(0);
	#else
		this->bind_ = std::move(f);
	#endif
		this->valid_ = true;
	}

	void VertexArray::bind() const
	{
		if (this == g_active_array)
			return;
		g_active_array = this;

	#ifdef USE_VERTEX_ARRAY_OBJECT
		glBindVertexArray(this->array_);
	#else
		this->bind_();
	#endif
	}

	VertexArray::~VertexArray()
	{
	#ifdef USE_VERTEX_ARRAY_OBJECT
		if (!this->valid_)
			return;
		glDeleteVertexArrays(1, &this->array_);
	#endif
	}
}
