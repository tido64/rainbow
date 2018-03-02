// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_IMAGE_H_
#define GRAPHICS_IMAGE_H_

#include "Common/Data.h"

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
            PNG,
            SVG,
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
        static Image decode(const Data&, float scale);

        Format format;
        uint32_t width;
        uint32_t height;
        uint32_t depth;
        uint32_t channels;
        size_t size;
        const uint8_t* data;

        Image() : Image(Format::Unknown) {}

        Image(Format format_)
            : format(format_), width(0), height(0), depth(0), channels(0),
              size(0), data(nullptr)
        {
        }

        Image(Image&& image) noexcept
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
                case Format::SVG:
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
#   include "Graphics/Decoders/SVG.h"
#endif
#ifdef GL_IMG_texture_compression_pvrtc
#   include "Graphics/Decoders/PVRTC.h"
#endif  // GL_IMG_texture_compression_pvrtc
#ifdef GL_EXT_texture_compression_s3tc
#   include "Graphics/Decoders/DDS.h"
#endif  // GL_EXT_texture_compression_s3tc

namespace rainbow
{
    Image Image::decode(const Data& data, [[maybe_unused]] float scale)
    {
#ifdef USE_DDS
        if (dds::check(data))
            return dds::decode(data);
#endif  // USE_DDS

#ifdef USE_PVRTC
        if (pvrtc::check(data))
            return pvrtc::decode(data);
#endif  // USE_PVRTC

#ifdef USE_PNG
        if (png::check(data))
            return png::decode(data);
#endif  // USE_PNG

#ifdef USE_SVG
        if (svg::check(data))
            return svg::decode(data, scale);
#endif  // USE_SVG

#ifdef USE_UIKIT
        return uikit::decode(data);
#else
        return {};
#endif
    }
}

#endif
