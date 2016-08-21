// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/DataMap.h"
#include "Graphics/OpenGL.h"
#ifndef RAINBOW_OS_IOS
#   define USE_PNG 1
#   define USE_SVG 1
#endif
#ifdef GL_IMG_texture_compression_pvrtc
#   define USE_PVRTC 1
#endif
#ifdef GL_EXT_texture_compression_s3tc
#   define USE_DDS 1
#endif

#ifdef USE_DDS
namespace dds
{
    extern bool check(const DataMap& data);
}

TEST(DecodersTest, DetectsDDS)
{
   constexpr uint8_t kNotDDSSignature[]{' ', 'S', 'D', 'D'};
   constexpr uint8_t kDDSSignature[]{'D', 'D', 'S', ' '};

   ASSERT_TRUE(dds::check(DataMap{kDDSSignature}));
   ASSERT_FALSE(dds::check(DataMap{kNotDDSSignature}));
}
#endif  // USE_DDS

#ifdef USE_PNG
namespace png
{
    extern bool check(const DataMap& data);
}

TEST(DecodersTest, DetectsPNG)
{
   constexpr uint8_t kNotPNGSignature[]{10, 26, 10, 13, 71, 78, 80, 137};
   constexpr uint8_t kPNGSignature[]{137, 80, 78, 71, 13, 10, 26, 10};

   ASSERT_TRUE(png::check(DataMap{kPNGSignature}));
   ASSERT_FALSE(png::check(DataMap{kNotPNGSignature}));
}
#endif  // USE_PNG

#ifdef USE_PVRTC
namespace pvrtc
{
    extern bool check(const DataMap& data);
}

TEST(DecodersTest, DetectsPVRTC)
{
   constexpr uint8_t kNotPVRTCSignature[]{'3', 'R', 'V', 'P'};
   constexpr uint8_t kPVRTCSignature[]{'P', 'V', 'R', '3'};

   ASSERT_TRUE(pvrtc::check(DataMap{kPVRTCSignature}));
   ASSERT_FALSE(pvrtc::check(DataMap{kNotPVRTCSignature}));
}
#endif  // USE_PVRTC

#ifdef USE_SVG
namespace svg
{
    extern bool check(const DataMap& data);
}

TEST(DecodersTest, DetectsSVG)
{
   constexpr uint8_t kNotSVGSignature[]{'\0', ' ', 'g', 'v', 's', '<'};
   constexpr uint8_t kSVGSignature[]{'<', 's', 'v', 'g', ' ', '\0'};

   ASSERT_TRUE(svg::check(DataMap{kSVGSignature}));
   ASSERT_FALSE(svg::check(DataMap{kNotSVGSignature}));
}
#endif  // USE_SVG
