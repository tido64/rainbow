// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef FILESYSTEM_FILE_SYSTEM_H_
#define FILESYSTEM_FILE_SYSTEM_H_

#include "FileSystem/File.h"
#include "FileSystem/Path.h"

struct PHYSFS_File;

namespace rainbow::system
{
    class File
    {
    protected:
        static constexpr auto is_platform_handle() { return false; }
        static constexpr auto open(czstring, FileType, File&) { return false; }

        static auto resolve_path(czstring path, FileType)
        {
            return filesystem::Path{path};
        }

        constexpr File() = default;

        File(File&& file) noexcept : stream_(file.stream_)
        {
            file.stream_ = nullptr;
        }

        [[nodiscard]] auto handle() const { return stream_; }

        void set_handle(PHYSFS_File* stream)
        {
            R_ASSERT(stream_ == nullptr, "Set only once");
            stream_ = stream;
        }

        [[nodiscard]] constexpr auto size() const -> size_t { return 0; }

        void close() {}
        constexpr auto read(void*, size_t) const -> size_t { return 0; }
        constexpr auto seek(int64_t, SeekOrigin) const -> bool { return false; }
        [[nodiscard]] constexpr auto tell() const -> size_t { return 0; }
        constexpr auto write(const void*, size_t) const -> size_t { return 0; }

        explicit operator bool() const { return stream_ != nullptr; }

    private:
        PHYSFS_File* stream_ = nullptr;
    };
}  // namespace rainbow::system

namespace rainbow
{
    using WriteableFile = TWriteableFile<system::File>;
}  // namespace rainbow

#endif
