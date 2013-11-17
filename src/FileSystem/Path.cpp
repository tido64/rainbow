// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <cerrno>
#include <cstring>
#include <sys/stat.h>

#include "FileSystem/Path.h"
#ifdef RAINBOW_OS_WINDOWS
#	include <direct.h>
#	define S_IRWXU 0000700
#	define S_IRWXG 0000070
#	define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)  // directory
#	define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)  // regular file
#	define getcwd(buf, size) _getcwd(buf, size)
#	define kPathSeparator '\\'
#	define kPathSeparatorLiteral "\\"
#	define mkdir(dirname, mode) _mkdir(dirname)
#	define mode_t int
#else
#	ifdef RAINBOW_OS_ANDROID
#		include <android/native_activity.h>
#	endif
#	include <unistd.h>
#	define kPathSeparator '/'
#	define kPathSeparatorLiteral "/"
#endif

#include "Common/Debug.h"

#ifdef RAINBOW_OS_ANDROID
extern ANativeActivity *gNativeActivity;
#endif

namespace
{
	const char kErrorAccessDirectory[] = "Error accessing '%s' (%x)\n";
	const char kErrorCreateDirectory[] = "Failed to create directory '%s' (%x)\n";
	const char kErrorInvalidDirectory[] = "'%s' is not a valid directory\n";
	const char kUserDataPath[] = "user";

	Path g_current_path;
	Path g_user_data_path;

	/// Creates a directory, including any intermediate directories.
	/// \see mkdir()
	/// \see stat()
	/// \param path  Directory path to create.
	/// \param mode  Permission bits to set on created directories.
	/// \return 0 upon success. Otherwise, -1 is returned and errno is set to
	///         indicate the error.
	int create_directories(const char *const path, const mode_t mode)
	{
		bool end = false;
		struct stat sb;
		char dir[256];
		strcpy(dir, path);
		for (int i = 1; !end; ++i)
		{
			end = dir[i] == '\0';
			if (dir[i] == kPathSeparator || end)
			{
			#ifdef RAINBOW_OS_WINDOWS
				// Windows' stat() doesn't handle trailing backslash.
				if (dir[i - 1] == ':' || dir[i - 1] == kPathSeparator)
					continue;
			#endif
				dir[i] = '\0';
				if (stat(dir, &sb) != 0)
				{
					if (errno != ENOENT)
					{
						R_ERROR(kErrorAccessDirectory, dir, errno);
						return -1;
					}
					if (mkdir(dir, mode) != 0 || stat(dir, &sb) != 0)
					{
						R_ERROR(kErrorCreateDirectory, dir, errno);
						return -1;
					}
				}
				if (!S_ISDIR(sb.st_mode))
				{
					R_ERROR(kErrorInvalidDirectory, dir);
					return -1;
				}
				dir[i] = kPathSeparator;
			}
		}
		return 0;
	}
}

const char* Path::current()
{
	return g_current_path;
}

void Path::set_current()
{
#ifdef RAINBOW_OS_ANDROID
	const char *data_path = gNativeActivity->externalDataPath;
	if (!data_path)
	{
		data_path = gNativeActivity->internalDataPath;
		if (!data_path)
			return;
	}
	g_user_data_path = data_path;
#else
	g_user_data_path = g_current_path;
	g_user_data_path += kUserDataPath;
#endif
}

void Path::set_current(const char *const path)
{
	g_current_path = path;
	set_current();
}

Path::Path()
{
	if (getcwd(this->path, sizeof(this->path)) != this->path)
		this->path[0] = '\0';
}

Path::Path(const char *const file, const RelativeTo rel)
{
	this->path[0] = '\0';
	switch (rel)
	{
		case RelativeTo::CurrentPath:
			#if defined(RAINBOW_OS_ANDROID)
			{
				// Android doesn't ignore multiple '/' in paths.
				int i = -1;
				int j = -1;
				while (file[++i] != '\0')
				{
					if (file[i] == '/' && file[i + 1] == '/')
						continue;
					this->path[++j] = file[i];
				}
				this->path[++j] = '\0';
			}
			#elif defined(RAINBOW_OS_IOS)
			{
				NSString *string = [[NSString alloc]
						initWithBytesNoCopy:(void*)file
						             length:strlen(file)
						           encoding:NSUTF8StringEncoding
						       freeWhenDone:NO];
				string = [[NSBundle mainBundle]
						pathForResource:[string stringByDeletingPathExtension]
						         ofType:[string pathExtension]];
				if (!string)
					break;

				*this = [string UTF8String];
			}
			#else
			{
				*this = ::g_current_path;
				*this += file;
			}
			#endif
			break;
		case RelativeTo::UserDataPath:
			#ifdef RAINBOW_OS_IOS
			{
				NSError *err = nil;
				NSString *libraryDir = [[[NSFileManager defaultManager]
						    URLForDirectory:NSLibraryDirectory
						           inDomain:NSUserDomainMask
						  appropriateForURL:nil
						             create:YES
						              error:&err] path];
				if (!libraryDir)
					return;
				NSString *string = [[NSString alloc]
						initWithBytesNoCopy:(void*)file
						             length:strlen(file)
						           encoding:NSUTF8StringEncoding
						       freeWhenDone:NO];
				*this = [[libraryDir stringByAppendingPathComponent:string] UTF8String];
			}
			#else
			{
				if (!g_user_data_path)
					break;
				struct stat sb;
				if (stat(g_user_data_path, &sb) != 0)
				{
					if (errno != ENOENT || create_directories(g_user_data_path, S_IRWXU | S_IRWXG) != 0)
						break;
				}
				else if (!S_ISDIR(sb.st_mode))
					break;
				*this = g_user_data_path;
				*this += file;
			}
			#endif  // RAINBOW_OS_IOS
			break;
		case RelativeTo::Root:
			*this = file;
			break;
		default:
			break;
	}
}

#ifdef RAINBOW_TEST

int Path::create()
{
	return create_directories(this->path, 0775);
}

#endif

#ifdef RAINBOW_OS_MACOS

CFURLRef Path::CreateCFURL() const
{
	CFStringRef str = CFStringCreateWithBytesNoCopy(
			kCFAllocatorDefault, (const UInt8*)this->path, strlen(this->path),
			kCFStringEncodingUTF8, false, kCFAllocatorNull);
	CFURLRef url = CFURLCreateWithFileSystemPath(
			kCFAllocatorDefault, str, kCFURLPOSIXPathStyle, false);
	CFRelease(str);
	return url;
}

#endif

bool Path::is_file() const
{
	struct stat sb;
	if (stat(this->path, &sb) != 0)
		return false;
	return S_ISREG(sb.st_mode);
}

Path& Path::operator=(const char *const path)
{
	strncpy(this->path, path, sizeof(this->path) - sizeof(this->path[0]));
	return *this;
}

Path& Path::operator+=(const char *const path)
{
	const size_t last = strlen(this->path) - 1;
	if (this->path[last] != kPathSeparator)
		strcat(this->path, kPathSeparatorLiteral);
	strcat(this->path, path);
	return *this;
}

#ifdef RAINBOW_OS_IOS

Path::operator NSURL*() const
{
	NSString *path = [[NSString alloc] initWithBytesNoCopy:(void*)this->path
	                                                length:strlen(this->path)
	                                              encoding:NSUTF8StringEncoding
	                                          freeWhenDone:NO];
	return [NSURL fileURLWithPath:path isDirectory:NO];
}

#endif
