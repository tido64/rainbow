// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <cstdio>

#include <physfs.h>

#include "FileSystem/Bundle.h"
#include "FileSystem/FileSystem.h"

namespace rainbow::test
{
    inline auto fixture_path(czstring path)
    {
        filesystem::Path fixture_path{__FILE__};
        fixture_path /= "..";
        fixture_path /= "__fixtures";
        fixture_path /= path;
        return fixture_path.lexically_normal().make_preferred();
    }

    class ScopedAssetsDirectory
    {
    public:
        explicit ScopedAssetsDirectory(czstring path)
            : assets_path_(fixture_path(path))
        {
            PHYSFS_unmount(filesystem::bundle().assets_path());
            PHYSFS_mount(c_str(), nullptr, 0);
            PHYSFS_setWriteDir(c_str());
        }

        ~ScopedAssetsDirectory()
        {
            PHYSFS_setWriteDir(nullptr);
            PHYSFS_unmount(c_str());
        }

        auto c_str() const -> czstring { return assets_path_.c_str(); }
        auto path() const -> const filesystem::Path& { return assets_path_; }

    private:
        filesystem::Path assets_path_;
    };

    class ScopedFile
    {
    public:
        ScopedFile(const std::string& path) : path_(path.c_str())
        {
            FILE* fd = std::fopen(path.c_str(), "wb");
            [](FILE* fd) { ASSERT_NE(nullptr, fd); }(fd);
            std::fwrite(path.data(), 1, path.length(), fd);
            std::fclose(fd);
        }

        ~ScopedFile() { std::remove(path_); }

    private:
        czstring path_;
    };
}  // namespace rainbow::test

namespace rainbow
{
    struct ISolemnlySwearThatIAmOnlyTesting {
    };
}  // namespace rainbow
