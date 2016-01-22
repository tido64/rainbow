// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_IMAGE_H_
#define GRAPHICS_IMAGE_H_

#include "Common/DataMap.h"

namespace rainbow
{
    struct Image : private NonCopyable<Image>
    {
        enum class Format
        {
            Unknown,
            ATITC,  // Adreno
            ETC1,   // OpenGL ES standard
            PVRTC,  // iOS, OMAP43xx, PowerVR
            S3TC,   // Desktops, Tegra
            PNG
        };

        /// <summary>Creates an Image struct from image data.</summary>
        /// <remarks>
        ///   Supports
        ///   <list type="bullet">
        ///     <item>iOS: PVRTC and whatever UIImage devours.</item>
        ///     <item>Others: PNG.</item>
        ///   </list>
        ///   Limitations
        ///   <list type="bullet">
        ///     <item>
        ///       PNG: Non-transparent grayscale is assumed to be PNG8, i.e.
        ///       it's not supported.
        ///     </item>
        ///     <item>
        ///       PVRTC: PVR3 only; square, power of 2; no mipmaps;
        ///       pre-multiplied alpha.
        ///     </item>
        ///   </list>
        /// </remarks>
        static Image decode(const DataMap&);

        Format format = Format::Unknown;
        unsigned int width = 0;
        unsigned int height = 0;
        unsigned int depth = 0;
        unsigned int channels = 0;
        size_t size = 0;
        const byte_t* data = nullptr;

        Image() = default;

        Image(Image&& image)
            : format(image.format), width(image.width), height(image.height),
              depth(image.depth), channels(image.channels), size(image.size),
              data(image.data)
        {
            image.format = Format::Unknown;
            image.width = 0;
            image.height = 0;
            image.depth = 0;
            image.channels = 0;
            image.size = 0;
            image.data = nullptr;
        }

        ~Image()
        {
            switch (format)
            {
                case Format::ATITC:
                case Format::ETC1:
                case Format::PVRTC:
                case Format::S3TC:
                    break;

                case Format::PNG:
                default:
                    delete[] data;
                    break;
            }
        }
    };
}

#include "Graphics/OpenGL.h"
#if defined(RAINBOW_OS_IOS)
#   include "Graphics/Decoders/UIKit.h"
#else
#   include "Graphics/Decoders/PNG.h"
#endif
#ifdef GL_IMG_texture_compression_pvrtc
#   include "Graphics/Decoders/PVRTC.h"
#endif  // GL_IMG_texture_compression_pvrtc

namespace rainbow
{
    Image Image::decode(const DataMap& data)
    {
#ifdef USE_PVRTC
        if (pvrtc::check(data))
            return pvrtc::decode(data);
#endif  // USE_PVRTC

#ifdef USE_PNG
        if (png::check(data))
            return png::decode(data);
#endif  // USE_PNG

#ifdef USE_UIKIT
        return uikit::decode(data);
#else
        return {};
#endif
    }
}

#endif
