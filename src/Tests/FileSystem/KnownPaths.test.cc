// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/String.h"
#include "FileSystem/KnownPaths.h"
#include "Platform/Macros.h"
#include "Tests/TestHelpers.h"

using rainbow::ISolemnlySwearThatIAmOnlyTesting;
using rainbow::strllen;

#if defined(RAINBOW_OS_WINDOWS)
#   include <ShlObj.h>
#   include <Shlwapi.h>

namespace
{
    constexpr char kDocumentsFolder[] = "\\Documents";
    constexpr size_t kDocumentsPathLength = strllen(kDocumentsFolder);

    constexpr char kSavedGamesFolder[] = "\\Saved Games";
    constexpr size_t kSavedGamesPathLength = strllen(kSavedGamesFolder);
}

namespace rainbow::filesystem
{
    auto test_get_known_folder_path(REFKNOWNFOLDERID fid,
                                    const ISolemnlySwearThatIAmOnlyTesting&)
        -> std::string;
}  // namespace rainbow::filesystem

TEST(KnownPathsTest, GetsDocumentsFolderPath)
{
    const std::string path = rainbow::filesystem::test_get_known_folder_path(
        FOLDERID_Documents, ISolemnlySwearThatIAmOnlyTesting{});

    ASSERT_LT(kDocumentsPathLength, path.length());
    ASSERT_EQ(
        kDocumentsFolder, path.substr(path.length() - kDocumentsPathLength));
}

TEST(KnownPathsTest, GetsDocumentsLibraryPath)
{
    const std::string path = rainbow::filesystem::test_get_known_folder_path(
        FOLDERID_DocumentsLibrary, ISolemnlySwearThatIAmOnlyTesting{});

    ASSERT_LT(kDocumentsPathLength, path.length());
    ASSERT_EQ(
        kDocumentsFolder, path.substr(path.length() - kDocumentsPathLength));
}

TEST(KnownPathsTest, GetsSaveDirectory)
{
    static_assert(kDocumentsPathLength != kSavedGamesPathLength,
                  "This test assumes the lengths of these paths differ");

    const std::string save_path = rainbow::filesystem::get_save_directory();
    const std::string save_dir =
        save_path.substr(save_path.length() - strllen(kSavedGamesFolder));

    if (save_dir[0] == '\\')
        ASSERT_EQ(kSavedGamesFolder, save_dir);
    else
        ASSERT_EQ(kDocumentsFolder, save_dir);
}

TEST(KnownPathsTest, GetsSavedGamesFolderPath)
{
    const std::string path = rainbow::filesystem::test_get_known_folder_path(
        FOLDERID_SavedGames, ISolemnlySwearThatIAmOnlyTesting{});

    ASSERT_LT(kSavedGamesPathLength, path.length());
    ASSERT_EQ(
        kSavedGamesFolder, path.substr(path.length() - kSavedGamesPathLength));
}
#endif
