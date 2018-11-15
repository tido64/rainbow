// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_DECODERS_SVG_H_
#define GRAPHICS_DECODERS_SVG_H_

#include "Common/Algorithm.h"
#include "ThirdParty/NanoSVG/NanoSVG.h"

#define USE_SVG

namespace std
{
    template <>
    struct default_delete<NSVGimage>
    {
        void operator()(NSVGimage* image) { nsvgDelete(image); }
    };

    template <>
    struct default_delete<NSVGrasterizer>
    {
        void operator()(NSVGrasterizer* rasterizer)
        {
            nsvgDeleteRasterizer(rasterizer);
        }
    };
}  // namespace std

namespace svg
{
    bool check(const rainbow::Data& data)
    {
        const auto p = data.bytes();
        return data.size() > 5 &&  //
               p[0] == '<' &&      //
               p[1] == 's' &&      //
               p[2] == 'v' &&      //
               p[3] == 'g' &&      //
               p[4] == ' ';
    }

    auto decode(const rainbow::Data& data, float scale)
    {
        rainbow::Image image{rainbow::Image::Format::SVG};

        std::unique_ptr<NSVGimage> img;
        {
            auto svg = std::make_unique<char[]>(data.size());
            std::copy(data.bytes(), data.bytes() + data.size(), svg.get());
            img.reset(nsvgParse(svg.get(), "px", 96.0f));
            if (img == nullptr)
                return image;
        }

        image.width = rainbow::ceil_pow2(img->width * scale);
        image.height = rainbow::ceil_pow2(img->height * scale);
        image.depth = 32;
        image.channels = 4;
        image.size = image.width * image.height * 4;

        auto buffer = std::make_unique<uint8_t[]>(image.size);
        std::unique_ptr<NSVGrasterizer> rasterizer{nsvgCreateRasterizer()};
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
}  // namespace svg

#endif
