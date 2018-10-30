// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <cstdio>
#include <type_traits>
#include <utility>

#include "FileSystem/FileSystem.h"

namespace rainbow::test
{
    class ScopedDirectory
    {
    public:
        ScopedDirectory(const std::string& path) : path_(path.c_str())
        {
            std::error_code error;
            filesystem::create_directories(path.c_str(), std::ref(error));
        }

        ~ScopedDirectory()
        {
            std::error_code error;
            filesystem::remove(path_, error);
        }

    private:
        czstring path_;
    };

    class ScopedFile
    {
    public:
        ScopedFile(const std::string& path) : path_(path.c_str())
        {
            FILE* fd = fopen(path.c_str(), "wb");
            [fd] { ASSERT_NE(nullptr, fd); }();
            fwrite(path.data(), 1, path.length(), fd);
            fclose(fd);
        }

        ~ScopedFile()
        {
            std::error_code error;
            filesystem::remove(path_, std::ref(error));
        }

    private:
        czstring path_;
    };
}  // namespace rainbow::test

namespace rainbow
{
    struct ISolemnlySwearThatIAmOnlyTesting
    {
    };
}  // namespace rainbow
