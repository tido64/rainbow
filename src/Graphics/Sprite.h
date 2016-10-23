// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SPRITE_H_
#define GRAPHICS_SPRITE_H_

#include "Common/NonCopyable.h"
#include "Graphics/SpriteVertex.h"
#include "Memory/Array.h"

#define USE_SPRITE_FUNCTION_CHAINING 1

class Sprite;
class SpriteBatch;
class TextureAtlas;

class SpriteRef
{
public:
    SpriteRef() : batch_(nullptr), i_(0) {}
    SpriteRef(NotNull<SpriteBatch*> batch, uint32_t i) : batch_(batch), i_(i) {}

    auto batch() const -> const SpriteBatch* { return batch_; }
    auto index() const { return i_; }

    auto operator*() -> Sprite& { return get(); }
    auto operator*() const -> const Sprite& { return get(); }

    auto operator->() -> Sprite* { return &get(); }
    auto operator->() const -> const Sprite* { return &get(); }

    explicit operator bool() const { return batch_; }
    explicit operator uint32_t() const { return i_; }

    friend bool operator==(const SpriteRef& lhs, const SpriteRef& rhs)
    {
        return lhs.batch() == rhs.batch() && lhs.index() == rhs.index();
    }

private:
    SpriteBatch* batch_;
    uint32_t i_;

    auto get() const -> Sprite&;
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
///     The vertex order of the quad is 0,1,2 for the first triangle, and 2,3,0
///     for the second.
///   </para>
///   <para>
///     The sprite itself does not have a texture. It holds the texture
///     coordinates but it is the sprite batch that holds the actual texture.
///     That way, changing textures on a whole batch (i.e. skinning) can be
///     easily achieved.
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
    enum { kNoId };

    Sprite() : Sprite(0, 0) {}
    Sprite(unsigned int w, unsigned int h) : width_(w), height_(h) {}
    Sprite(Sprite&&) noexcept;

    auto angle() const { return angle_; }
    auto color() const { return color_; }
    auto height() const { return height_; }
    auto id() const { return id_; }
    auto is_flipped() const -> bool;
    auto is_hidden() const -> bool;
    auto is_mirrored() const -> bool;
    auto pivot() const { return pivot_; }
    auto position() const { return position_; }
    auto scale() const { return scale_; }
    auto vertex_array() const -> const SpriteVertex* { return vertex_array_; }
    auto width() const { return width_; }

    /// <summary>Sets sprite colour.</summary>
    void set_color(Colorb c);

    /// <summary>Sets the identifier for the sprite</summary>
    void set_id(int id) { id_ = id; }

    /// <summary>Sets normal map.</summary>
    /// <param name="id">Identifier of the normal map to set.</param>
    void set_normal(unsigned int id);

    /// <summary>Sets the pivot point for rotation and translation.</summary>
    /// <param name="p">Normalised pivot point.</param>
    void set_pivot(const Vec2f& p);

    /// <summary>Sets sprite position (absolute).</summary>
    void set_position(const Vec2f&);

    /// <summary>Sets angle of rotation (in radian).</summary>
    void set_rotation(float r);

    /// <summary>
    ///   Uniform scaling of sprite (does not affect actual width and height).
    /// </summary>
    /// <param name="f">Scaling factor for both axes.</param>
    void set_scale(float f) { set_scale(Vec2f(f, f)); }

    /// <summary>
    ///   Non-uniform scaling of sprite (does not affect actual width and
    ///   height).
    /// </summary>
    /// <param name="f">Scaling factors for x- and y-axis.</param>
    void set_scale(const Vec2f& f);

    /// <summary>Sets texture.</summary>
    /// <param name="id">Identifier of the texture to set.</param>
    void set_texture(unsigned int id);

    /// <summary>Flips sprite vertically.</summary>
    void flip();

    /// <summary>Hides sprite if it is currently shown.</summary>
    void hide();

    /// <summary>Mirrors sprite.</summary>
    void mirror();

    /// <summary>Moves sprite by (x,y).</summary>
    void move(const Vec2f&);

    /// <summary>Rotates sprite by <paramref name="r"/>.</summary>
    void rotate(float r);

    /// <summary>Shows sprite if it is currently hidden.</summary>
    void show();

    /// <summary>Updates the vertex buffer.</summary>
    /// <returns>
    ///   <c>true</c> if the buffer has changed; <c>false</c> otherwise.
    /// </returns>
    auto update(const ArraySpan<SpriteVertex>& vertex_array,
                const TextureAtlas& texture) -> bool;

    /// <summary>Updates the normal buffer.</summary>
    /// <returns>
    ///   <c>true</c> if the buffer has changed; <c>false</c> otherwise.
    /// </returns>
    auto update(const ArraySpan<Vec2f>& normal_array,
                const TextureAtlas& normal) -> bool;

    auto operator=(Sprite&&) -> Sprite&;

#if USE_SPRITE_FUNCTION_CHAINING
    auto angle(float angle) -> Sprite&
    {
        set_rotation(angle);
        return *this;
    }

    auto color(Colorb c) -> Sprite&
    {
        set_color(c);
        return *this;
    }

    auto id(int id) -> Sprite&
    {
        set_id(id);
        return *this;
    }

    auto normal(unsigned int id) -> Sprite&
    {
        set_normal(id);
        return *this;
    }

    auto pivot(const Vec2f& pivot) -> Sprite&
    {
        set_pivot(pivot);
        return *this;
    }

    auto position(const Vec2f& pos) -> Sprite&
    {
        set_position(pos);
        return *this;
    }

    auto scale(const Vec2f& scale) -> Sprite&
    {
        set_scale(scale);
        return *this;
    }

    auto texture(unsigned int id) -> Sprite&
    {
        set_texture(id);
        return *this;
    }
#endif  // USE_SPRITE_FUNCTION_CHAINING

#ifdef RAINBOW_TEST
    auto state() const { return state_; }
#endif

private:
    unsigned int state_ = 1;                ///< State of internals, e.g. buffer.
    Vec2f center_;                          ///< Committed position.
    Vec2f position_;                        ///< Uncommitted position.
    unsigned int texture_ = 0;
    Colorb color_;
    unsigned int width_ = 0;                ///< Width of sprite (not scaled).
    unsigned int height_ = 0;               ///< Height of sprite (not scaled).
    float angle_ = 0.0f;                    ///< Angle of rotation.
    Vec2f pivot_ = {0.5f, 0.5f};            ///< Pivot point (normalised).
    Vec2f scale_ = Vec2f::One;              ///< Scaling factor.
    unsigned int normal_map_ = 0;
    int id_ = kNoId;                        ///< Sprite identifier.
    SpriteVertex* vertex_array_ = nullptr;  ///< Interleaved vertex array.
};

#undef USE_SPRITE_FUNCTION_CHAINING
#endif
