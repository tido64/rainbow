// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SPRITEVERTEX_H_
#define GRAPHICS_SPRITEVERTEX_H_

#include "Common/Color.h"
#include "Math/Vec2.h"

namespace rainbow
{
    /// <summary>
    ///   A typical sprite vertex contains x- and y-coordinates for position and
    ///   texture.
    /// </summary>
    struct SpriteVertex
    {
        Colorb color;    ///< Texture colour, usually white.
        Vec2f texcoord;  ///< Texture coordinates.
        Vec2f position;  ///< Position of vertex.
    };
}

#endif
