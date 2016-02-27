// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Renderer.h"

#include <cstring>

#include "Graphics/Label.h"
#include "Graphics/SpriteBatch.h"

#define S0(i)  (i) * 4
#define S1(i)  S0(i), S0(i) + 1, S0(i) + 2, S0(i) + 2, S0(i) + 3, S0(i)

#define S4(i)       S1((i)),   S1((i) +   1),   S1((i) +   2),   S1((i) +   3)
#define S16(i)      S4((i)),   S4((i) +   4),   S4((i) +   8),   S4((i) +  12)
#define S64(i)     S16((i)),  S16((i) +  16),  S16((i) +  32),  S16((i) +  48)
#define S256(i)    S64((i)),  S64((i) +  64),  S64((i) + 128),  S64((i) + 192)
#define S1024(i)  S256((i)), S256((i) + 256), S256((i) + 512), S256((i) + 768)

void Renderer::clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

bool Renderer::has_extension(const char* extension)
{
    return strstr(reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)),
                  extension);
}

int Renderer::max_texture_size()
{
    static const int max_texture_size = [] {
        int max_texture_size;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
        return max_texture_size;
    }();
    return max_texture_size;
}

void Renderer::set_projection(const rainbow::Rect& projection)
{
    rect_ = projection;
    shader_manager_.update_projection();
}

void Renderer::set_resolution(const Vec2i& resolution)
{
    R_ASSERT(is_global(),
             "Cannot set resolution with an uninitialised renderer");

    view_ = resolution;
    rect_.right = rect_.left + resolution.x;
    rect_.top = rect_.bottom + resolution.y;
    shader_manager_.update_viewport();
    set_window_size(window_.is_zero() ? view_ : window_);

    R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to set resolution");
}

void Renderer::set_window_size(const Vec2i& new_size, float factor)
{
    R_ASSERT(is_global(),
             "Cannot set window size with an uninitialised renderer");

    scale_ = static_cast<float>(view_.x) / new_size.x;
    if (factor * new_size == view_)
    {
        zoom_ = 1.0f;
        origin_.x = 0;
        origin_.y = 0;
    }
    else
    {
        const Vec2i actual_size = factor * new_size;
        zoom_ = std::min(static_cast<float>(actual_size.x) / view_.x,
                         static_cast<float>(actual_size.y) / view_.y);
        origin_.x = (actual_size.x - view_.x * zoom_) * 0.5f;
        origin_.y = (actual_size.y - view_.y * zoom_) * 0.5f;
    }
    window_ = new_size;

    glViewport(origin_.x,
               origin_.y,
               new_size.x * factor - origin_.x * 2,
               new_size.y * factor - origin_.y * 2);
}

void Renderer::bind_element_array() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
}

Vec2i Renderer::convert_to_flipped_view(const Vec2i& p) const
{
    return convert_to_view(Vec2i(p.x, window_.y - p.y));
}

Vec2i Renderer::convert_to_screen(const Vec2i& p) const
{
    return Vec2i(origin_.x + (p.x - rect_.left) * zoom_,
                 origin_.y + (p.y - rect_.bottom) * zoom_);
}

Vec2i Renderer::convert_to_view(const Vec2i& p) const
{
    return Vec2i(rect_.left + p.x * scale_ - origin_.x / zoom_,
                 rect_.bottom + p.y * scale_ - origin_.y / zoom_);
}

void Renderer::reset() const
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    glActiveTexture(GL_TEXTURE0);
}

void Renderer::unbind_all()
{
    VertexArray::unbind();
    shader_manager_.use(ShaderManager::kInvalidProgram);
    texture_manager_.bind();
}

Renderer::Renderer()
    : index_buffer_(0), scale_(1.0f), zoom_(1.0f), shader_manager_(this) {}

Renderer::~Renderer()
{
    if (index_buffer_)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &index_buffer_);
    }
}

bool Renderer::init()
{
#ifdef __glew_h__
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        LOGF("Failed to initialise GLEW: %s", glewGetErrorString(err));
        return false;
    }
#endif

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (!shader_manager_.init())
        return false;

    const unsigned short kDefaultIndices[]{S256(0)};
    static_assert(sizeof(kDefaultIndices) ==
                      kNumSprites * 6 * sizeof(kDefaultIndices[0]),
                  "Number of indices do not match set number of sprites");
    glGenBuffers(1, &index_buffer_);
    bind_element_array();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kDefaultIndices),
                 kDefaultIndices, GL_STATIC_DRAW);

    make_global();
    return glGetError() == GL_NO_ERROR;
}
