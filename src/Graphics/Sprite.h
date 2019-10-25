// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SPRITE_H_
#define GRAPHICS_SPRITE_H_

#include "Common/NonCopyable.h"
#include "Graphics/SpriteVertex.h"
#include "Math/Geometry.h"
#include "Memory/Array.h"

namespace rainbow::graphics
{
    struct TextureData;
}  // namespace rainbow::graphics

namespace rainbow
{
    class Sprite;
    class SpriteBatch;

    class SpriteRef
    {
    public:
        SpriteRef() = default;
        SpriteRef(SpriteBatch& batch, uint32_t i) : batch_(&batch), i_(i) {}

        [[nodiscard]] auto batch() const -> const SpriteBatch*
        {
            return batch_;
        }

        [[nodiscard]] auto index() const { return i_; }

        auto operator*() -> Sprite& { return get(); }
        auto operator*() const -> const Sprite& { return get(); }

        auto operator->() -> Sprite* { return &get(); }
        auto operator->() const -> const Sprite* { return &get(); }

        explicit operator bool() const { return batch_ != nullptr; }
        explicit operator uint32_t() const { return i_; }

        friend auto operator==(const SpriteRef& lhs, const SpriteRef& rhs)
        {
            return lhs.batch() == rhs.batch() && lhs.index() == rhs.index();
        }

    private:
        SpriteBatch* batch_ = nullptr;
        uint32_t i_ = 0;

        [[nodiscard]] auto get() const -> Sprite&;
    };

    /// <summary>A textured quad.</summary>
    /// <remarks>
    ///   <code>
    ///     3 ┌─────┐ 2
    ///       │     │
    ///       │     │
    ///     0 └─────┘ 1
    ///   </code>
    ///   <para>
    ///     The vertex order of the quad is 0,1,2 for the first triangle, and
    ///     2,3,0 for the second. Textures, on the other hand, are read into
    ///     memory upside-down. The order of the UV coordinates must be flipped
    ///     vertically, giving us 3,2,1 and 1,0,3.
    ///   </para>
    ///   <para>
    ///     The sprite itself does not have a texture. It holds the texture
    ///     coordinates but it is the sprite batch that holds the actual
    ///     texture. That way, changing textures on a whole batch (i.e.
    ///     skinning) can be easily achieved.
    ///   </para>
    ///   <list type="bullet">
    ///     <item>http://developer.apple.com/library/ios/#documentation/3DDrawing/Conceptual/OpenGLES_ProgrammingGuide/TechniquesforWorkingwithVertexData/TechniquesforWorkingwithVertexData.html</item>
    ///     <item>http://iphonedevelopment.blogspot.com/2009/06/opengl-es-from-ground-up-part-8.html</item>
    ///     <item>http://mathforum.org/mathimages/index.php/Transformation_Matrix</item>
    ///   </list>
    /// </remarks>
    class Sprite : private NonCopyable<Sprite>
    {
    public:
        enum
        {
            kNoId
        };

        Sprite() = default;

        Sprite(uint32_t width, uint32_t height) : width_(width), height_(height)
        {
        }

        Sprite(Sprite&&) noexcept;

        [[nodiscard]] auto angle() const { return angle_; }
        [[nodiscard]] auto color() const { return color_; }
        [[nodiscard]] auto height() const { return height_; }
        [[nodiscard]] auto id() const { return id_; }
        [[nodiscard]] auto is_flipped() const -> bool;
        [[nodiscard]] auto is_hidden() const -> bool;
        [[nodiscard]] auto is_mirrored() const -> bool;
        [[nodiscard]] auto pivot() const { return pivot_; }
        [[nodiscard]] auto position() const { return position_; }
        [[nodiscard]] auto scale() const { return scale_; }
        [[nodiscard]] auto width() const { return width_; }

        /// <summary>Sets angle of rotation (in radian).</summary>
        auto angle(float r) -> Sprite&;

