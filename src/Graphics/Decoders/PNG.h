// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_DECODERS_PNG_H_
#define GRAPHICS_DECODERS_PNG_H_

#include <cstring>
#include <memory>

#include <png.h>

#define USE_PNG

namespace png
{
    bool check(const DataMap& data)
    {
        return png_sig_cmp(data.data(), 0, 8) == 0;
    }

    auto decode(const DataMap& data)
    {
        rainbow::Image image;
        image.format = rainbow::Image::Format::PNG;

        png_image pi;
        memset(&pi, 0, sizeof(pi));
        pi.version = PNG_IMAGE_VERSION;

        if (!png_image_begin_read_from_memory(&pi, data.data(), data.size()))
            return image;

        image.width = pi.width;
        image.height = pi.height;
        if (PNG_IMAGE_PIXEL_CHANNELS(pi.format) == 2)
        {
            pi.format = PNG_FORMAT_GA;
            image.depth = PNG_IMAGE_SAMPLE_SIZE(PNG_FORMAT_GA) * 8;
            image.channels = PNG_IMAGE_SAMPLE_CHANNELS(PNG_FORMAT_GA);
        }
        else
        {
            pi.format = PNG_FORMAT_RGBA;
            image.depth = PNG_IMAGE_SAMPLE_SIZE(PNG_FORMAT_RGBA) * 8;
            image.channels = PNG_IMAGE_SAMPLE_CHANNELS(PNG_FORMAT_RGBA);
        }

        auto buffer = std::make_unique<unsigned char[]>(PNG_IMAGE_SIZE(pi));
        png_image_finish_read(
            &pi, nullptr, buffer.get(), PNG_IMAGE_ROW_STRIDE(pi), nullptr);
        image.data = buffer.release();

        return image;
    }
}

#endif
