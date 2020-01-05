// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_RENDERER_H_
#define GRAPHICS_RENDERER_H_

#include <system_error>

#include "Graphics/ElementBuffer.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/Texture.h"
#include "Graphics/TextureAllocator.gl.h"
#include "Graphics/VertexArray.h"
#include "Math/Geometry.h"

namespace rainbow::graphics
{
    /// <summary>Hard-coded limit on number of sprites.</summary>
    static constexpr size_t kMaxSprites = 4096;

    struct Context
    {
        float scale = 1.0F;
        float zoom = 1.0F;
        Vec2i origin;
        Vec2i surface_size;
        Vec2i window_size;
        Rect projection;
        ElementBuffer element_buffer;
        gl::TextureAllocator texture_allocator;
        TextureProvider texture_provider{texture_allocator};
        ShaderManager shader_manager{*this, Passkey<Context>{}};

        ~Context();

        auto initialize() -> std::error_code;
    };

    struct MemoryInfo
    {
        int current_available;
        int total_available;
    };

    auto draw_count() -> unsigned int;
    auto gl_version() -> czstring;
    auto max_texture_size() -> int;
    auto memory_info() -> MemoryInfo;
    auto renderer() -> czstring;
    auto vendor() -> czstring;

    void set_projection(Context&, const Rect&);
    void set_surface_size(Context&, const Vec2i& resolution);
    void set_window_size(Context&, const Vec2i& size, float factor = 1.0F);

    void bind_element_array();

    void clear();

    auto convert_to_flipped_view(const Context&, const Vec2i&) -> Vec2i;
    auto convert_to_screen(const Context&, const Vec2i&) -> Vec2i;
    auto convert_to_view(const Context&, const Vec2i&) -> Vec2i;

    void increment_draw_count();

    template <typename T>
    void draw_arrays(const T& obj, int first, size_t count)
    {
        obj.bind_textures();
        draw(obj.vertex_array(), first, count);
    }

    void reset();

    void scissor(const Context&, int x, int y, int width, int height);

    class ScopedProjection
    {
    public:
        ScopedProjection(Context& ctx, const Rect& new_projection)
            : context_(ctx), projection_(ctx.projection)
        {
            set_projection(context_, new_projection);
        }

        ~ScopedProjection() { set_projection(context_, projection_); }

    private:
        Context& context_;
        const Rect projection_;
    };

    template <int GL_STATE>
    struct ScopedState
    {
        ScopedState() { glEnable(GL_STATE); }
        ~ScopedState() { glDisable(GL_STATE); }
    };

    using ScopedCullFace = ScopedState<GL_CULL_FACE>;
    using ScopedDepthTest = ScopedState<GL_DEPTH_TEST>;
    using ScopedScissorTest = ScopedState<GL_SCISSOR_TEST>;
}  // namespace rainbow::graphics

#endif
