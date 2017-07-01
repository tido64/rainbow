// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Buffer.h"

#include "Graphics/OpenGL.h"
#include "Graphics/ShaderDetails.h"
#include "Graphics/SpriteVertex.h"

using rainbow::graphics::Buffer;

#define s_offsetof(type, field)                                                \
    reinterpret_cast<const void*>(offsetof(type, field)))  // NOLINT

namespace
{
    auto glGenBuffer()
    {
        unsigned int id;
        glGenBuffers(1, &id);
        return id;
    }
}

Buffer::Buffer() : id_(glGenBuffer())
{
}

Buffer::Buffer(Buffer&& buffer) noexcept : id_(buffer.id_)
{
    buffer.id_ = 0;
}

Buffer::~Buffer()
{
    if (id_ == 0)
        return;

    glDeleteBuffers(1, &id_);
}

void Buffer::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, id_);
    glEnableVertexAttribArray(Shader::kAttributeColor);
    glVertexAttribPointer(
        Shader::kAttributeColor,
        4,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        sizeof(SpriteVertex),
        s_offsetof(SpriteVertex, color);
    glEnableVertexAttribArray(Shader::kAttributeTexCoord);
    glVertexAttribPointer(
        Shader::kAttributeTexCoord,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(SpriteVertex),
        s_offsetof(SpriteVertex, texcoord);
    glEnableVertexAttribArray(Shader::kAttributeVertex);
    glVertexAttribPointer(
        Shader::kAttributeVertex,
        2,
        GL_FLOAT,
        GL_TRUE,
        sizeof(SpriteVertex),
        s_offsetof(SpriteVertex, position);
}

void Buffer::bind(unsigned int index) const
{
    glBindBuffer(GL_ARRAY_BUFFER, id_);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2f), nullptr);
}

void Buffer::upload(const void* data, size_t size) const
{
    glBindBuffer(GL_ARRAY_BUFFER, id_);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STREAM_DRAW);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
