// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef DOTNET_RAINBOW_INTEROP_GRAPHICS_USPRITE_H_
#define DOTNET_RAINBOW_INTEROP_GRAPHICS_USPRITE_H_

#include "Graphics/Sprite.h"

namespace rainbow
{
    struct USprite
    {
        uint32_t size_of_struct = sizeof(USprite);
        /*StdCall*/ void (*dtor_fn)(void*) = [](void*) {};

        /*ThisCall*/ float (*get_angle_fn)(SpriteBatch*, uint32_t) =
            [](SpriteBatch* parent, uint32_t i) {
                return parent->at(i).angle();
            };

        /*ThisCall*/ Color (*get_color_fn)(SpriteBatch*, uint32_t) =
            [](SpriteBatch* parent, uint32_t i) {
                return parent->at(i).color();
            };

        /*ThisCall*/ uint32_t (*get_height_fn)(SpriteBatch*, uint32_t) =
            [](SpriteBatch* parent, uint32_t i) {
                return parent->at(i).height();
            };

        /*ThisCall*/ int32_t (*get_id_fn)(SpriteBatch*, uint32_t) =
            [](SpriteBatch* parent, uint32_t i) {
                return parent->at(i).id();
            };

        /*ThisCall*/ bool (*is_flipped_fn)(SpriteBatch*, uint32_t) =
            [](SpriteBatch* parent, uint32_t i) {
                return parent->at(i).is_flipped();
            };

        /*ThisCall*/ bool (*is_hidden_fn)(SpriteBatch*, uint32_t) =
            [](SpriteBatch* parent, uint32_t i) {
                return parent->at(i).is_hidden();
            };

        /*ThisCall*/ bool (*is_mirrored_fn)(SpriteBatch*, uint32_t) =
            [](SpriteBatch* parent, uint32_t i) {
                return parent->at(i).is_mirrored();
            };

        /*ThisCall*/ Vec2f (*get_pivot_fn)(SpriteBatch*, uint32_t) =
            [](SpriteBatch* parent, uint32_t i) {
                return parent->at(i).pivot();
            };

        /*ThisCall*/ Vec2f (*get_position_fn)(SpriteBatch*, uint32_t) =
            [](SpriteBatch* parent, uint32_t i) {
                return parent->at(i).position();
            };

        /*ThisCall*/ Vec2f (*get_scale_fn)(SpriteBatch*, uint32_t) =
            [](SpriteBatch* parent, uint32_t i) {
                return parent->at(i).scale();
            };

        /*ThisCall*/ uint32_t (*get_width_fn)(SpriteBatch*, uint32_t) =
            [](SpriteBatch* parent, uint32_t i) {
                return parent->at(i).width();
            };

        /*ThisCall*/ void (*set_color_fn)(SpriteBatch*, uint32_t, Color) =
            [](SpriteBatch* parent, uint32_t i, Color color) {
                parent->at(i).set_color(color);
            };

        /*ThisCall*/ void (*set_flipped_fn)(SpriteBatch*, uint32_t, bool) =
            [](SpriteBatch* parent, uint32_t i, bool flipped) {
                Sprite& sprite = parent->at(i);
                if (flipped != sprite.is_flipped())
                    sprite.flip();
            };

        /*ThisCall*/ void (*set_id_fn)(SpriteBatch*, uint32_t, int32_t) =
            [](SpriteBatch* parent, uint32_t i, int id) {
                parent->at(i).set_id(id);
            };

        /*ThisCall*/ void (*set_mirrored_fn)(SpriteBatch*, uint32_t, bool) =
            [](SpriteBatch* parent, uint32_t i, bool mirrored) {
                Sprite& sprite = parent->at(i);
                if (mirrored != sprite.is_mirrored())
                    sprite.mirror();
            };

        /*ThisCall*/ void (*set_normal_fn)(SpriteBatch*, uint32_t, uint32_t) =
            [](SpriteBatch* parent, uint32_t i, uint32_t id) {
                parent->at(i).set_normal(id);
            };

        /*ThisCall*/ void (*set_pivot_fn)(SpriteBatch*, uint32_t, Vec2f) =
            [](SpriteBatch* parent, uint32_t i, Vec2f pivot) {
                parent->at(i).set_pivot(pivot);
            };

        /*ThisCall*/ void (*set_position_fn)(SpriteBatch*, uint32_t, Vec2f) =
            [](SpriteBatch* parent, uint32_t i, Vec2f position) {
                parent->at(i).set_position(position);
            };

        /*ThisCall*/ void (*set_rotation_fn)(SpriteBatch*, uint32_t, float) =
            [](SpriteBatch* parent, uint32_t i, float r) {
                parent->at(i).set_rotation(r);
            };

        /*ThisCall*/ void (*set_scale_fn)(SpriteBatch*, uint32_t, Vec2f) =
            [](SpriteBatch* parent, uint32_t i, Vec2f scale) {
                parent->at(i).set_scale(scale);
            };

        /*ThisCall*/ void (*set_texture_fn)(SpriteBatch*, uint32_t, uint32_t) =
            [](SpriteBatch* parent, uint32_t i, uint32_t id) {
                parent->at(i).set_texture(id);
            };

        /*ThisCall*/ void (*hide_fn)(SpriteBatch*, uint32_t) =
            [](SpriteBatch* parent, uint32_t i) {
                parent->at(i).hide();
            };

        /*ThisCall*/ void (*show_fn)(SpriteBatch*, uint32_t) =
            [](SpriteBatch* parent, uint32_t i) {
                parent->at(i).show();
            };
    };
}

#endif
