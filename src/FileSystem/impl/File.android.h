// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef FILESYSTEM_IMPL_FILE_ANDROID_H_
#define FILESYSTEM_IMPL_FILE_ANDROID_H_

#include <android/native_activity.h>

#include "Common/String.h"
#include "Common/Variant.h"
#include "FileSystem/File.h"
#include "FileSystem/FileSystem.h"

#define USE_PLATFORM_FILE 1

struct AAsset;

extern ANativeActivity* g_native_activity;

namespace rainbow { namespace android
{
    using FileHandle = variant<std::nullptr_t, AAsset*, FILE*>;

    class File
    {
    protected:
        static constexpr bool open(czstring path, FileType type, File& file)
        {
            if (type != FileType::Asset)
                return false;

            auto handle = AAssetManager_open(
                g_native_activity->assetManager, path, AASSET_MODE_UNKNOWN);
            file.set_handle(handle);
            return true;
        }

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

        auto handle() const -> FILE* { return get<FILE*>(handle_); }

        template <typename T>
        void set_handle(T stream)
        {
            R_ASSERT(holds_alternative<nullptr_t>(handle_),
                     "Should only be set only once");

            handle_ = stream;
        }

        auto is_platform_handle() const
        {
            return holds_alternative<AAsset*>(handle_);
        }

        auto size() const -> size_t
        {
            return AAsset_getLength64(get<AAsset*>(handle_));
        }

        void close()
        {
            AAsset_close(get<AAsset*>(handle_));
            handle_ = nullptr;
        }

        auto read(void* dst, size_t size) const -> size_t
        {
            return std::max(AAsset_read(get<AAsset*>(handle_), dst, size), 0);
        }

        bool seek(int64_t offset, SeekOrigin origin) const
        {
            const int whence = detail::seek_origin(origin);
            return AAsset_seek64(get<AAsset*>(handle_), offset, whence) >= 0;
        }

        explicit operator bool() const
        {
            return handle_ != nullptr &&
                   handle_ != static_cast<AAsset*>(nullptr) &&
                   handle_ != static_cast<FILE*>(nullptr);
        }

    private:
        FileHandle handle_;
    };
}}  // namespace rainbow::android

namespace rainbow
{
    using File = TFile<android::File>;
}

#endif
