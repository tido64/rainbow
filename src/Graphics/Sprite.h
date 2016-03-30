// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SPRITE_H_
#define GRAPHICS_SPRITE_H_

#include "Common/NonCopyable.h"
#include "Graphics/SpriteVertex.h"
#include "Memory/NotNull.h"

class Sprite;
class SpriteBatch;

class SpriteRef
{
public:
    SpriteRef() : batch_(nullptr), i_(0) {}

    SpriteRef(NotNull<const SpriteBatch*> batch, size_t i)
        : batch_(batch), i_(i) {}

    Sprite& operator*() const;
    Sprite* operator->() const;

    bool operator==(const SpriteRef& other) const
    {
        return batch_ == other.batch_ && i_ == other.i_;
    }

    explicit operator bool() const { return batch_; }

private:
    const SpriteBatch* batch_;
    size_t i_;

    friend SpriteBatch;
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

    Sprite(unsigned int width,
           unsigned int height,
           NotNull<const SpriteBatch*> parent);
    Sprite(Sprite&&);

    auto angle() const { return angle_; }
    auto color() const { return vertex_array_[0].color; }
    auto height() const { return height_; }
    auto id() const { return id_; }
    auto is_flipped() const -> bool;
    auto is_hidden() const -> bool;
    auto is_mirrored() const -> bool;
    auto parent() const -> const SpriteBatch& { return *parent_; }
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

    /// <summary>Sets buffer used to store UV coordinates.</summary>
    void set_normal_buffer(NotNull<Vec2f*> map) { normal_map_ = map; }

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

    /// <summary>Sets vertex array buffer.</summary>
    void set_vertex_array(NotNull<SpriteVertex*> array);

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
    auto update() -> bool;

    Sprite& operator=(Sprite&&);

private:
    unsigned int width_;          ///< Width of sprite (not scaled).
    unsigned int height_;         ///< Height of sprite (not scaled).
    unsigned int state_;          ///< State of internals, e.g. buffer.
    float angle_;                 ///< Angle of rotation.
    Vec2f pivot_;                 ///< Pivot point (normalised).
    Vec2f center_;                ///< Committed position.
    Vec2f position_;              ///< Uncommitted position.
    Vec2f scale_;                 ///< Scaling factor.
    const SpriteBatch* parent_;   ///< Pointer to sprite batch.
    SpriteVertex* vertex_array_;  ///< Interleaved vertex array.
    Vec2f* normal_map_;           ///< Normal map UV coordinates.
    int id_;                      ///< Sprite identifier.

    void flip_textures(unsigned int axis);
    void set_normal(unsigned int f, NotNull<const Vec2f*> uv);
};

class SpriteModel
{
public:
    SpriteModel(SpriteRef* ref = nullptr) : ref_(ref) {}

    auto angle() const { return angle_; }
    auto angle(float angle) -> SpriteModel&
    {
        angle_ = angle;
        return *this;
    }

    auto color() const { return color_; }
    auto color(Colorb color) -> SpriteModel&
    {
        color_ = color;
        return *this;
    }

    auto pivot() const { return pivot_; }
    auto pivot(const Vec2f& pivot) -> SpriteModel&
    {
        pivot_ = pivot;
        return *this;
    }

    auto position() const { return position_; }
    auto position(const Vec2f& position) -> SpriteModel&
    {
        position_ = position;
        return *this;
    }

    auto ref() const { return ref_; }

    auto scale() const { return scale_; }
    auto scale(const Vec2f& scale) -> SpriteModel&
    {
        scale_ = scale;
        return *this;
    }

    auto size() const { return Vec2i(width_, height_); }
    auto size(unsigned int width, unsigned int height) -> SpriteModel&
    {
        width_ = width;
        height_ = height;
        return *this;
    }

    auto texture() const { return texture_; }
    auto texture(unsigned int texture) -> SpriteModel&
    {
        texture_ = texture;
        return *this;
    }

private:
    unsigned int width_ = 0;
    unsigned int height_ = 0;
    float angle_ = 0.0f;
    Vec2f pivot_ = {0.5f, 0.5f};
    Vec2f position_;
    Vec2f scale_ = Vec2f::One;
    Colorb color_;
    unsigned int texture_ = 0;
    SpriteRef* ref_;
};

using SpriteList = std::initializer_list<SpriteModel>;

#endif
