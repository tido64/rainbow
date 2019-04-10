// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_IMAGE_H_
#define GRAPHICS_IMAGE_H_

#include <cstddef>
#include <cstdint>

#include "Common/NonCopyable.h"

namespace rainbow
{
    class Data;

    struct Image : private NonCopyable<Image>
    {
        enum class Format
        {
            Unknown,
            ATITC,  // Adreno
            BC1,    // DXT1
            BC2,    // DXT3
            BC3,    // DXT5
            ETC1,   // OpenGL ES standard
            PVRTC,  // iOS, OMAP43xx, PowerVR
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
        static auto decode(const Data&, float scale) -> Image;

        Format format;
        uint32_t width;
        uint32_t height;
        uint32_t depth;
        uint32_t channels;
        size_t size;
        const uint8_t* data;

        Image(Format format_ = Format::Unknown,
              uint32_t width_ = 0,
              uint32_t height_ = 0,
              uint32_t depth_ = 0,
              uint32_t channels_ = 0,
              size_t size_ = 0,
              const uint8_t* data_ = nullptr)
            : format(format_), width(width_), height(height_), depth(depth_),
              channels(channels_), size(size_), data(data_)
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
                case Format::BC1:
                case Format::BC2:
                case Format::BC3:
                case Format::ETC1:
                case Format::PVRTC:
                    break;

                case Format::PNG:
                case Format::SVG:
                default:
                    delete[] data;
                    break;
            }
        }
    };
}  // namespace rainbow

#endif
