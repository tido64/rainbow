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

File::File(File &&f) : is_asset(f.is_asset), stream(f.stream)
{
	f.stream = nullptr;
}

File::~File()
{
	if (!this->stream)
		return;

#if defined(RAINBOW_OS_ANDROID)
	if (this->is_asset)
		AAsset_close(this->asset);
	else
#endif
		fclose(this->stream);
}

size_t File::size() const
{
#ifdef RAINBOW_OS_ANDROID
	if (this->is_asset)
		return AAsset_getLength(this->asset);
#endif
	const int fd = fileno(this->stream);
	struct stat file_status;
	return (fstat(fd, &file_status) != 0) ? 0 : file_status.st_size;
}

size_t File::read(void *dst, const size_t size) const
{
#ifdef RAINBOW_OS_ANDROID
	if (this->is_asset)
		return AAsset_read(this->asset, dst, size);
#endif
	return fread(dst, sizeof(char), size, this->stream);
}

int File::seek(const long offset, const int origin) const
{
	return fseek(this->stream, offset, origin);
}

size_t File::write(const void *buffer, const size_t size) const
{
	return fwrite(buffer, sizeof(char), size, this->stream);
}

File::File(const char *const path) : is_asset(true), stream(nullptr)
{
	if (!path || path[0] == '\0')
		return;

#ifdef RAINBOW_OS_ANDROID
	this->asset = AAssetManager_open(gNativeActivity->assetManager, path, AASSET_MODE_UNKNOWN);
#else
	this->stream = fopen(path, "rb");
#endif
	if (!this->stream)
		R_ERROR("[Rainbow] File: Failed to open '%s'\n", path);
}

File::File(const char *const path, const char *const mode) : is_asset(false), stream(nullptr)
{
	this->stream = fopen(path, mode);
}
