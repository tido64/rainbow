// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/ElementBuffer.h"

#include "Graphics/OpenGL.h"

using rainbow::graphics::ElementBuffer;

ElementBuffer::~ElementBuffer()
{
    if (buffer_ == 0)
        return;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &buffer_);
}

void ElementBuffer::bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_);
}

void ElementBuffer::upload(const void* data, size_t size) const
{
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}
