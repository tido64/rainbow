// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "FileSystem/File.h"

#include "Common/Logging.h"
#include "FileSystem/FileOps/CloseFile.h"
#include "FileSystem/FileOps/FileSize.h"
#include "FileSystem/FileOps/IsValidHandle.h"
#include "FileSystem/FileOps/Read.h"
#include "FileSystem/FileOps/Seek.h"
#include "FileSystem/FileOps/Write.h"
#include "FileSystem/FileSystem.h"

#if defined(RAINBOW_OS_ANDROID)
extern ANativeActivity* g_native_activity;
#endif

using rainbow::filesystem::Path;
using rainbow::filesystem::ops::CloseFile;
using rainbow::filesystem::ops::FileSize;
using rainbow::filesystem::ops::IsValidHandle;
using rainbow::filesystem::ops::Read;
using rainbow::filesystem::ops::Seek;
using rainbow::filesystem::ops::Write;

namespace
{
    template <typename T, typename V>
    auto variant_get(const V& variant)
    {
        return !variant.template is<T>() ? T{}
                                         : variant.template get_unchecked<T>();
    }
}

auto File::open(const Path& path) -> File
{
    return File{path};
}

auto File::open_asset(const char* path) -> File
{
    return File{rainbow::filesystem::relative(path)};
}

auto File::open_document(const char* path) -> File
{
    return {rainbow::filesystem::user(path), "r+b"};
}

auto File::open_write(const char* path) -> File
{
    return {rainbow::filesystem::user(path), "wb"};
}

File::~File()
{
    mapbox::util::apply_visitor(CloseFile{}, handle_);
}

auto File::is_open() const -> bool
{
    return mapbox::util::apply_visitor(IsValidHandle{}, handle_);
}

auto File::size() const -> size_t
{
    return mapbox::util::apply_visitor(FileSize{}, handle_);
}

auto File::read(void* dst, size_t size) -> size_t
{
    return mapbox::util::apply_visitor(Read{dst, size}, handle_);
}

auto File::seek(int64_t offset, int origin) -> int
{
    return mapbox::util::apply_visitor(Seek{offset, origin}, handle_);
}

auto File::write(const void* buffer, size_t size) -> size_t
{
    return mapbox::util::apply_visitor(Write{buffer, size}, handle_);
}

File::operator AAsset*() const
{
    return variant_get<AAsset*>(handle_);
}

File::operator FILE*() const
{
    return variant_get<FILE*>(handle_);
}

File::File(const Path& path)
{
    if (path.empty())
        return;

#ifdef RAINBOW_OS_ANDROID
    handle_ = AAssetManager_open(
        g_native_activity->assetManager, path.c_str(), AASSET_MODE_UNKNOWN);
#else
    handle_ = fopen(path.c_str(), "rb");
#endif
    if (!is_open())
    {
        handle_ = nullptr;
        LOGE("File: Failed to open '%s'", path.c_str());
    }
}

File::File(const Path& path, const char* mode)
    : handle_(fopen(path.c_str(), mode))
{
}