        /// <summary>Sets sprite colour.</summary>
        auto color(Color c) -> Sprite&;

        /// <summary>Flips sprite vertically.</summary>
        auto flip() -> Sprite&;

        /// <summary>Hides sprite if it is currently shown.</summary>
        auto hide() -> Sprite&;

        /// <summary>Sets the identifier for the sprite</summary>
        auto id(int id) -> Sprite&
        {
            id_ = id;
            return *this;
        }

        /// <summary>Mirrors sprite.</summary>
        auto mirror() -> Sprite&;

        /// <summary>Moves sprite by (x,y).</summary>
        auto move(Vec2f) -> Sprite&;

        /// <summary>Sets the area of the normal texture area to map.</summary>
        /// <param name="area">Area of the normal texture.</param>
        auto normal(const Rect& area) -> Sprite&;

        /// <summary>Sets pivot point for rotation and translation.</summary>
        /// <param name="p">Normalised pivot point.</param>
        auto pivot(Vec2f pivot) -> Sprite&;

        /// <summary>Sets sprite position (absolute).</summary>
        auto position(Vec2f position) -> Sprite&;

        /// <summary>Rotates sprite by <paramref name="r"/>.</summary>
        auto rotate(float r) -> Sprite&;

        /// <summary>
        ///   Uniform scaling of sprite (does not affect actual width and
        ///   height).
        /// </summary>
        /// <param name="f">Scaling factor for both axes.</param>
        auto scale(float f) -> Sprite& { return scale({f, f}); }

        /// <summary>
        ///   Non-uniform scaling of sprite (does not affect actual width and
        ///   height).
        /// </summary>
        /// <param name="f">Scaling factors for x- and y-axis.</param>
        auto scale(Vec2f f) -> Sprite&;

        /// <summary>Shows sprite if it is currently hidden.</summary>
        auto show() -> Sprite&;

        /// <summary>Sets the area of the texture area to map.</summary>
        /// <param name="area">Area of the texture.</param>
        auto texture(const Rect& area) -> Sprite&;

        /// <summary>Updates the vertex buffer.</summary>
        /// <returns>
        ///   <c>true</c> if the buffer has changed; <c>false</c> otherwise.
        /// </returns>
        auto update(ArraySpan<SpriteVertex> vertex_array,
                    const graphics::TextureData&) -> bool;

        /// <summary>Updates the normal buffer.</summary>
        /// <returns>
        ///   <c>true</c> if the buffer has changed; <c>false</c> otherwise.
        /// </returns>
        auto update(ArraySpan<Vec2f> normal_array, const graphics::TextureData&)
            -> bool;

        auto operator=(Sprite&&) noexcept -> Sprite&;

#ifdef RAINBOW_TEST
        [[nodiscard]] auto state() const { return state_; }
#endif

    private:
        /// <summary>State of internals, e.g. buffer.</summary>
        uint32_t state_ = 1;

        /// <summary>Committed position.</summary>
        Vec2f center_;

        /// <summary>Uncommitted position.</summary>
        Vec2f position_;

        /// <summary>Area of the texture to map.</summary>
        Rect texture_area_;

        /// <summary>Tint color.</summary>
        Color color_;

        /// <summary>Width of sprite (unscaled).</summary>
        uint32_t width_ = 0;

        /// <summary>Height of sprite (unscaled).</summary>
        uint32_t height_ = 0;

        /// <summary>Angle of rotation.</summary>
        float angle_ = 0.0F;

        /// <summary>Pivot point (normalised).</summary>
        Vec2f pivot_ = {0.5F, 0.5F};

        /// <summary>Scaling factor.</summary>
        Vec2f scale_ = Vec2f::One;

        /// <summary>Area of the normal texture to map.</summary>
        Rect normal_map_;

        /// <summary>User defined identifier.</summary>
        int id_ = kNoId;
    };
}  // namespace rainbow

#endif
