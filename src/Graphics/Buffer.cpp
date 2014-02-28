// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Vec2.h"
#include "Graphics/Buffer.h"
#include "Graphics/ShaderDetails.h"
#include "Graphics/SpriteVertex.h"

/// Used by Label and SpriteBatch for interleaved vertex buffer.
template<>
void Buffer<SpriteVertex>::bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, this->id_);
	glVertexAttribPointer(
	    Shader::kAttributeColor, 4, GL_UNSIGNED_BYTE, GL_TRUE,
	    sizeof(SpriteVertex),
	    reinterpret_cast<void*>(offsetof(SpriteVertex, color)));
	glVertexAttribPointer(
	    Shader::kAttributeTexCoord, 2, GL_FLOAT, GL_FALSE,
	    sizeof(SpriteVertex),
	    reinterpret_cast<void*>(offsetof(SpriteVertex, texcoord)));
	glVertexAttribPointer(
	    Shader::kAttributeVertex, 2, GL_FLOAT, GL_TRUE,
	    sizeof(SpriteVertex),
	    reinterpret_cast<void*>(offsetof(SpriteVertex, position)));
}

/// Used by SpriteBatch for normal buffers.
template<>
void Buffer<Vec2f>::bind(const unsigned int index) const
{
	glBindBuffer(GL_ARRAY_BUFFER, this->id_);
	glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2f), nullptr);
	glEnableVertexAttribArray(index);
}
