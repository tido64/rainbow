// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Texture.h"

#include <string_view>

#include <gtest/gtest.h>

#include "Common/Data.h"
#include "Graphics/Image.h"
#include "Tests/TestHelpers.h"
#include "Tests/__fixtures/ImageTest/Images.h"

using namespace rainbow::graphics;
using namespace rainbow::test;
using namespace std::literals::string_view_literals;

using rainbow::Data;
using rainbow::Image;

namespace
{
    constexpr const char kMockImageData[] = "RNBWMOCK";  // NOLINT

    struct MockTextureAllocator final : public ITextureAllocator
    {
        int current_id = 0;  // NOLINT
        int released = 0;    // NOLINT
        int updated = 0;     // NOLINT

        void construct(TextureHandle& handle,
                       const Image&,
                       Filter,
                       Filter) override
        {
            handle[0] = ++current_id;
        }

        void destroy(TextureHandle&) override { ++released; }

        [[maybe_unused, nodiscard]]
        auto max_size() const noexcept -> size_t override
        {
            return sizeof(current_id);
        }

        void update(const TextureHandle&, const Image&, Filter, Filter) override
        {
            ++updated;
        }
    };
}  // namespace

TEST(TextureProviderTest, ReferenceCounts)
{
    constexpr auto kTextureID = "test"sv;
    constexpr auto kTextureID2 = "test2"sv;

    MockTextureAllocator allocator;
    {
        auto mock_image = Data::from_literal(kMockImageData);
        TextureProvider provider{allocator};

        auto tex1_1 = provider.get(kTextureID, mock_image);
        ASSERT_TRUE(tex1_1);

        auto tex1_1_raw = provider.raw_get(tex1_1);
        ASSERT_EQ(tex1_1_raw.use_count, 1U);

        auto tex1_2 = provider.get(kTextureID, Data{});
        ASSERT_TRUE(tex1_2);

        auto tex1_2_raw = provider.raw_get(tex1_2);
        ASSERT_EQ(allocator.current_id, 1);
        ASSERT_EQ(tex1_2_raw.data[0], tex1_1_raw.data[0]);
        ASSERT_EQ(tex1_2_raw.use_count, 2U);

        auto tex2 = provider.get(kTextureID2, mock_image);
        ASSERT_TRUE(tex2);

        auto tex2_raw = provider.raw_get(tex2);
        ASSERT_EQ(allocator.current_id, 2);
        ASSERT_GT(tex2_raw.data[0], tex1_1_raw.data[0]);
        ASSERT_EQ(tex2_raw.use_count, 1U);

        provider.release(tex1_2);
        auto tex1_3 = provider.get(kTextureID, Data{});
        ASSERT_TRUE(tex1_3);

        auto tex1_3_raw = provider.raw_get(tex1_3);
        ASSERT_EQ(allocator.current_id, 2);
        ASSERT_EQ(tex1_3_raw.data[0], tex1_2_raw.data[0]);
        ASSERT_EQ(tex1_3_raw.use_count, 2U);

        provider.release(tex1_3);
        provider.release(tex1_1);
        auto tex1_4 = provider.get(kTextureID, mock_image);
        ASSERT_TRUE(tex1_4);

        auto tex1_4_raw = provider.raw_get(tex1_4);
        ASSERT_EQ(allocator.current_id, 3);
        ASSERT_NE(tex1_4_raw.data[0], tex1_3_raw.data[0]);
        ASSERT_EQ(tex1_4_raw.use_count, 1U);

        Texture tex1_5;
        ASSERT_FALSE(tex1_5);

        tex1_5 = tex1_4;
        ASSERT_TRUE(tex1_5);
        ASSERT_EQ(allocator.current_id, 3);

        auto tex1_5_raw = provider.raw_get(tex1_5);
        ASSERT_EQ(tex1_5_raw.data[0], tex1_4_raw.data[0]);
        ASSERT_EQ(tex1_5_raw.use_count, 2U);

        auto tex1_6 = std::move(tex1_5);
        ASSERT_FALSE(tex1_5);  // NOLINT(bugprone-use-after-move)
        ASSERT_TRUE(tex1_6);
        ASSERT_EQ(allocator.current_id, 3);

        auto tex1_6_raw = provider.raw_get(tex1_6);
        ASSERT_EQ(tex1_6_raw.data[0], tex1_4_raw.data[0]);
        ASSERT_EQ(tex1_6_raw.use_count, 2U);
    }

    ASSERT_EQ(allocator.released, allocator.current_id);
}

