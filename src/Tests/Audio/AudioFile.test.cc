// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Audio/AudioFile.h"

#include <gtest/gtest.h>

#include "Platform/Macros.h"
#include "Tests/TestHelpers.h"

using rainbow::audio::IAudioFile;
using rainbow::test::ScopedAssetsDirectory;

TEST(AudioFileTest, HandlesNonExistingFiles)
{
    ScopedAssetsDirectory scoped_assets{"AudioTest"};

    auto file = IAudioFile::open("test.audio");

    ASSERT_FALSE(*file);
    ASSERT_EQ(file->channels(), 0);
    ASSERT_EQ(file->rate(), 0);
    ASSERT_EQ(file->size(), 0u);
}

#if !defined(RAINBOW_OS_IOS)
TEST(AudioFileTest, LoadsOggVorbis)
{
    ScopedAssetsDirectory scoped_assets{"AudioTest"};

    auto file = IAudioFile::open("test.ogg");

    ASSERT_TRUE(*file);
    ASSERT_EQ(file->channels(), 2);
    ASSERT_EQ(file->rate(), 44100);
    ASSERT_EQ(file->size(), 17640u);
}
#endif  // !RAINBOW_OS_IOS
