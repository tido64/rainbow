/// Copyright 2013 Bifrost Entertainment. All rights reserved.

#include "Graphics/ShaderManager.h"
#include "Graphics/SpriteVertex.h"
#include "Graphics/VertexArray.h"

#ifndef USE_VERTEX_ARRAY_OBJECTS
#	ifndef RAINBOW_OS_ANDROID
#		define USE_VERTEX_ARRAY_OBJECTS 1
#	endif
#endif

namespace Renderer
{
	extern unsigned int g_index_buffer;

	namespace
	{
		void rbBindBuffer(const unsigned int buffer)
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

	BindVertexArray::BindVertexArray(const VertexArray &array)
	{
	#ifdef USE_VERTEX_ARRAY_OBJECTS
		glBindVertexArray(array.array);
	#else
		rbBindBuffer(array.buffer);
	#endif
	}

	BindVertexArray::~BindVertexArray()
	{
	#ifdef USE_VERTEX_ARRAY_OBJECTS
		glBindVertexArray(0);
	#else
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	#endif
	}

	VertexArray::VertexArray() : count(0), array(0), buffer(0)
	{
		glGenBuffers(1, &this->buffer);

	#ifdef USE_VERTEX_ARRAY_OBJECTS

		glGenVertexArrays(1, &this->array);
		glBindVertexArray(this->array);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_index_buffer);

		rbBindBuffer(this->buffer);
		glEnableVertexAttribArray(Shader::COLOR);
		glEnableVertexAttribArray(Shader::TEXCOORD);
		glEnableVertexAttribArray(Shader::VERTEX);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	#endif
	}

	VertexArray::~VertexArray()
	{
	#ifdef USE_VERTEX_ARRAY_OBJECTS
		glDeleteVertexArrays(1, &this->array);
	#endif
		glDeleteBuffers(1, &this->buffer);
	}

	void VertexArray::update(void *data, const size_t size) const
	{
		glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to update buffer");
	}
}
