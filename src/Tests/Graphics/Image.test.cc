// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Image.h"

#include <gtest/gtest.h>

#include "Common/Algorithm.h"
#include "Common/Data.h"
#include "Graphics/OpenGL.h"
#include "Resources/Rainbow.svg.h"
#include "Tests/__fixtures/ImageTest/Images.h"

using namespace rainbow;
using namespace rainbow::test;

#if defined(GL_EXT_texture_compression_s3tc) || defined(RAINBOW_OS_WINDOWS)
TEST(ImageTest, LoadsDDSs_BC1)
{
    auto image = Image::decode({fixtures::basn6a08_bc1_dds.data(),
                                fixtures::basn6a08_bc1_dds.size(),
                                Data::Ownership::Reference},
                               1.0F);

    ASSERT_EQ(image.format, Image::Format::BC1);
    ASSERT_EQ(image.width, 32U);
    ASSERT_EQ(image.height, 32U);
    ASSERT_EQ(image.depth, 1U);
    ASSERT_EQ(image.channels, 3U);
    ASSERT_EQ(image.size, floor_pow2(fixtures::basn6a08_bc1_dds.size()));
}

TEST(ImageTest, LoadsDDSs_BC2)
{
    auto image = Image::decode({fixtures::basn6a08_bc2_dds.data(),
                                fixtures::basn6a08_bc2_dds.size(),
                                Data::Ownership::Reference},
                               1.0F);

    ASSERT_EQ(image.format, Image::Format::BC2);
    ASSERT_EQ(image.width, 32U);
    ASSERT_EQ(image.height, 32U);
    ASSERT_EQ(image.depth, 1U);
    ASSERT_EQ(image.channels, 4U);
    ASSERT_EQ(image.size, floor_pow2(fixtures::basn6a08_bc2_dds.size()));
}

TEST(ImageTest, LoadsDDSs_BC3)
{
    auto image = Image::decode({fixtures::basn6a08_bc3_dds.data(),
                                fixtures::basn6a08_bc3_dds.size(),
                                Data::Ownership::Reference},
                               1.0F);

    ASSERT_EQ(image.format, Image::Format::BC3);
    ASSERT_EQ(image.width, 32U);
    ASSERT_EQ(image.height, 32U);
    ASSERT_EQ(image.depth, 1U);
    ASSERT_EQ(image.channels, 4U);
    ASSERT_EQ(image.size, floor_pow2(fixtures::basn6a08_bc3_dds.size()));
}
#endif  // GL_EXT_texture_compression_s3tc || RAINBOW_OS_WINDOWS

TEST(ImageTest, LoadsPNGs_16BitGrayscale)
{
    auto image = Image::decode({fixtures::basn4a08_png.data(),
                                fixtures::basn4a08_png.size(),
                                Data::Ownership::Reference},
                               1.0F);

    ASSERT_EQ(image.format, Image::Format::PNG);
    ASSERT_EQ(image.width, 32U);
    ASSERT_EQ(image.height, 32U);
    ASSERT_EQ(image.depth, 16U);
    ASSERT_EQ(image.channels, 2U);
    ASSERT_EQ(image.size, image.width * image.height * 2U);
}

TEST(ImageTest, LoadsPNGs_32BitColor)
{
    auto image = Image::decode({fixtures::basn6a08_png.data(),
                                fixtures::basn6a08_png.size(),
                                Data::Ownership::Reference},
                               1.0F);

    ASSERT_EQ(image.format, Image::Format::PNG);
    ASSERT_EQ(image.width, 32U);
    ASSERT_EQ(image.height, 32U);
    ASSERT_EQ(image.depth, 32U);
    ASSERT_EQ(image.channels, 4U);
    ASSERT_EQ(image.size, image.width * image.height * 4U);
}

TEST(ImageTest, LoadsSVGs)
{
    auto image = Image::decode(
        {assets::kLogo, sizeof(assets::kLogo), Data::Ownership::Reference},
        1.0F);

    ASSERT_EQ(image.format, Image::Format::SVG);
    ASSERT_EQ(image.width, rainbow::ceil_pow2(assets::kLogoWidth));
    ASSERT_EQ(image.height, rainbow::ceil_pow2(assets::kLogoHeight));
    ASSERT_EQ(image.depth, 32U);
    ASSERT_EQ(image.channels, 4U);
    ASSERT_EQ(image.size, image.width * image.height * 4U);
}
