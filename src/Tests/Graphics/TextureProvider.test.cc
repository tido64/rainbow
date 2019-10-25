// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Texture.h"

#include <string_view>

#include <gtest/gtest.h>

#include "Common/Data.h"
#include "Tests/TestHelpers.h"

using namespace rainbow::graphics;
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
        auto tex1_1_raw = provider.raw_get(tex1_1);
        ASSERT_EQ(tex1_1_raw.use_count, 1U);

        auto tex1_2 = provider.get(kTextureID, Data{});
        auto tex1_2_raw = provider.raw_get(tex1_2);
        ASSERT_EQ(allocator.current_id, 1);
        ASSERT_EQ(tex1_2_raw.data[0], tex1_1_raw.data[0]);
        ASSERT_EQ(tex1_2_raw.use_count, 2U);

        auto tex2 = provider.get(kTextureID2, mock_image);
        auto tex2_raw = provider.raw_get(tex2);
        ASSERT_EQ(allocator.current_id, 2);
        ASSERT_GT(tex2_raw.data[0], tex1_1_raw.data[0]);
        ASSERT_EQ(tex2_raw.use_count, 1U);

        provider.release(tex1_2);
        auto tex1_3 = provider.get(kTextureID, Data{});
        auto tex1_3_raw = provider.raw_get(tex1_3);
        ASSERT_EQ(allocator.current_id, 2);
        ASSERT_EQ(tex1_3_raw.data[0], tex1_2_raw.data[0]);
        ASSERT_EQ(tex1_3_raw.use_count, 2U);

        provider.release(tex1_3);
        provider.release(tex1_1);
        auto tex1_4 = provider.get(kTextureID, mock_image);
        auto tex1_4_raw = provider.raw_get(tex1_4);
        ASSERT_EQ(allocator.current_id, 3);
        ASSERT_NE(tex1_4_raw.data[0], tex1_3_raw.data[0]);
        ASSERT_EQ(tex1_4_raw.use_count, 1U);
    }

    ASSERT_EQ(allocator.released, allocator.current_id);
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
