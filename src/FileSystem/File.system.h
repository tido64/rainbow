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
        static constexpr bool open(czstring, FileType, File&) { return false; }

        static auto resolve_path(czstring path, FileType)
        {
            return filesystem::Path{path};
        }

        constexpr File() : stream_(nullptr) {}

        File(File&& file) noexcept : stream_(file.stream_)
        {
            file.stream_ = nullptr;
        }

        auto handle() const { return stream_; }

        void set_handle(PHYSFS_File* stream)
        {
            R_ASSERT(stream_ == nullptr, "Set only once");
            stream_ = stream;
        }

        constexpr auto size() const -> size_t { return 0; }

        void close() {}
        constexpr auto read(void*, size_t) const -> size_t { return 0; }
        constexpr bool seek(int64_t, SeekOrigin) const { return false; }
        constexpr auto tell() const -> size_t { return 0; }
        constexpr auto write(const void*, size_t) const -> size_t { return 0; }

        explicit operator bool() const { return stream_ != nullptr; }

    private:
        PHYSFS_File* stream_;
    };
}  // namespace rainbow::system

namespace rainbow
{
    using WriteableFile = TWriteableFile<system::File>;
}  // namespace rainbow

#endif
