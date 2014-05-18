// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "FileSystem/File.h"

#include <sys/stat.h>

#include "Platform/Macros.h"
#ifdef RAINBOW_OS_ANDROID
#	include <android/native_activity.h>
#endif

#include "Common/Debug.h"
#include "FileSystem/Path.h"

#if defined(RAINBOW_OS_ANDROID)
extern ANativeActivity *gNativeActivity;
#elif defined(RAINBOW_OS_WINDOWS)
#	define fileno _fileno
#endif

File File::open(const char *const path)
{
	return File(path);
}

File File::open_asset(const char *const path)
{
	return File(Path(path, Path::RelativeTo::CurrentPath));
}

File File::open_document(const char *const path)
{
	return File(Path(path, Path::RelativeTo::UserDataPath), "r+b");
}

File File::open_write(const char *const path)
{
	return File(Path(path, Path::RelativeTo::UserDataPath), "wb");
}

File::File(File &&f) : is_asset_(f.is_asset_), stream_(f.stream_)
{
	f.stream_ = nullptr;
}

File::~File()
{
	if (!stream_)
		return;

#if defined(RAINBOW_OS_ANDROID)
	if (is_asset_)
		AAsset_close(asset_);
	else
#endif
		fclose(stream_);
}

size_t File::size() const
{
#ifdef RAINBOW_OS_ANDROID
	if (is_asset_)
		return AAsset_getLength(asset_);
#endif
	const int fd = fileno(stream_);
	struct stat file_status;
	return (fstat(fd, &file_status) != 0) ? 0 : file_status.st_size;
}

size_t File::read(void *dst, const size_t size) const
{
#ifdef RAINBOW_OS_ANDROID
	if (is_asset_)
		return AAsset_read(asset_, dst, size);
#endif
	return fread(dst, sizeof(char), size, stream_);
}

int File::seek(const long offset, const int origin) const
{
	return fseek(stream_, offset, origin);
}

size_t File::write(const void *buffer, const size_t size) const
{
	return fwrite(buffer, sizeof(char), size, stream_);
}

File::File(const char *const path) : is_asset_(true), stream_(nullptr)
{
	if (!path || path[0] == '\0')
		return;

#ifdef RAINBOW_OS_ANDROID
	asset_ = AAssetManager_open(
	    gNativeActivity->assetManager, path, AASSET_MODE_UNKNOWN);
#else
	stream_ = fopen(path, "rb");
#endif
	if (!stream_)
		R_ERROR("[Rainbow] File: Failed to open '%s'\n", path);
}

File::File(const char *const path, const char *const mode)
    : is_asset_(false), stream_(nullptr)
{
	stream_ = fopen(path, mode);
}
