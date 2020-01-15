// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Image.h"

#include "Common/Data.h"
#include "Common/Logging.h"
#include "Graphics/Decoders/PNG.h"
#include "Graphics/Decoders/SVG.h"
#include "Graphics/OpenGL.h"
#ifdef GL_IMG_texture_compression_pvrtc
#    include "Graphics/Decoders/PVRTC.h"
#endif  // GL_IMG_texture_compression_pvrtc
#if defined(GL_EXT_texture_compression_s3tc) || defined(RAINBOW_OS_WINDOWS)
#    include "Graphics/Decoders/DDS.h"
#endif  // GL_EXT_texture_compression_s3tc

using rainbow::Data;
using rainbow::Image;

auto Image::decode(const Data& data, float scale) -> Image
{
#ifdef USE_DDS
    if (dds::check(data))
        return dds::decode(data);
#endif  // USE_DDS

#ifdef USE_PVRTC
    if (pvrtc::check(data))
        return pvrtc::decode(data);
#endif  // USE_PVRTC

    if (png::check(data))
        return png::decode(data);

    if (svg::check(data))
        return svg::decode(data, scale);

#ifdef RAINBOW_TEST
    if (memcmp(data.bytes(), "RNBWMOCK", 8) == 0)
        return {};
#endif

    R_ASSERT(false, "Unknown image format");
    return {};
}
