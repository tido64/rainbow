// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef DOTNET_RAINBOW_INTEROP_GRAPHICS_USPRITEBATCH_H_
#define DOTNET_RAINBOW_INTEROP_GRAPHICS_USPRITEBATCH_H_

#include "DotNet/Rainbow/Interop/Helper.h"
#include "Graphics/SpriteBatch.h"

namespace rainbow
{
    struct USpriteBatch
    {
        uint32_t size_of_struct = sizeof(USpriteBatch);
        /*StdCall*/ SpriteBatch* (*ctor_fn)(uint32_t) = &create<SpriteBatch, uint32_t>;
        /*StdCall*/ void (*dtor_fn)(SpriteBatch*) = &release<SpriteBatch>;
        /*ThisCall*/ uint32_t (*count_fn)(SpriteBatch*) = &size<SpriteBatch>;
        /*ThisCall*/ bool (*is_visible_fn)(SpriteBatch*) = &is_visible<SpriteBatch>;
        /*ThisCall*/ void (*set_normal_fn)(SpriteBatch*, TextureAtlas*) = &set_normal<SpriteBatch>;
        /*ThisCall*/ void (*set_texture_fn)(SpriteBatch*, TextureAtlas*) = &set_texture<SpriteBatch>;
        /*ThisCall*/ void (*set_visible_fn)(SpriteBatch*, bool) = &set_visible<SpriteBatch>;

        /*ThisCall*/ void (*bring_to_front_fn)(SpriteBatch*, uint32_t) =
            [](SpriteBatch* batch, uint32_t sprite) {
                batch->bring_to_front(sprite);
            };

        /*ThisCall*/ void (*clear_fn)(SpriteBatch*) = &clear<SpriteBatch>;

        /*ThisCall*/ uint32_t (*create_sprite_fn)(SpriteBatch*, uint32_t, uint32_t) =
            [](SpriteBatch* batch, uint32_t width, uint32_t height) {
                return static_cast<uint32_t>(batch->create_sprite(width, height));
            };

        /*ThisCall*/ void (*erase_fn)(SpriteBatch*, uint32_t) =
            [](SpriteBatch* batch, uint32_t sprite) {
                batch->erase(sprite);
            };

        /*ThisCall*/ uint32_t (*find_sprite_by_id_fn)(SpriteBatch*, int32_t) =
            [](SpriteBatch* batch, int32_t id) {
                auto sprite = batch->find_sprite_by_id(id);
                return !sprite ? std::numeric_limits<uint32_t>::max()
                               : static_cast<uint32_t>(sprite);
            };

        /*ThisCall*/ void (*move_fn)(SpriteBatch*, Vec2f) = &move<SpriteBatch>;

        /*ThisCall*/ void (*swap_fn)(SpriteBatch*, uint32_t, uint32_t) =
            [](SpriteBatch* batch, uint32_t i, uint32_t j) {
                batch->swap(i, j);
            };
    };
}

#endif
