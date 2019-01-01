// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SPRITEBATCH_H_
#define GRAPHICS_SPRITEBATCH_H_

#include <type_traits>

#include "Graphics/LifetimeTracked.h"
#include "Graphics/Sprite.h"
#include "Graphics/TextureAtlas.h"
#include "Graphics/Vulkan.h"
#include "Memory/StableArray.h"

namespace rainbow::graphics
{
    class Driver;
}

namespace rainbow
{
    struct ISolemnlySwearThatIAmOnlyTesting;

    /// <summary>A drawable batch of sprites.</summary>
    /// <remarks>
    ///   All sprites share a common vertex buffer object (at different offsets)
    ///   and are drawn with a single draw call. The sprites must use the same
    ///   texture atlas.
    /// </remarks>
    class SpriteBatch : public LifetimeTracked<SpriteBatch>,
                        NonCopyable<SpriteBatch>
    {
    public:
        /// <summary>Creates a batch of sprites.</summary>
        /// <param name="count">Number of sprites to allocate for.</param>
        SpriteBatch(const graphics::Driver&, uint32_t count);

        template <typename... Args>
        SpriteBatch(const graphics::Driver& driver,
                    std::shared_ptr<TextureAtlas> texture,
                    Args&&... sprites)
            : SpriteBatch(driver, sizeof...(Args))
        {
            static_assert((std::is_same_v<std::decay_t<Args>, Sprite> && ...),
                          "Elements must be of type Sprite");

            set_texture(std::move(texture));
            (add(std::move<Args>(sprites)), ...);
        }

        SpriteBatch(SpriteBatch&&) noexcept;

        /// <summary>Returns a pointer to the beginning.</summary>
        auto begin() { return sprites_.data(); }
        auto begin() const { return sprites_.data(); }

        /// <summary>Returns a pointer to the end.</summary>
        auto end() { return begin() + count_; }
        auto end() const { return begin() + count_; }

        /// <summary>Returns whether the batch is visible.</summary>
        auto is_visible() const { return visible_; }

        /// <summary>Returns sprite count.</summary>
        auto size() const { return count_; }

        /// <summary>Returns current texture.</summary>
        auto texture() const -> TextureAtlas&
        {
            R_ASSERT(texture_.get(), "Texture is not set");
            return *texture_.get();
        }

        /// <summary>Returns the vertex buffer.</summary>
        auto vertex_buffer() const -> const vk::VertexBuffer&
        {
            return vertex_buffer_;
        }

        /// <summary>Returns the vertex count.</summary>
        auto vertex_count() const { return !visible_ ? 0 : count_ * 6; }

        /// <summary>Assigns a texture atlas.</summary>
        void set_texture(std::shared_ptr<TextureAtlas> texture);

        /// <summary>Sets batch visibility.</summary>
        void set_visible(bool visible) { visible_ = visible; }

        auto at(uint32_t i) -> Sprite& { return (*this)[i]; }
        auto at(uint32_t i) const -> const Sprite& { return (*this)[i]; }

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
        auto find_sprite_by_id(int id) const -> SpriteRef;

        /// <summary>Moves all sprites by (x,y).</summary>
        void move(Vec2f);

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
        void update();

        auto operator[](uint32_t i) -> Sprite& { return sprites_[i]; }

        auto operator[](uint32_t i) const -> const Sprite&
        {
            return sprites_[i];
        }

    private:
        StableArray<Sprite> sprites_;

        /// <summary>Client vertex buffer.</summary>
        std::unique_ptr<SpriteVertex[]> vertices_;

        /// <summary>Number of sprites.</summary>
        uint32_t count_;

        /// <summary>Shared, interleaved vertex buffer.</summary>
        vk::VertexBuffer vertex_buffer_;

        /// <summary>Texture atlas used by all sprites in the batch.</summary>
        std::shared_ptr<TextureAtlas> texture_;

        /// <summary>Whether the batch is visible.</summary>
        bool visible_;

        void add(Sprite&& sprite)
        {
            auto s = create_sprite(0, 0);
            *s = std::move(sprite);
        }

#ifdef RAINBOW_TEST
    public:
        explicit SpriteBatch(const ISolemnlySwearThatIAmOnlyTesting&);

        auto capacity() const { return sprites_.size(); }
        auto sprites() { return sprites_.data(); }
        auto sprites() const { return sprites_.data(); }
        auto vertices() const { return vertices_.get(); }
#endif
    };

    namespace vk
    {
        void draw(const CommandBuffer&, const SpriteBatch&, const IndexBuffer&);
    }
}  // namespace rainbow

#endif
