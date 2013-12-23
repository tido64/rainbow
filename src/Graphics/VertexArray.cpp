/// Copyright 2013 Bifrost Entertainment. All rights reserved.

#include "Graphics/ShaderManager.h"
#include "Graphics/SpriteVertex.h"
#include "Graphics/VertexArray.h"

namespace
{
	unsigned int g_active_buffer = 0;  ///< Currently bound vertex array/buffer.

	void rBindBuffer(const unsigned int buffer)
	{
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glVertexAttribPointer(
			Shader::COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(SpriteVertex),
			reinterpret_cast<void*>(offsetof(SpriteVertex, color)));
		glVertexAttribPointer(
			Shader::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex),
			reinterpret_cast<void*>(offsetof(SpriteVertex, texcoord)));
		glVertexAttribPointer(
			Shader::VERTEX, 2, GL_FLOAT, GL_TRUE, sizeof(SpriteVertex),
			reinterpret_cast<void*>(offsetof(SpriteVertex, position)));
	}
}

namespace Renderer
{
	extern unsigned int g_index_buffer;

	void VertexArray::bind(const VertexArray &array)
	{
	#ifdef USE_VERTEX_ARRAY_OBJECT
		if (array.array == g_active_buffer)
			return;

		glBindVertexArray(array.array);
		g_active_buffer = array.array;
	#else
		if (array.buffer == g_active_buffer)
			return;

		rBindBuffer(array.buffer);
		g_active_buffer = array.buffer;
	#endif
	}

	void VertexArray::unbind()
	{
		if (g_active_buffer == 0)
			return;

	#ifdef USE_VERTEX_ARRAY_OBJECT
		glBindVertexArray(0);
	#else
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	#endif
		g_active_buffer = 0;
	}

	VertexArray::VertexArray() : count(0), array(0), buffer(0)
	{
		glGenBuffers(1, &this->buffer);

	#ifdef USE_VERTEX_ARRAY_OBJECT
		glGenVertexArrays(1, &this->array);
		glBindVertexArray(this->array);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_index_buffer);

		rBindBuffer(this->buffer);
		glEnableVertexAttribArray(Shader::COLOR);
		glEnableVertexAttribArray(Shader::TEXCOORD);
		glEnableVertexAttribArray(Shader::VERTEX);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	#else
		static_cast<void>(this->array);  // Silence -Wunused-private-field
	#endif
	}

	VertexArray::~VertexArray()
	{
	#ifdef USE_VERTEX_ARRAY_OBJECT
		glDeleteVertexArrays(1, &this->array);
	#endif
		glDeleteBuffers(1, &this->buffer);
	}

	void VertexArray::update(void *data, const size_t size) const
	{
		glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	#ifndef USE_VERTEX_ARRAY_OBJECT
		g_active_buffer = 0;
	#endif

		R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to update buffer");
	}
}
