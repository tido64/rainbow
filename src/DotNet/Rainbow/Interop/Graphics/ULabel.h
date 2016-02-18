// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef DOTNET_RAINBOW_INTEROP_GRAPHICS_ULABEL_H_
#define DOTNET_RAINBOW_INTEROP_GRAPHICS_ULABEL_H_

#include "DotNet/Rainbow/Interop/Helper.h"
#include "Graphics/Label.h"

namespace rainbow
{
    struct ULabel
    {
        uint32_t size_of_struct = sizeof(ULabel);
        /*StdCall*/ Label* (*ctor_fn)() = &create<Label>;
        /*StdCall*/ void (*dtor_fn)(Label*) = &release<Label>;

        /*ThisCall*/ int32_t (*get_alignment_fn)(Label*) =
            [](Label* label) { return static_cast<int32_t>(label->alignment()); };

        /*ThisCall*/ float (*get_angle_fn)(Label*) = &get_angle<Label>;
        /*ThisCall*/ Color (*get_color_fn)(Label*) = &get_color<Label>;
        /*ThisCall*/ Vec2f (*get_position_fn)(Label*) = &get_position<Label>;
        /*ThisCall*/ float (*get_scale_fn)(Label*) = &get_scale<Label>;
        /*ThisCall*/ int32_t (*get_width_fn)(Label*) = &get_width<Label>;
        /*ThisCall*/ void (*set_color_fn)(Label*, Color) = &set_color<Label>;
        /*ThisCall*/ void (*set_font_fn)(Label*, FontAtlas*) = &set_font<Label>;

        /*ThisCall*/ void (*set_alignment_fn)(Label*, int32_t) =
            [](Label* label, int32_t alignment) {
                label->set_alignment(static_cast<Label::TextAlignment>(alignment));
            };

        /*ThisCall*/ void (*set_position_fn)(Label*, Vec2f) = &set_position<Label>;
        /*ThisCall*/ void (*set_rotation_fn)(Label*, float) = &set_rotation<Label>;
        /*ThisCall*/ void (*set_scale_fn)(Label*, float) = &set_scale<Label>;
        /*ThisCall*/ void (*set_text_fn)(Label*, const char*) = &set_text<Label>;
    };
}

#endif
