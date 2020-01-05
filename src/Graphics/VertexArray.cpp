// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/VertexArray.h"

#include "Graphics/Renderer.h"

using rainbow::graphics::VertexArray;

void VertexArray::unbind()
{
#ifdef USE_VERTEX_ARRAY_OBJECT
    glBindVertexArray(0);
#else
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
}

VertexArray::VertexArray(VertexArray&& va) noexcept : array_(va.array_)
{
#ifdef USE_VERTEX_ARRAY_OBJECT
    va.array_ = 0;
#endif
}

VertexArray::~VertexArray()
{
#ifdef USE_VERTEX_ARRAY_OBJECT
    if (array_ == 0)
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

auto VertexArray::init_state() const -> GLuint
{
#ifdef USE_VERTEX_ARRAY_OBJECT
    GLuint array;
    glGenVertexArrays(1, &array);
    glBindVertexArray(array);
    graphics::bind_element_array();
    return array;
#else
    return 0;
#endif
}

void rainbow::graphics::draw(const VertexArray& array, uint32_t count)
{
    array.bind();
    glDrawElements(
        GL_TRIANGLES, narrow_cast<GLsizei>(count), GL_UNSIGNED_SHORT, nullptr);

    IF_DEBUG(increment_draw_count());
}

void rainbow::graphics::draw(const VertexArray& array,
                             uint32_t first,
                             uint32_t count)
{
    array.bind();
    glDrawArrays(
        GL_TRIANGLES, narrow_cast<GLint>(first), narrow_cast<GLsizei>(count));

    IF_DEBUG(increment_draw_count());
}
