// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef DOTNET_RAINBOW_INTEROP_GRAPHICS_UTEXTUREATLAS_H_
#define DOTNET_RAINBOW_INTEROP_GRAPHICS_UTEXTUREATLAS_H_

#include "DotNet/Rainbow/Interop/Helper.h"
#include "FileSystem/FileSystem.h"
#include "Graphics/TextureAtlas.h"

namespace rainbow
{
    struct UTextureAtlas
    {
        uint32_t size_of_struct = sizeof(UTextureAtlas);

        /*StdCall*/ TextureAtlas* (*ctor_fn)(const char*, float) =
            [](const char* path, float scale) {
                auto p = rainbow::filesystem::relative(path);
                return std::make_unique<TextureAtlas>(p, scale).release();
            };

        /*StdCall*/ void (*dtor_fn)(TextureAtlas*) = &release_shared<TextureAtlas>;
        /*ThisCall*/ int32_t (*get_height_fn)(TextureAtlas*) = &get_height<TextureAtlas>;
        /*ThisCall*/ int32_t (*get_width_fn)(TextureAtlas*) = &get_width<TextureAtlas>;
        /*ThisCall*/ bool (*is_valid_fn)(TextureAtlas*) = &is_valid<TextureAtlas>;

        /*ThisCall*/ uint32_t (*add_region_fn)(TextureAtlas*, int32_t, int32_t, int32_t, int32_t) =
            [](TextureAtlas* atlas, int32_t x, int32_t y, int32_t width, int32_t height) {
                return atlas->add_region(x, y, width, height);
            };

        /*ThisCall*/ uint32_t (*get_texture_region_fn)(TextureAtlas*, uint32_t) = &subscript_op<TextureAtlas, uint32_t>;

        /*ThisCall*/ void (*set_regions_fn)(TextureAtlas*, const int32_t*, int32_t) =
            [](TextureAtlas* atlas, const int32_t* rectangles, int32_t count) {
                atlas->set_regions(ArrayView<int>{rectangles, static_cast<size_t>(count)});
            };
    };
}

#endif
