// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Image.h"

#include <gtest/gtest.h>

#include "Common/Algorithm.h"
#include "Common/Data.h"
#include "Resources/Rainbow.svg.h"

using namespace rainbow;

TEST(ImageTest, LoadsPNGs_16BitGrayscale)
{
    constexpr uint8_t kbasn4a08_png[126]{
        137, 80,  78,  71,  13,  10,  26,  10,  0,   0,   0,   13,  73,  72,
        68,  82,  0,   0,   0,   32,  0,   0,   0,   32,  8,   4,   0,   0,
        0,   217, 115, 178, 127, 0,   0,   0,   4,   103, 65,  77,  65,  0,
        1,   134, 160, 49,  232, 150, 95,  0,   0,   0,   53,  73,  68,  65,
        84,  120, 156, 99,  252, 207, 192, 192, 1,   133, 156, 104, 52,  49,
        34,  156, 44,  223, 25,  40,  3,   44,  63,  134, 190, 1,   163, 97,
        48,  26,  6,   84,  49,  96,  52,  16,  71,  195, 128, 42,  6,   140,
        6,   226, 104, 24,  80,  195, 0,   0,   223, 42,  32,  125, 196, 235,
        133, 200, 0,   0,   0,   0,   73,  69,  78,  68,  174, 66,  96,  130,
    };

    auto basn4a08 = Image::decode(
        {kbasn4a08_png, sizeof(kbasn4a08_png), Data::Ownership::Reference},
        1.0f);

    ASSERT_EQ(basn4a08.format, Image::Format::PNG);
    ASSERT_EQ(basn4a08.width, 32u);
    ASSERT_EQ(basn4a08.height, 32u);
    ASSERT_EQ(basn4a08.depth, 16u);
    ASSERT_EQ(basn4a08.channels, 2u);
    ASSERT_EQ(basn4a08.size, basn4a08.width * basn4a08.height * 2u);
}

TEST(ImageTest, LoadsPNGs_32BitColor)
{
    constexpr uint8_t kbasn6a08_png[184]{
        137, 80,  78,  71,  13,  10,  26,  10,  0,   0,   0,   13,  73,  72,
        68,  82,  0,   0,   0,   32,  0,   0,   0,   32,  8,   6,   0,   0,
        0,   115, 122, 122, 244, 0,   0,   0,   4,   103, 65,  77,  65,  0,
        1,   134, 160, 49,  232, 150, 95,  0,   0,   0,   111, 73,  68,  65,
        84,  120, 156, 237, 214, 49,  10,  128, 48,  12,  70,  225, 39,  100,
        104, 79,  161, 247, 63,  85,  4,   143, 33,  196, 221, 197, 69,  120,
        29,  82,  232, 80,  40,  252, 31,  77,  40,  217, 138, 1,   48,  94,
        123, 126, 156, 255, 186, 51,  131, 29,  117, 5,   71,  3,   202, 6,
        168, 249, 13,  88,  160, 7,   78,  53,  30,  34,  125, 128, 92,  130,
        84,  227, 27,  176, 66,  15,  92,  220, 46,  0,   121, 32,  136, 146,
        255, 226, 160, 1,   54,  160, 123, 64,  7,   148, 60,  16,  4,   217,
        0,   25,  80,  54,  64,  127, 1,   27,  240, 0,   82,  32,  26,  156,
        22,  15,  184, 76,  0,   0,   0,   0,   73,  69,  78,  68,  174, 66,
        96,  130,
    };

    auto basn6a08 = Image::decode(
        {kbasn6a08_png, sizeof(kbasn6a08_png), Data::Ownership::Reference},
        1.0f);

    ASSERT_EQ(basn6a08.format, Image::Format::PNG);
    ASSERT_EQ(basn6a08.width, 32u);
    ASSERT_EQ(basn6a08.height, 32u);
    ASSERT_EQ(basn6a08.depth, 32u);
    ASSERT_EQ(basn6a08.channels, 4u);
    ASSERT_EQ(basn6a08.size, basn6a08.width * basn6a08.height * 4u);
}

TEST(ImageTest, LoadsSVGs)
{
    auto svg = Image::decode(
        {assets::kLogo, sizeof(assets::kLogo), Data::Ownership::Reference},
        1.0f);

    ASSERT_EQ(svg.format, Image::Format::SVG);
    ASSERT_EQ(svg.width, rainbow::ceil_pow2(assets::kLogoWidth));
    ASSERT_EQ(svg.height, rainbow::ceil_pow2(assets::kLogoHeight));
    ASSERT_EQ(svg.depth, 32u);
    ASSERT_EQ(svg.channels, 4u);
    ASSERT_EQ(svg.size, svg.width * svg.height * 4u);
}
