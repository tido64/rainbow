// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef DOTNET_RAINBOW_INTEROP_GRAPHICS_UFONTATLAS_H_
#define DOTNET_RAINBOW_INTEROP_GRAPHICS_UFONTATLAS_H_

#include "DotNet/Rainbow/Interop/Helper.h"
#include "Graphics/FontAtlas.h"

namespace rainbow
{
    struct UFontAtlas
    {
        uint32_t size_of_struct = sizeof(UFontAtlas);
        /*StdCall*/ FontAtlas* (*ctor_fn)(const char*, float) = &create<FontAtlas, const char*, float>;
        /*StdCall*/ void (*dtor_fn)(FontAtlas*) = &release_shared<FontAtlas>;
        /*ThisCall*/ int32_t (*get_height_fn)(FontAtlas*) = &get_height<FontAtlas>;
        /*ThisCall*/ bool (*is_valid_fn)(FontAtlas*) = &is_valid<FontAtlas>;
    };
}

#endif
