// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_DECODERS_PNG_H_
#define GRAPHICS_DECODERS_PNG_H_

#include <cstring>
#include <memory>

#include "Platform/Macros.h"
#ifdef RAINBOW_USE_COREGRAPHICS
#    ifdef RAINBOW_OS_MACOS
#        include <ApplicationServices/ApplicationServices.h>
#    else
#        include <CoreGraphics/CoreGraphics.h>
#    endif
#else
#    include <png.h>
#endif

namespace png
{
    bool check(const rainbow::Data& data)
    {
        constexpr uint8_t png_signature[8]{137, 80, 78, 71, 13, 10, 26, 10};
        return memcmp(data.bytes(), png_signature, sizeof(png_signature)) == 0;
    }

    auto decode(const rainbow::Data& data)
    {
#ifdef RAINBOW_USE_COREGRAPHICS
        auto data_provider = CGDataProviderCreateWithData(
            nullptr, data.bytes(), data.size(), nullptr);
        auto image_ref = CGImageCreateWithPNGDataProvider(
            data_provider, nullptr, false, kCGRenderingIntentDefault);

        const auto width = CGImageGetWidth(image_ref);
        const auto height = CGImageGetHeight(image_ref);
        const auto depth = CGImageGetBitsPerPixel(image_ref);
        const auto bits_per_component = CGImageGetBitsPerComponent(image_ref);
        const auto channels = depth / bits_per_component;
        const auto size = width * height * depth / 8;

        auto buffer = std::make_unique<uint8_t[]>(size);
        auto context = CGBitmapContextCreate(  //
            buffer.get(),
            width,
            height,
            bits_per_component,
            width * depth / 8,
            CGImageGetColorSpace(image_ref),
            kCGImageAlphaPremultipliedLast);
        CGContextSetBlendMode(context, kCGBlendModeCopy);
        CGContextDrawImage(context, CGRectMake(0, 0, width, height), image_ref);

        CGContextRelease(context);
        CGImageRelease(image_ref);
        CGDataProviderRelease(data_provider);

        return rainbow::Image(  //
            rainbow::Image::Format::PNG,
            width,
            height,
            depth,
            channels,
            size,
            buffer.release());
#else
        png_image pi{};
        pi.version = PNG_IMAGE_VERSION;

        auto memory = data.as<png_const_voidp>();
        if (!png_image_begin_read_from_memory(&pi, memory, data.size()))
            return rainbow::Image(rainbow::Image::Format::PNG);

        pi.format = PNG_IMAGE_PIXEL_CHANNELS(pi.format) == 2 ? PNG_FORMAT_GA
                                                             : PNG_FORMAT_RGBA;

        const auto size = PNG_IMAGE_SIZE(pi);
        auto buffer = std::make_unique<uint8_t[]>(size);
        png_image_finish_read(
            &pi, nullptr, buffer.get(), PNG_IMAGE_ROW_STRIDE(pi), nullptr);

        return rainbow::Image(  //
            rainbow::Image::Format::PNG,
            pi.width,
            pi.height,
            /* depth */ PNG_IMAGE_SAMPLE_SIZE(pi.format) * 8,
            /* channels */ PNG_IMAGE_SAMPLE_CHANNELS(pi.format),
            size,
            buffer.release());
#endif
    }
}  // namespace png

#endif