TEST(TextureProviderTest, GetsPreloadedTexture)
{
    MockTextureAllocator allocator;
    TextureProvider provider{allocator};

    auto image = Image::decode({fixtures::basn6a08_png.data(),
                                fixtures::basn6a08_png.size(),
                                Data::Ownership::Reference},
                               1.0F);
    auto texture = provider.get("rainbow://assets/basn6a08.png", image);
    ASSERT_TRUE(texture);
}

TEST(TextureProviderTest, ReleasesNothing)
{
    MockTextureAllocator allocator;
    TextureProvider provider{allocator};
    rainbow::ISolemnlySwearThatIAmOnlyTesting contract{};
    provider.release(Texture{"", contract});
    provider.release(Texture{"test", contract});
}

TEST(TextureProviderTest, TryGetDoesNotConstruct)
{
    constexpr auto kTextureID = "test"sv;

    MockTextureAllocator allocator;
    TextureProvider provider{allocator};

    ASSERT_EQ(allocator.current_id, 0);

    if (auto optional_tex = provider.try_get(Texture{}))
        FAIL();

    ASSERT_EQ(allocator.current_id, 0);

    auto tex = provider.get(kTextureID, Data::from_literal(kMockImageData));
    ASSERT_TRUE(tex);
    ASSERT_EQ(allocator.current_id, 1);

    if (auto optional_tex = provider.try_get(tex))
    {
        ASSERT_EQ(allocator.current_id, 1);
        ASSERT_EQ(optional_tex->data[0], provider.raw_get(tex).data[0]);
        ASSERT_EQ(optional_tex->use_count, 2U);
    }
    else
    {
        FAIL();
    }
}

TEST(TextureProviderTest, UpdatesExistingTexture)
{
    MockTextureAllocator allocator;
    TextureProvider provider{allocator};

    auto mock_image = Data::from_literal(kMockImageData);
    auto texture = provider.get("test", mock_image);
    ASSERT_TRUE(texture);
    ASSERT_EQ(allocator.updated, 0);

    auto image = Image::decode({fixtures::basn6a08_png.data(),
                                fixtures::basn6a08_png.size(),
                                Data::Ownership::Reference},
                               1.0F);
    provider.update(texture, image);
    ASSERT_EQ(allocator.updated, 1);
}

TEST(TextureProviderTest, ReleasesPreviousTextureWhenAssigned)
{
    MockTextureAllocator allocator;
    TextureProvider provider{allocator};

    {
        auto mock_image = Data::from_literal(kMockImageData);
        auto texture = provider.get("test", mock_image);
        ASSERT_TRUE(texture);
        ASSERT_EQ(allocator.current_id, 1);
        ASSERT_EQ(allocator.released, 0);

        texture = provider.get("test2", mock_image);
        ASSERT_TRUE(texture);
        ASSERT_EQ(allocator.current_id, 2);
        ASSERT_EQ(allocator.released, 1);
    }

    ASSERT_EQ(allocator.current_id, 2);
    ASSERT_EQ(allocator.released, 2);
}

TEST(TextureProviderTest, TextureHandlesAreMovable)
{
    MockTextureAllocator allocator;
    TextureProvider provider{allocator};

    {
        auto mock_image = Data::from_literal(kMockImageData);
        auto texture = provider.get("test", mock_image);
        ASSERT_TRUE(texture);
        ASSERT_EQ(allocator.current_id, 1);
        ASSERT_EQ(allocator.released, 0);

        auto texture2 = std::move(texture);
        ASSERT_TRUE(texture2);
        ASSERT_FALSE(texture);
        ASSERT_EQ(allocator.current_id, 1);
        ASSERT_EQ(allocator.released, 0);
    }

    ASSERT_EQ(allocator.current_id, 1);
    ASSERT_EQ(allocator.released, 1);
}
