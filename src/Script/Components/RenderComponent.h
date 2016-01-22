// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_COMPONENTS_RENDERCOMPONENT_H_
#define SCRIPT_COMPONENTS_RENDERCOMPONENT_H_

#include "Graphics/SpriteBatch.h"
#include "Script/Components/SceneComponent.h"

namespace rainbow
{
    class RenderComponent : public SceneComponent
    {
    public:
        auto batch() -> SpriteBatch& { return batch_; }
        auto batch() const -> const SpriteBatch& { return batch_; }
        auto first() const -> Sprite::Ref { return sprite(0); }
        auto is_mirrored() const -> bool { return first()->is_mirrored(); }
        auto second() const -> Sprite::Ref { return sprite(1); }
        auto third() const -> Sprite::Ref { return sprite(2); }

        /// <summary>Sets local angle of rotation (in radian).</summary>
        void set_local_rotation(float r)
        {
            for (auto&& sprite : batch())
                sprite.set_rotation(r);
        }

        /// <summary>Sets uniform local scaling factor.</summary>
        /// <param name="f">Scaling factor for both axes.</param>
        void set_local_scale(float f) { set_local_scale(Vec2f(f, f)); }

        /// <summary>Sets non-uniform local scaling factor.</summary>
        /// <param name="f">Scaling factors for x- and y-axis.</param>
        void set_local_scale(const Vec2f& scale)
        {
            for (auto&& sprite : batch())
                sprite.set_scale(scale);
        }

        /// <summary>Sets texture.</summary>
        /// <param name="id">Identifier of the texture to set.</param>
        void set_texture(SharedPtr<TextureAtlas> texture)
        {
            batch().set_texture(texture);
        }

        /// <summary>Adds a sprite with specified dimension.</summary>
        /// <param name="width">Width of the sprite.</param>
        /// <param name="height">Height of the sprite.</param>
        /// <returns>
        ///   Reference to the newly created sprite, positioned at (0,0).
        /// </returns>
        auto add_sprite(unsigned int width, unsigned int height) -> Sprite::Ref
        {
            return batch().create_sprite(width, height);
        }

        /// <summary>Hides render component.</summary>
        void hide() { batch().set_visible(false); }

        /// <summary>Mirrors render component.</summary>
        void mirror()
        {
            for (auto&& sprite : batch())
                sprite.mirror();
        }

        /// <summary>Moves component by specified vector.</summary>
        void move(const Vec2f& delta) override
        {
            SceneComponent::move(delta);
            for (auto&& sprite : batch())
                sprite.move(delta);
        }

        /// <summary>Shows render component.</summary>
        void show() { batch().set_visible(true); }

        /// <summary>Returns the <see cref="Sprite"/> at index.</summary>
        auto sprite(size_t i) const -> Sprite::Ref
        {
            R_ASSERT(i < batch().size(),
                     "Tried to access a sprite out of range.");
            return {&batch(), i};
        }

    private:
        SpriteBatch batch_;
    };
}

#endif
