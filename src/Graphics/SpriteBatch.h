// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SPRITEBATCH_H_
#define GRAPHICS_SPRITEBATCH_H_

#include <type_traits>

#include "Graphics/Buffer.h"
#include "Graphics/Sprite.h"
#include "Graphics/TextureAtlas.h"
#include "Graphics/VertexArray.h"
#include "Memory/StableArray.h"

namespace rainbow
{
    struct ISolemnlySwearThatIAmOnlyTesting;

    /// <summary>A drawable batch of sprites.</summary>
    /// <remarks>
    ///   All sprites share a common vertex buffer object (at different offsets)
    ///   and are drawn with a single glDraw call. The sprites must use the same
    ///   texture atlas.
    /// </remarks>
    class SpriteBatch : private NonCopyable<SpriteBatch>
    {
    INTERNAL:
        class State
        {
            static constexpr uint32_t kSizeMask = 0xffffff;
            static constexpr uint32_t kUpdateBit = 1 << 24;
            static constexpr uint32_t kVisibilityBit = 1 << 25;

        public:
            auto is_visible() const { return (state_ & kVisibilityBit) != 0; }
            auto needs_update() const { return (state_ & kUpdateBit) != 0; }
            auto size() const { return state_ & kSizeMask; }

            void set_needs_update(bool value) { set(kUpdateBit, value); }
            void set_visible(bool visible) { set(kVisibilityBit, visible); }

            void clear_size() { state_ &= ~kSizeMask; }
            auto decrement() { return add(-1); }
            auto increment() { return add(1); }

        private:
            uint32_t state_ = kVisibilityBit;

            auto add(int32_t i) -> uint32_t
            {
                const auto size = (state_ + i) & kSizeMask;
                state_ = size | (state_ & ~kSizeMask);
                return size;
            }

            void set(uint32_t bit, int32_t value)
            {
                state_ ^= (static_cast<uint32_t>(-value) ^ state_) & bit;
            }
        };

    public:
        /// <summary>Creates a batch of sprites.</summary>
        /// <param name="count">Number of sprites to allocate for.</param>
        SpriteBatch(uint32_t count);

        template <typename... Args>
        SpriteBatch(SharedPtr<TextureAtlas> texture, Args&&... sprites)
            : SpriteBatch(sizeof...(Args))
        {
            set_texture(std::move(texture));
            add(std::forward<Args>(sprites)...);
        }

        SpriteBatch(SpriteBatch&&) noexcept;

        /// <summary>Returns a pointer to the beginning.</summary>
        auto begin() { return sprites_.data(); }
        auto begin() const { return sprites_.data(); }

        /// <summary>Returns a pointer to the end.</summary>
        auto end() { return begin() + size(); }
        auto end() const { return begin() + size(); }

        /// <summary>Returns whether the batch is visible.</summary>
        auto is_visible() const { return state_.is_visible(); }

        /// <summary>Returns current normal map.</summary>
        auto normal() const -> const TextureAtlas&
        {
            R_ASSERT(normal_.get(), "Normal texture is not set");
            return *normal_.get();
        }

        /// <summary>Returns sprite count.</summary>
        auto size() const -> uint32_t { return state_.size(); }

        /// <summary>Returns current texture.</summary>
        auto texture() const -> TextureAtlas&
        {
            R_ASSERT(texture_.get(), "Texture is not set");
            return *texture_.get();
        }

        /// <summary>Returns the vertex array object.</summary>
        auto vertex_array() const -> const graphics::VertexArray&
        {
            return array_;
        }

        /// <summary>Returns the vertex count.</summary>
        auto vertex_count() const { return !is_visible() ? 0 : size() * 6; }

        /// <summary>Assigns a normal map.</summary>
        void set_normal(SharedPtr<TextureAtlas> texture);

        /// <summary>Assigns a texture atlas.</summary>
        void set_texture(SharedPtr<TextureAtlas> texture);

        /// <summary>Sets batch visibility.</summary>
        void set_visible(bool visible) { state_.set_visible(visible); }

        auto at(uint32_t i) -> Sprite& { return (*this)[i]; }
        auto at(uint32_t i) const -> const Sprite& { return (*this)[i]; }

        /// <summary>Binds all used textures.</summary>
        void bind_textures() const;

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
        void clear() { state_.clear_size(); }

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
        void update();
        void upload() const;

        auto operator[](uint32_t i) -> Sprite& { return sprites_[i]; }

        auto operator[](uint32_t i) const -> const Sprite&
        {
            return sprites_[i];
        }

#ifndef NDEBUG
        ~SpriteBatch();
#endif

#ifdef RAINBOW_TEST
        explicit SpriteBatch(const ISolemnlySwearThatIAmOnlyTesting&);

        auto capacity() const { return sprites_.size(); }
        auto sprites() { return sprites_.data(); }
        auto sprites() const { return sprites_.data(); }
        auto vertices() const { return vertices_.get(); }
#endif

    private:
        StableArray<Sprite> sprites_;

        /// <summary>Client vertex buffer.</summary>
        std::unique_ptr<SpriteVertex[]> vertices_;

        /// <summary>Client normal buffer.</summary>
        std::unique_ptr<Vec2f[]> normals_;

        /// <summary>
        ///   State contains number of sprites, visibility, and staleness.
        /// </summary>
        State state_;

        /// <summary>Shared, interleaved vertex buffer.</summary>
        graphics::Buffer vertex_buffer_;

        /// <summary>Shared normal buffer.</summary>
        graphics::Buffer normal_buffer_;

        /// <summary>Vertex array object.</summary>
        graphics::VertexArray array_;

        /// <summary>Normal map used by all sprites in the batch.</summary>
        SharedPtr<TextureAtlas> normal_;

        /// <summary>Texture atlas used by all sprites in the batch.</summary>
        SharedPtr<TextureAtlas> texture_;

        void add() {}

        template <typename T, typename... Args>
        void add(T&& sprite, Args&&... sprites)
        {
            static_assert(std::is_same<std::decay_t<T>, Sprite>::value,
                          "Elements must be of type Sprite");

            auto s = create_sprite(0, 0);
            *s = std::move(sprite);
            add(std::forward<Args>(sprites)...);
        }

        /// <summary>Sets the array state for this batch.</summary>
        void bind_arrays() const;
    };
}  // namespace rainbow

#endif
