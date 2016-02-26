// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_DECODERS_SVG_H_
#define GRAPHICS_DECODERS_SVG_H_

#include "ThirdParty/NanoSVG.h"

#define USE_SVG

namespace svg
{
    bool check(const DataMap& data)
    {
        return strstr(reinterpret_cast<const char*>(data.data()), "<svg ");
    }

    rainbow::Image decode(const DataMap& data, float scale)
    {
        using NSVGImage = std::unique_ptr<NSVGimage, decltype(&nsvgDelete)>;
        using NSVGRasterizer =
            std::unique_ptr<NSVGrasterizer, decltype(&nsvgDeleteRasterizer)>;

        rainbow::Image image;
        image.format = rainbow::Image::Format::SVG;

        NSVGImage img(nullptr, nsvgDelete);
        {
            auto svg = std::make_unique<char[]>(data.size());
            std::copy(data.data(), data.data() + data.size(), svg.get());
            img.reset(nsvgParse(svg.get(), "px", 96.0f));
            if (!img)
                return image;
        }

        image.width = rainbow::next_pow2(img->width * scale);
        image.height = rainbow::next_pow2(img->height * scale);
        image.depth = 32;
        image.channels = 4;
        image.size = image.width * image.height * 4;

        auto buffer = std::make_unique<rainbow::byte_t[]>(image.size);
        NSVGRasterizer rasterizer(nsvgCreateRasterizer(), nsvgDeleteRasterizer);
        nsvgRasterize(rasterizer.get(),
                      img.get(),
                      0.0f,
                      0.0f,
                      scale,
                      buffer.get(),
                      img->width * scale,
                      img->height * scale,
                      image.width * 4);

        image.data = buffer.release();
        return image;
    }
}

#endif
