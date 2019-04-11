// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/Data.h"
#include "Graphics/OpenGL.h"

#ifdef GL_IMG_texture_compression_pvrtc
#    define USE_PVRTC 1
#endif
#ifdef GL_EXT_texture_compression_s3tc
#    define USE_DDS 1
#endif

using rainbow::Data;

#ifdef USE_DDS
namespace dds
{
    extern bool check(const Data& data);
}

TEST(DecodersTest, DetectsDDS)
{
    constexpr uint8_t kNotDDSSignature[]{' ', 'S', 'D', 'D'};
    constexpr uint8_t kDDSSignature[]{'D', 'D', 'S', ' '};

    ASSERT_TRUE(dds::check(Data::from_bytes(kDDSSignature)));
    ASSERT_FALSE(dds::check(Data::from_bytes(kNotDDSSignature)));
}
#endif  // USE_DDS

namespace png
{
    extern bool check(const Data& data);
}

TEST(DecodersTest, DetectsPNG)
{
    constexpr uint8_t kNotPNGSignature[]{10, 26, 10, 13, 71, 78, 80, 137};
    constexpr uint8_t kPNGSignature[]{137, 80, 78, 71, 13, 10, 26, 10};

    ASSERT_TRUE(png::check(Data::from_bytes(kPNGSignature)));
    ASSERT_FALSE(png::check(Data::from_bytes(kNotPNGSignature)));
}

#ifdef USE_PVRTC
namespace pvrtc
{
    extern bool check(const Data& data);
}

TEST(DecodersTest, DetectsPVRTC)
{
    constexpr uint8_t kNotPVRTCSignature[]{'3', 'R', 'V', 'P'};
    constexpr uint8_t kPVRTCSignature[]{'P', 'V', 'R', '3'};

    ASSERT_TRUE(pvrtc::check(Data::from_bytes(kPVRTCSignature)));
    ASSERT_FALSE(pvrtc::check(Data::from_bytes(kNotPVRTCSignature)));
}
#endif  // USE_PVRTC

namespace svg
{
    extern bool check(const Data& data);
}

TEST(DecodersTest, DetectsSVG)
{
    constexpr uint8_t kNotSVGSignature[]{'\0', ' ', 'g', 'v', 's', '<'};
    constexpr uint8_t kSVGSignature[]{'<', 's', 'v', 'g', ' ', '\0'};

    ASSERT_TRUE(svg::check(Data::from_bytes(kSVGSignature)));
    ASSERT_FALSE(svg::check(Data::from_bytes(kNotSVGSignature)));
}
