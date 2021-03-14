// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_DECODERS_PVRTC_H_
#define GRAPHICS_DECODERS_PVRTC_H_

#include <cstdint>

#include "Common/Logging.h"

#define USE_PVRTC

namespace
{
    constexpr uint32_t kPVRHeaderVersion3 = 0x03525650;  // "PVR3"

    /// PVRTC texture header, as specified by Imagination Technologies Ltd.
    /// \see
    /// http://www.imgtec.net/powervr/insider/docs/PVR%20File%20Format.Specification.1.0.11.External.pdf
    struct PVRTexHeader {
        uint32_t version;
        uint32_t flags;
        uint64_t pixel_format;
        uint32_t colour_space;
        uint32_t channel_type;
        uint32_t height;
        uint32_t width;
        uint32_t depth;
        uint32_t num_surfaces;
        uint32_t num_faces;
        uint32_t mipmap_count;
        uint32_t metadata_size;
    };
}  // namespace

namespace pvrtc
{
    bool check(const rainbow::Data& data)
    {
        return data.as<PVRTexHeader*>()->version == kPVRHeaderVersion3;
    }

    auto decode([[maybe_unused]] const rainbow::Data& data)
    {
        rainbow::Image image{rainbow::Image::Format::PVRTC};

#ifdef RAINBOW_OS_IOS
        auto header = data.as<PVRTexHeader*>();
        R_ASSERT(CFSwapInt32LittleToHost(header->mipmap_count) == 1,
                 "Mipmaps are not supported");

        image.height = CFSwapInt32LittleToHost(header->height);
        image.width = CFSwapInt32LittleToHost(header->width);
        R_ASSERT(image.width == image.height,
                 "Compressed textures *must* be square");

        const uint64_t format = CFSwapInt64LittleToHost(header->pixel_format);
        R_ASSERT(format < 4, "Invalid PVRTC format type");
        image.depth = (format < 2 ? 2 : 4);
        image.channels = ((format & 0x01) == 0 ? 3 : 4);

        const size_t offset =
            sizeof(*header) + CFSwapInt32LittleToHost(header->metadata_size);
        image.size = image.width * image.height * image.depth >> 3;
        R_ASSERT(
            offset + image.size == data.size(), "Unsupported PVR file format");
        image.data = data.bytes() + offset;
#endif

        return image;
    }
}  // namespace pvrtc

#endif
