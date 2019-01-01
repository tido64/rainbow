// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_RENDERUNIT_H_
#define GRAPHICS_RENDERUNIT_H_

#include "Common/Variant.h"

namespace rainbow
{
    class Animation;
    class IDrawable;
    class Label;
    class SpriteBatch;

    namespace graphics
    {
        using RenderUnit = variant<  //
            Animation*,
            IDrawable*,
            Label*,
            SpriteBatch*>;
    }
}  // namespace rainbow

#endif
