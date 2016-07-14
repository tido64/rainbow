// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Renderer.h"

#include <cstring>

#include "Graphics/Label.h"
#include "Graphics/SpriteBatch.h"

// clang-format off
#define S0(i)  (i) * 4
#define S1(i)  S0(i), S0(i) + 1, S0(i) + 2, S0(i) + 2, S0(i) + 3, S0(i)

#define S4(i)        S1((i)),    S1((i) +    1),    S1((i) +    2),    S1((i) +    3)
#define S16(i)       S4((i)),    S4((i) +    4),    S4((i) +    8),    S4((i) +   12)
#define S64(i)      S16((i)),   S16((i) +   16),   S16((i) +   32),   S16((i) +   48)
#define S256(i)     S64((i)),   S64((i) +   64),   S64((i) +  128),   S64((i) +  192)
#define S1024(i)   S256((i)),  S256((i) +  256),  S256((i) +  512),  S256((i) +  768)
#define S4096(i)  S1024((i)), S1024((i) + 1024), S1024((i) + 2048), S1024((i) + 3072)
// clang-format on

using rainbow::Rect;
using rainbow::graphics::State;

namespace graphics = rainbow::graphics;

namespace
{
    unsigned int g_draw_count = 0;
    State* g_state = nullptr;

    auto gl_get_string(GLenum name)
    {
        return reinterpret_cast<const char*>(glGetString(name));
    }
}

namespace rainbow { namespace graphics { namespace detail
{
#ifndef NDEBUG
    unsigned int g_draw_count_accumulator = 0;
#endif  // NDEBUG
}}}

auto graphics::draw_count() -> unsigned int
{
    return g_draw_count;
}

auto graphics::gl_version() -> const char*
{
    return gl_get_string(GL_VERSION);
}

auto graphics::max_texture_size() -> int
{
    static const int max_texture_size = [] {
        GLint max_texture_size;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
        return max_texture_size;
    }();
    return max_texture_size;
}

auto graphics::memory_info() -> graphics::MemoryInfo
{
    static const GLenum pname = [] {
        if (has_extension("GL_NVX_gpu_memory_info"))
            return GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX;
        if (has_extension("GL_ATI_meminfo"))
            return GL_TEXTURE_FREE_MEMORY_ATI;
        return 0;
    }();

    MemoryInfo meminfo;
    switch (pname)
    {
        case GL_TEXTURE_FREE_MEMORY_ATI: {
            GLint info[4];
            glGetIntegerv(pname, info);
            meminfo.current_available = info[0];
            meminfo.total_available = 0;
            break;
        }
        case GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX:
            glGetIntegerv(pname, &meminfo.current_available);
            glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX,
                          &meminfo.total_available);
            break;
        default:
            meminfo.current_available = 0;
            meminfo.total_available = 0;
            break;
    }
    return meminfo;
}

auto graphics::projection() -> const Rect&
{
    return g_state->rect;
}

auto graphics::renderer() -> const char*
{
    return gl_get_string(GL_RENDERER);
}

auto graphics::resolution() -> const Vec2i&
{
    return g_state->view;
}

auto graphics::vendor() -> const char*
{
    return gl_get_string(GL_VENDOR);
}

auto graphics::window_size() -> const Vec2i&
{
    return g_state->window;
}

void graphics::set_projection(const Rect& projection)
{
    R_ASSERT(g_state != nullptr,
             "Cannot set projection with an uninitialised renderer");

    g_state->rect = projection;
    g_state->shader_manager.update_projection();
}

void graphics::set_resolution(const Vec2i& resolution)
{
    R_ASSERT(g_state != nullptr,
             "Cannot set resolution with an uninitialised renderer");

    g_state->view = resolution;
    g_state->rect.right = g_state->rect.left + resolution.x;
    g_state->rect.top = g_state->rect.bottom + resolution.y;
    g_state->shader_manager.update_viewport();
    set_window_size(g_state->window.is_zero() ? g_state->view
                                              : g_state->window);

    R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to set resolution");
}

void graphics::set_window_size(const Vec2i& new_size, float factor)
{
    R_ASSERT(g_state != nullptr,
             "Cannot set window size with an uninitialised renderer");

    g_state->scale = static_cast<float>(g_state->view.x) / new_size.x;
    if (factor * new_size == g_state->view)
    {
        g_state->zoom = 1.0f;
        g_state->origin.x = 0;
        g_state->origin.y = 0;
    }
    else
    {
        const Vec2i actual_size = factor * new_size;
        g_state->zoom =
            std::min(static_cast<float>(actual_size.x) / g_state->view.x,
                     static_cast<float>(actual_size.y) / g_state->view.y);
        g_state->origin.x =
            (actual_size.x - g_state->view.x * g_state->zoom) * 0.5f;
        g_state->origin.y =
            (actual_size.y - g_state->view.y * g_state->zoom) * 0.5f;
    }
    g_state->window = new_size;

    glViewport(g_state->origin.x,
               g_state->origin.y,
               new_size.x * factor - g_state->origin.x * 2,
               new_size.y * factor - g_state->origin.y * 2);
}

void graphics::bind_element_array()
{
    g_state->element_buffer.bind();
}

void graphics::clear()
{
    glClear(GL_COLOR_BUFFER_BIT);

#ifndef NDEBUG
    g_draw_count = detail::g_draw_count_accumulator;
    detail::g_draw_count_accumulator = 0;
#endif
}

auto graphics::convert_to_flipped_view(const Vec2i& p) -> Vec2i
{
    return convert_to_view(Vec2i(p.x, g_state->window.y - p.y));
}

auto graphics::convert_to_screen(const Vec2i& p) -> Vec2i
{
    return Vec2i(
        g_state->origin.x + (p.x - g_state->rect.left) * g_state->zoom,
        g_state->origin.y + (p.y - g_state->rect.bottom) * g_state->zoom);
}

auto graphics::convert_to_view(const Vec2i& p) -> Vec2i
{
    return Vec2i(g_state->rect.left + p.x * g_state->scale -
                     g_state->origin.x / g_state->zoom,
                 g_state->rect.bottom + p.y * g_state->scale -
                     g_state->origin.y / g_state->zoom);
}

bool graphics::has_extension(const char* extension)
{
    static auto gl_extensions = gl_get_string(GL_EXTENSIONS);
    return strstr(gl_extensions, extension);
}

void graphics::reset()
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    glActiveTexture(GL_TEXTURE0);
}

void graphics::scissor(int x, int y, int width, int height)
{
    glScissor(g_state->origin.x + x, g_state->origin.y + y, width, height);
}

void graphics::unbind_all()
{
    VertexArray::unbind();
    g_state->shader_manager.use(ShaderManager::kInvalidProgram);
    g_state->texture_manager.bind();
}

State::~State()
{
    if (this == g_state)
        g_state = nullptr;
}

bool State::initialize()
{
    R_ASSERT(g_state == nullptr, "Renderer is already initialised");

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

    if (!shader_manager.init())
        return false;

    const unsigned short kDefaultIndices[]{S4096(0)};
    static_assert(
        sizeof(kDefaultIndices) == kMaxSprites * 6 * sizeof(kDefaultIndices[0]),
        "Number of indices do not match set number of sprites");

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    element_buffer = buffer;
    element_buffer.upload(kDefaultIndices, sizeof(kDefaultIndices));

    const bool success = glGetError() == GL_NO_ERROR;
    if (success)
        g_state = this;

    return success;
}
