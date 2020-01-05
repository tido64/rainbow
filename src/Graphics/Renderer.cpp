// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Renderer.h"

#include <string_view>

#include "Common/Error.h"
#include "Graphics/VertexArray.h"

using namespace std::literals::string_view_literals;

using rainbow::czstring;
using rainbow::Rect;
using rainbow::Vec2i;
using rainbow::graphics::Context;

namespace graphics = rainbow::graphics;

namespace
{
    unsigned int g_draw_count = 0;
    Context* g_context = nullptr;

    auto gl_get_string(GLenum name)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        return reinterpret_cast<czstring>(glGetString(name));
    }
}  // namespace

#ifndef NDEBUG
namespace rainbow::graphics::detail
{
    unsigned int g_draw_count_accumulator = 0;
}  // namespace rainbow::graphics::detail
#endif  // NDEBUG

auto graphics::draw_count() -> unsigned int
{
    return g_draw_count;
}

auto graphics::gl_version() -> czstring
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
        std::string_view ext(gl_get_string(GL_EXTENSIONS));
        if (ext.find("GL_NVX_gpu_memory_info"sv) != std::string_view::npos)
            return GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX;
        if (ext.find("GL_ATI_meminfo"sv) != std::string_view::npos)
            return GL_TEXTURE_FREE_MEMORY_ATI;
        return 0;
    }();

    MemoryInfo meminfo{};
    switch (pname)
    {
        case GL_TEXTURE_FREE_MEMORY_ATI:
        {
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
            break;
    }
    return meminfo;
}

auto graphics::renderer() -> czstring
{
    return gl_get_string(GL_RENDERER);
}

auto graphics::vendor() -> czstring
{
    return gl_get_string(GL_VENDOR);
}

void graphics::set_projection(Context& ctx, const Rect& projection)
{
    ctx.projection = projection;
    ctx.shader_manager.update_projection();
}

void graphics::set_surface_size(Context& ctx, const Vec2i& resolution)
{
    ctx.surface_size = resolution;
    ctx.projection.width = resolution.x;
    ctx.projection.height = resolution.y;
    ctx.shader_manager.update_viewport();
    set_window_size(
        ctx, ctx.window_size.is_zero() ? ctx.surface_size : ctx.window_size);

    R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to set resolution");
}

void graphics::set_window_size(Context& ctx, const Vec2i& size, float factor)
{
    ctx.scale = static_cast<float>(ctx.surface_size.x) / size.x;
    if (factor * size == ctx.surface_size)
    {
        ctx.zoom = 1.0f;
        ctx.origin.x = 0;
        ctx.origin.y = 0;
    }
    else
    {
        const Vec2i actual_size = factor * size;
        ctx.zoom =
            std::min(static_cast<float>(actual_size.x) / ctx.surface_size.x,
                     static_cast<float>(actual_size.y) / ctx.surface_size.y);
        ctx.origin.x = (actual_size.x - ctx.surface_size.x * ctx.zoom) * 0.5f;
        ctx.origin.y = (actual_size.y - ctx.surface_size.y * ctx.zoom) * 0.5f;
    }
    ctx.window_size = size;

    glViewport(ctx.origin.x,
               ctx.origin.y,
               size.x * factor - ctx.origin.x * 2,
               size.y * factor - ctx.origin.y * 2);
}

void graphics::bind_element_array()
{
    g_context->element_buffer.bind();
}

void graphics::clear()
{
    glClear(GL_COLOR_BUFFER_BIT);

#ifndef NDEBUG
    g_draw_count = detail::g_draw_count_accumulator;
    detail::g_draw_count_accumulator = 0;
#endif
}

auto graphics::convert_to_flipped_view(const Context& ctx, const Vec2i& p)
    -> Vec2i
{
    return convert_to_view(ctx, {p.x, ctx.window_size.y - p.y});
}

auto graphics::convert_to_screen(const Context& ctx, const Vec2i& p) -> Vec2i
{
    return Vec2i(ctx.origin.x + (p.x - ctx.projection.left) * ctx.zoom,
                 ctx.origin.y + (p.y - ctx.projection.bottom) * ctx.zoom);
}

auto graphics::convert_to_view(const Context& ctx, const Vec2i& p) -> Vec2i
{
    return Vec2i(
        ctx.projection.left + p.x * ctx.scale - ctx.origin.x / ctx.zoom,
        ctx.projection.bottom + p.y * ctx.scale - ctx.origin.y / ctx.zoom);
}

#ifndef NDEBUG
void graphics::increment_draw_count()
{
    ++detail::g_draw_count_accumulator;
}
#endif  // NDEBUG

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

void graphics::scissor(const Context& ctx, int x, int y, int width, int height)
{
    glScissor(ctx.origin.x + x, ctx.origin.y + y, width, height);
}

Context::~Context()
{
    if (this == g_context)
        g_context = nullptr;
}

auto Context::initialize() -> std::error_code
{
    R_ASSERT(g_context == nullptr, "Renderer is already initialised");

#ifdef __glad_h_
    if (!gladLoadGL())
        return ErrorCode::GLInitializationFailed;
#endif

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (!shader_manager.init())
        return ErrorCode::ShaderManagerInitializationFailed;

    constexpr size_t kElementBufferSize = kMaxSprites * 6;
    auto default_indices = std::make_unique<uint16_t[]>(kElementBufferSize);
    for (size_t i = 0; i < kMaxSprites; ++i)
    {
        const auto index = i * 6;
        const auto vertex = static_cast<uint16_t>(i * 4);
        default_indices[index] = vertex;
        default_indices[index + 1] = vertex + 1;
        default_indices[index + 2] = vertex + 2;
        default_indices[index + 3] = vertex + 2;
        default_indices[index + 4] = vertex + 3;
        default_indices[index + 5] = vertex;
    }

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    element_buffer = buffer;
    element_buffer.upload(default_indices.get(), kElementBufferSize);

    if (glGetError() != GL_NO_ERROR)
        return ErrorCode::RenderInitializationFailed;

    g_context = this;
    return ErrorCode::Success;
}
