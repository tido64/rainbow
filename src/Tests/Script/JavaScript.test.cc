// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <climits>

#include <gtest/gtest.h>

#include "Common/Constants.h"
#include "Script/JavaScript/Helper.h"
#include "Script/JavaScript/JavaScript.h"

namespace
{
    namespace duk = rainbow::duk;

    constexpr char kRainbowKey[] = "rainbow";

    class JavaScriptTest : public ::testing::Test
    {
    protected:
        duk::Context context_;
    };
}  // namespace

TEST_F(JavaScriptTest, ReturnsBufferObjectType)
{
    ASSERT_EQ(static_cast<duk_uint_t>(DUK_BUFOBJ_DATAVIEW),
              duk::buffer_object_type<int64_t>());

    ASSERT_EQ(static_cast<duk_uint_t>(DUK_BUFOBJ_FLOAT32ARRAY),
              duk::buffer_object_type<float>());

    ASSERT_EQ(static_cast<duk_uint_t>(DUK_BUFOBJ_FLOAT64ARRAY),
              duk::buffer_object_type<double>());

    ASSERT_EQ(static_cast<duk_uint_t>(DUK_BUFOBJ_INT16ARRAY),
              duk::buffer_object_type<int16_t>());

    ASSERT_EQ(static_cast<duk_uint_t>(DUK_BUFOBJ_INT32ARRAY),
              duk::buffer_object_type<int32_t>());

    ASSERT_EQ(static_cast<duk_uint_t>(DUK_BUFOBJ_INT8ARRAY),
              duk::buffer_object_type<int8_t>());
}

TEST_F(JavaScriptTest, SetsAndGetsArguments)
{
    duk::push(context_,
              true,
              std::numeric_limits<uint8_t>::max(),
              std::numeric_limits<uint32_t>::max(),
              std::numeric_limits<uint64_t>::max(),
              -1,
              rainbow::kPi<float>,
              kRainbowKey,
              &context_);
    auto args = duk::get_args<bool,
                              uint32_t,
                              uint32_t,
                              uint32_t,
                              int32_t,
                              float,
                              const char*>(context_);

    ASSERT_EQ(true, std::get<0>(args));
    ASSERT_EQ(std::numeric_limits<uint8_t>::max(), std::get<1>(args));
    ASSERT_EQ(std::numeric_limits<uint32_t>::max(), std::get<2>(args));
    ASSERT_EQ(std::numeric_limits<uint32_t>::max(), std::get<3>(args));
    ASSERT_EQ(-1, std::get<4>(args));
    ASSERT_FLOAT_EQ(rainbow::kPi<float>, std::get<5>(args));
    ASSERT_STREQ(kRainbowKey, std::get<6>(args));
    ASSERT_EQ(&context_, duk_require_pointer(context_, 7));
}

TEST_F(JavaScriptTest, SetsAndGetsAudioHandlers)
{
    const auto expected_channel =
        reinterpret_cast<rainbow::audio::Channel*>(0x10101010);
    const auto expected_sound =
        reinterpret_cast<rainbow::audio::Sound*>(0x10101010);

    duk::push(context_, expected_sound);
    duk::push(context_, expected_channel);

    ASSERT_EQ(expected_sound, duk::get<rainbow::audio::Sound*>(context_, 0));
    ASSERT_EQ(
        expected_channel, duk::get<rainbow::audio::Channel*>(context_, 1));
}

TEST_F(JavaScriptTest, SetsAndGetsColor)
{
    constexpr rainbow::Color expected{0xff, 0, 0xff};
    duk::push(context_, expected);

    ASSERT_EQ(expected, duk::get<rainbow::Color>(context_, 0));
}

TEST_F(JavaScriptTest, SetsAndGetsProperties)
{
    constexpr char kDoubleRainbow[] = "🌈🌈";

    auto obj_idx = duk_push_bare_object(context_);
    duk::push_literal(context_, kDoubleRainbow);
    duk::put_prop_literal(context_, obj_idx, kRainbowKey);

    ASSERT_TRUE(duk::has_prop_literal(context_, obj_idx, kRainbowKey));

    duk::get_prop_literal(context_, obj_idx, kRainbowKey);

    ASSERT_STREQ(kDoubleRainbow, duk::get<const char*>(context_, -1));
}

TEST_F(JavaScriptTest, SetsAndGetsVec2)
{
    constexpr rainbow::Vec2f expected{2, 7};
    duk::push(context_, expected);

    ASSERT_EQ(expected, duk::get<rainbow::Vec2f>(context_, 0));
}
