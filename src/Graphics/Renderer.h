// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_RENDERER_H_
#define GRAPHICS_RENDERER_H_

#include "Graphics/ElementBuffer.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/TextureManager.h"
#include "Math/Geometry.h"

namespace rainbow::graphics
{
    namespace detail
    {
#ifndef NDEBUG
        extern unsigned int g_draw_count_accumulator;
#endif
    }  // namespace detail

    /// <summary>Hard-coded limit on number of sprites.</summary>
    static constexpr size_t kMaxSprites = 4096;

    struct MemoryInfo
    {
        int current_available;
        int total_available;
    };

    auto draw_count() -> unsigned int;
    auto gl_version() -> czstring;
    auto max_texture_size() -> int;
    auto memory_info() -> MemoryInfo;
    auto projection() -> const Rect&;
    auto renderer() -> czstring;
    auto resolution() -> const Vec2i&;
    auto vendor() -> czstring;
    auto window_size() -> const Vec2i&;

    void set_projection(const Rect&);
    void set_resolution(const Vec2i& resolution);
    void set_window_size(const Vec2i& size, float factor = 1.0f);

    void bind_element_array();

    void clear();

    auto convert_to_flipped_view(const Vec2i&) -> Vec2i;
    auto convert_to_screen(const Vec2i&) -> Vec2i;
    auto convert_to_view(const Vec2i&) -> Vec2i;

    template <typename T>
    void draw(const T& obj)
    {
        obj.vertex_array().bind();
        obj.bind_textures();
        glDrawElements(
            GL_TRIANGLES, obj.vertex_count(), GL_UNSIGNED_SHORT, nullptr);

        IF_DEBUG(++detail::g_draw_count_accumulator);
    }

    template <typename T>
    void draw_arrays(const T& obj, int first, size_t count)
    {
        obj.vertex_array().bind();
        obj.bind_textures();
        glDrawArrays(GL_TRIANGLES, first, count);

        IF_DEBUG(++detail::g_draw_count_accumulator);
    }

    bool has_extension(czstring extension);

    void reset();

    void scissor(int x, int y, int width, int height);

    void unbind_all();

    class ScopedProjection
    {
    public:
        ScopedProjection(const Rect& new_projection) : projection_(projection())
        {
            set_projection(new_projection);
        }

        ~ScopedProjection() { set_projection(projection_); }

    private:
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

    struct State
    {
        float scale = 1.0f;
        float zoom = 1.0f;
        Vec2i origin;
        Vec2i view;
        Vec2i window;
        Rect rect;
        ElementBuffer element_buffer;
        TextureManager texture_manager;
        ShaderManager shader_manager;

        State()
            : texture_manager(Passkey<State>{}),
              shader_manager(Passkey<State>{})
        {
        }

        ~State();

        bool initialize();
    };
}  // namespace rainbow::graphics

#endif
