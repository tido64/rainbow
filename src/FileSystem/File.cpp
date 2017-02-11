// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
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

using rainbow::File;
using rainbow::czstring;
using rainbow::filesystem::Path;
using rainbow::filesystem::ops::CloseFile;
using rainbow::filesystem::ops::FileSize;
using rainbow::filesystem::ops::IsValidHandle;
using rainbow::filesystem::ops::Read;
using rainbow::filesystem::ops::Seek;
using rainbow::filesystem::ops::Write;

auto File::open(const Path& path) -> File
{
    return File{path};
}

auto File::open_asset(czstring path) -> File
{
    return File{filesystem::relative(path)};
}

auto File::open_document(czstring path) -> File
{
    return {filesystem::user(path), "r+b"};
}

auto File::open_write(czstring path) -> File
{
    return {filesystem::user(path), "wb"};
}

File::~File()
{
    visit(CloseFile{}, handle_);
}

auto File::is_open() const -> bool
{
    return visit(IsValidHandle{}, handle_);
}

auto File::size() const -> size_t
{
    return visit(FileSize{}, handle_);
}

auto File::read(void* dst, size_t size) -> size_t
{
    return visit(Read{dst, size}, handle_);
}

auto File::seek(int64_t offset, int origin) -> int
{
    return visit(Seek{offset, origin}, handle_);
}

auto File::write(const void* buffer, size_t size) -> size_t
{
    return visit(Write{buffer, size}, handle_);
}

File::operator AAsset*() const
{
    return get<AAsset*>(handle_);
}

File::operator FILE*() const
{
    return get<FILE*>(handle_);
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

File::File(const Path& path, czstring mode)
    : handle_(fopen(path.c_str(), mode))
{
}
