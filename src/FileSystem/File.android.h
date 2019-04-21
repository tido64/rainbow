// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef FILESYSTEM_FILE_ANDROID_H_
#define FILESYSTEM_FILE_ANDROID_H_

#include <android/native_activity.h>

#include "Common/String.h"
#include "Common/Variant.h"
#include "FileSystem/File.h"
#include "FileSystem/Path.h"

struct AAsset;
struct PHYSFS_File;

extern ANativeActivity* g_native_activity;

namespace rainbow::android
{
    using FileHandle = variant<std::nullptr_t, AAsset*, PHYSFS_File*>;

    class File
    {
    protected:
        static constexpr bool open(czstring path, FileType type, File& file)
        {
            if (type != FileType::Asset)
                return false;

            auto handle = AAssetManager_open(
                g_native_activity->assetManager, path, AASSET_MODE_UNKNOWN);
            if (handle == nullptr)
                return false;

            file.set_handle(handle);
            return true;
        }

        static auto resolve_path(czstring path, FileType file_type)
        {
            const auto resolved_path = [](czstring path, FileType) {
                return filesystem::Path{path};
            }(path, file_type);

            // Android is very particular about normalised paths.
            return resolved_path.lexically_normal();
        }

        File() : handle_(nullptr) {}

        File(File&& file) noexcept : handle_(file.handle_)
        {
            file.handle_ = nullptr;
        }

        auto handle() const -> PHYSFS_File*
        {
            return get<PHYSFS_File*>(handle_).value();
        }

        template <typename T>
        void set_handle(T stream)
        {
            R_ASSERT(holds_alternative<std::nullptr_t>(handle_),
                     "Should only be set once");

            handle_ = stream;
        }

        auto is_platform_handle() const
        {
            return holds_alternative<AAsset*>(handle_);
        }

        auto size() const -> size_t
        {
            return AAsset_getLength64(get<AAsset*>(handle_).value());
        }

        void close()
        {
            AAsset_close(get<AAsset*>(handle_).value());
            handle_ = nullptr;
        }

        auto read(void* dst, size_t size) const -> size_t
        {
            auto handle = get<AAsset*>(handle_).value();
            return std::max(AAsset_read(handle, dst, size), 0);
        }

        bool seek(int64_t offset, SeekOrigin origin) const
        {
            const int whence = detail::seek_origin(origin);
            auto handle = get<AAsset*>(handle_).value();
            return AAsset_seek64(handle, offset, whence) >= 0;
        }

        auto tell() const -> size_t
        {
            auto handle = get<AAsset*>(handle_).value();
            return AAsset_getLength64(handle) -
                   AAsset_getRemainingLength64(handle);
        }

        explicit operator bool() const
        {
            return visit(
                [](auto&& handle) { return handle != nullptr; }, handle_);
        }

    private:
        FileHandle handle_;
    };
}  // namespace rainbow::android

namespace rainbow
{
    using File = TFile<android::File>;
}

#endif
