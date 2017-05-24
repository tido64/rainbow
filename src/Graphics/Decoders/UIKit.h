// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_DECODERS_UIKIT_H_
#define GRAPHICS_DECODERS_UIKIT_H_

#include <UIKit/UIKit.h>

#include "Common/Logging.h"

#define USE_UIKIT

namespace uikit
{
    auto decode(const rainbow::Data& source)
    {
        rainbow::Image image;

        NSData* data =
            [NSData dataWithBytesNoCopy:const_cast<void*>(source.as<void*>())
                                 length:source.size()
                           freeWhenDone:NO];
        UIImage* uiimage = [UIImage imageWithData:data];
        if (!uiimage)
        {
            R_ASSERT(uiimage, "Unknown texture format");
            return image;
        }

        image.width = CGImageGetWidth(uiimage.CGImage);
        image.height = CGImageGetHeight(uiimage.CGImage);
        image.depth = CGImageGetBitsPerPixel(uiimage.CGImage);
        image.channels =
            image.depth / CGImageGetBitsPerComponent(uiimage.CGImage);
        if (image.depth == 24 || (image.depth == 8 && image.channels == 1))
        {
            // - Increase depth from 24 to 32 because iOS breaks on GL_RGB; or
            // - Assume PNG8.
            image.depth = 32;
            image.channels = 4;
        }
        CGRect bounds = CGRectMake(0, 0, image.width, image.height);

        CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();
        auto buffer =
            std::make_unique<uint8_t[]>(image.height * image.width * 4);
        CGContextRef context = CGBitmapContextCreate(
            buffer.get(),
            image.width,
            image.height,
            8,
            image.width * 4,
            color_space,
            kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
        CGColorSpaceRelease(color_space);

        CGContextClearRect(context, bounds);
        CGContextTranslateCTM(context, 0, 0);
        CGContextDrawImage(context, bounds, uiimage.CGImage);
        CGContextRelease(context);

        image.data = buffer.release();
        return image;
    }
}

#endif
