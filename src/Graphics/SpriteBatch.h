// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SPRITEBATCH_H_
#define GRAPHICS_SPRITEBATCH_H_

#include <type_traits>

#include "Graphics/Buffer.h"
#include "Graphics/Sprite.h"
#include "Graphics/Texture.h"
#include "Graphics/VertexArray.h"
#include "Memory/StableArray.h"

namespace rainbow
{
    class GameBase;
    struct ISolemnlySwearThatIAmOnlyTesting;

    /// <summary>A drawable batch of sprites.</summary>
    /// <remarks>
    ///   All sprites share a common vertex buffer object (at different offsets)
    ///   and are drawn with a single glDraw call. The sprites must use the same
    ///   texture atlas.
    /// </remarks>
    class SpriteBatch : private NonCopyable<SpriteBatch>
    {
    public:
        /// <summary>Creates a batch of sprites.</summary>
        /// <param name="count">Number of sprites to allocate for.</param>
        SpriteBatch(uint32_t count);

        template <typename... Args>
        SpriteBatch(const graphics::Texture& texture, Args&&... sprites)
            : SpriteBatch(sizeof...(Args))
        {
            set_texture(texture);
            add(std::forward<Args>(sprites)...);
        }

        SpriteBatch(SpriteBatch&&) noexcept;

        /// <summary>Returns a pointer to the beginning.</summary>
        [[nodiscard]] auto begin() { return sprites_.data(); }
        [[nodiscard]] auto begin() const { return sprites_.data(); }

        /// <summary>Returns a pointer to the end.</summary>
        [[nodiscard]] auto end() { return begin() + count_; }
        [[nodiscard]] auto end() const { return begin() + count_; }

        /// <summary>Returns whether the batch is visible.</summary>
        [[nodiscard]] auto is_visible() const { return visible_; }

        /// <summary>Returns current normal map.</summary>
        [[nodiscard]] auto normal() const { return normal_; }

        /// <summary>Returns sprite count.</summary>
        [[nodiscard]] auto size() const { return count_; }

        /// <summary>Returns current texture.</summary>
        [[nodiscard]] auto texture() const { return texture_; }

        /// <summary>Returns the vertex array object.</summary>
        [[nodiscard]] auto vertex_array() const -> const graphics::VertexArray&
        {
            return array_;
        }

        /// <summary>Returns vertex count.</summary>
        [[nodiscard]] auto vertex_count() const
        {
            return !visible_ ? 0 : count_ * 6;
        }

        /// <summary>Assigns a normal map.</summary>
        void set_normal(const graphics::Texture&);
        void set_normal(NotNull<const graphics::Texture*> texture)
        {
            set_normal(*texture.get());
        }

        /// <summary>Assigns a texture atlas.</summary>
        void set_texture(const graphics::Texture&);
        void set_texture(NotNull<const graphics::Texture*> texture)
        {
            set_texture(*texture.get());
        }

        /// <summary>Sets batch visibility.</summary>
        void set_visible(bool visible) { visible_ = visible; }

        [[nodiscard]] auto at(uint32_t i) -> Sprite& { return (*this)[i]; }

        [[nodiscard]] auto at(uint32_t i) const -> const Sprite&
        {
            return (*this)[i];
        }

        /// <summary>Brings sprite to front.</summary>
        void bring_to_front(uint32_t i);

        /// <summary>Brings sprite to front.</summary>
        void bring_to_front(const SpriteRef& ref)
        {
            R_ASSERT(ref.batch() == this,  //
                     "Sprite does not belong to this batch");

            bring_to_front(ref.index());
        }

        /// <summary>Clears all sprites.</summary>
        void clear() { count_ = 0; }

        /// <summary>Creates a sprite.</summary>
        /// <param name="width">Width of the sprite.</param>
        /// <param name="height">Height of the sprite.</param>
        /// <returns>
        ///   Reference to the newly created sprite, positioned at (0,0).
        /// </returns>
        auto create_sprite(uint32_t width, uint32_t height) -> SpriteRef;

        /// <summary>Erases a sprite from the batch.</summary>
        void erase(uint32_t i);

        /// <summary>Erases a sprite from the batch.</summary>
        void erase(const SpriteRef& ref)
        {
            R_ASSERT(ref.batch() == this,  //
                     "Sprite does not belong to this batch");

            erase(ref.index());
        }

        /// <summary>Returns the first sprite with the given id.</summary>
        [[nodiscard]] auto find_sprite_by_id(int id) const -> SpriteRef;

        /// <summary>Moves all sprites by (x,y).</summary>
        void move(const Vec2f&);

        /// <summary>Swaps two sprites' positions in the batch.</summary>
        void swap(uint32_t i, uint32_t j);

        /// <summary>Swaps two sprites' positions in the batch.</summary>
        void swap(const SpriteRef& a, const SpriteRef& b)
        {
            R_ASSERT(a.batch() == b.batch(),
                     "Cannot swap sprites from different batches.");
            swap(a.index(), b.index());
        }

        /// <summary>Updates the batch of sprites.</summary>
        void update(GameBase&);

        [[nodiscard]] auto operator[](uint32_t i) -> Sprite&
        {
            return sprites_[i];
        }

        [[nodiscard]] auto operator[](uint32_t i) const -> const Sprite&
        {
            return sprites_[i];
        }

#ifndef NDEBUG
        ~SpriteBatch();
#endif

#ifdef RAINBOW_TEST
        explicit SpriteBatch(const ISolemnlySwearThatIAmOnlyTesting&);

        [[nodiscard]] auto capacity() const { return sprites_.size(); }
        [[nodiscard]] auto sprites() { return sprites_.data(); }
        [[nodiscard]] auto sprites() const { return sprites_.data(); }
        [[nodiscard]] auto vertices() const { return vertices_.get(); }
#endif

    private:
        StableArray<Sprite> sprites_;

        /// <summary>Client vertex buffer.</summary>
        std::unique_ptr<SpriteVertex[]> vertices_;

        /// <summary>Client normal buffer.</summary>
        std::unique_ptr<Vec2f[]> normals_;

        /// <summary>Number of sprites.</summary>
        uint32_t count_ = 0;

        /// <summary>Shared, interleaved vertex buffer.</summary>
        graphics::Buffer vertex_buffer_;

        /// <summary>Shared normal buffer.</summary>
        graphics::Buffer normal_buffer_;

        /// <summary>Vertex array object.</summary>
        graphics::VertexArray array_;

        /// <summary>Texture used by all sprites in the batch.</summary>
        const graphics::Texture* texture_ = nullptr;

        /// <summary>Normal map used by all sprites in the batch.</summary>
        const graphics::Texture* normal_ = nullptr;

        /// <summary>Whether the batch is visible.</summary>
        bool visible_ = true;

        void add() {}

        template <typename T, typename... Args>
        void add(T&& sprite, Args&&... sprites)
        {
            static_assert(std::is_same_v<std::decay_t<T>, Sprite>,
                          "Elements must be of type Sprite");

            auto s = create_sprite(0, 0);
            *s = std::move(sprite);
            add(std::forward<Args>(sprites)...);
        }

        /// <summary>Sets the array state for this batch.</summary>
        void bind_arrays() const;
    };
}  // namespace rainbow

namespace rainbow::graphics
{
    void draw(Context&, const SpriteBatch&);
}  // namespace rainbow::graphics

#endif
