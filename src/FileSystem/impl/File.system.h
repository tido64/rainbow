// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef FILESYSTEM_IMPL_FILE_SYSTEM_H_
#define FILESYSTEM_IMPL_FILE_SYSTEM_H_

#include "FileSystem/File.h"
#include "FileSystem/FileSystem.h"

namespace rainbow { namespace system
{
    class File
    {
    protected:
        static constexpr bool open(czstring, FileType, File&) { return false; }

        static auto resolve_path(czstring path, FileType file_type)
        {
            switch (file_type)
            {
                case FileType::UserAsset:
                case FileType::UserFile:
                    return filesystem::user(path);
                default:
                    return filesystem::relative(path);
            }
        }

        constexpr File() : stream_(nullptr) {}

        File(File&& file) noexcept : stream_(file.stream_)
        {
            file.stream_ = nullptr;
        }

        auto handle() const { return stream_; }

        void set_handle(FILE* stream)
        {
            R_ASSERT(stream_ == nullptr, "Set only once");
            stream_ = stream;
        }

        constexpr auto is_platform_handle() const { return false; }
        constexpr auto size() const -> size_t { return 0; }

        void close() {}
        constexpr auto read(void*, size_t) const -> size_t { return 0; }
        constexpr bool seek(int64_t, SeekOrigin) const { return false; }
        constexpr auto write(const void*, size_t) const -> size_t { return 0; }

        explicit operator bool() const { return stream_ != nullptr; }

    private:
        FILE* stream_;
    };
}}  // namespace rainbow::system

namespace rainbow
{
#ifndef USE_PLATFORM_FILE
    using File = TFile<system::File>;
#endif
    using WriteableFile = TWriteableFile<system::File>;
}

#endif
