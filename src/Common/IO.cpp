#include <cerrno>
#include <cstring>
#include <sys/stat.h>

#include "Common/Debug.h"
#include "Common/IO.h"

#ifdef RAINBOW_WIN
#	include <direct.h>
#	define S_IRWXU 0000700
#	define S_IRWXG 0000070
#	define fileno _fileno
#	define kPathSeparator '\\'
#	define kPathSeparatorLit "\\"
#	define mkdir(dirname, mode) _mkdir(dirname)
#else
#	ifdef RAINBOW_ANDROID
#		include <android/native_activity.h>
#	endif
#	define kPathSeparator '/'
#	define kPathSeparatorLit "/"
#endif

#if defined(RAINBOW_ANDROID)
extern ANativeActivity *gNativeActivity;
#elif !defined(RAINBOW_IOS)
extern char data_path[];      ///< Path to asset data.
extern char userdata_path[];  ///< Path to user-specific data.
#endif

namespace Rainbow
{
	namespace IO
	{
		namespace
		{
			const char kErrorAccessDirectory[] = "Error accessing '%s' (%x)\n";
			const char kErrorCreateDirectory[] = "Failed to create directory '%s' (%x)\n";
			const char kErrorInvalidDirectory[] = "'%s' is not a valid directory\n";
		}

		void find(char *result, const char *const file, const Type type)
		{
			result[0] = '\0';
			switch (type)
			{
				case kIOTypeAsset:
					#if defined(RAINBOW_ANDROID)
					{
						// Android doesn't ignore multiple '/' in paths.
						int i = -1;
						int j = -1;
						while (file[++i] != '\0')
						{
							if (file[i] == '/' && file[i + 1] == '/')
								continue;
							result[++j] = file[i];
						}
						result[++j] = '\0';
					}
					#elif defined(RAINBOW_IOS)
					{
						NSString *path = [[NSString alloc]
								initWithBytesNoCopy:(void*)file
								             length:strlen(file)
								           encoding:NSUTF8StringEncoding
								       freeWhenDone:NO];
						path = [[NSBundle mainBundle]
								pathForResource:[path stringByDeletingPathExtension]
								         ofType:[path pathExtension]];
						if (!path)
							break;
						strcpy(result, [path UTF8String]);
					}
					#else
					{
						strcpy(result, data_path);
						strcat(result, file);
					}
					#endif
					break;
				case kIOTypeDocument:
					#ifdef RAINBOW_IOS
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
						NSString *path = [[NSString alloc]
								initWithBytesNoCopy:(void*)file
								             length:strlen(file)
								           encoding:NSUTF8StringEncoding
								       freeWhenDone:NO];
						path = [libraryDir stringByAppendingPathComponent:path];
						strcpy(result, [path UTF8String]);
					}
					#else
					{
					#ifdef RAINBOW_ANDROID
						const char *userdata_path = gNativeActivity->externalDataPath;
						if (!userdata_path)
						{
							userdata_path = gNativeActivity->internalDataPath;
							if (!userdata_path)
								break;
						}
					#endif  // RAINBOW_ANDROID
						struct stat sb;
						if (stat(userdata_path, &sb) != 0)
						{
							if (errno != ENOENT || mkdirp(userdata_path, S_IRWXU | S_IRWXG) != 0)
								break;
						}
						else if ((sb.st_mode & S_IFDIR) == 0)
							break;
						strcpy(result, userdata_path);
						strcat(result, kPathSeparatorLit);
						strcat(result, file);
					}
					#endif  // RAINBOW_IOS
					break;
				default:
					break;
			}
		}

		FileHandle find_and_open(const char *const file, const Type type)
		{
			char path[256];
			find(path, file, (type == kIOTypeWrite) ? kIOTypeDocument : type);
			return (path[0] == '\0') ? nullptr : open(path, type);
		}

		int mkdirp(const char *const path, const mode_t mode)
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
				#ifdef RAINBOW_WIN
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
					if ((sb.st_mode & S_IFDIR) == 0)
					{
						R_ERROR(kErrorInvalidDirectory, dir);
						return -1;
					}
					dir[i] = kPathSeparator;
				}
			}
			return 0;
		}

		FileHandle open(const char *const file, const Type type)
		{
			FileHandle fh = nullptr;
			switch (type)
			{
				case kIOTypeAsset:
					#ifdef RAINBOW_ANDROID
						fh = AAssetManager_open(gNativeActivity->assetManager, file, AASSET_MODE_UNKNOWN);
					#else
						fh = fopen(file, "rb");
					#endif
					break;
				case kIOTypeDocument:
					fh = fopen(file, "r+b");
					break;
				case kIOTypeWrite:
					fh = fopen(file, "wb");
					break;
				default:
					break;
			}
			return fh;
		}

		size_t size(FileHandle fh)
		{
		#ifdef RAINBOW_ANDROID
			if (fh.type == kIOTypeAsset)
				return AAsset_getLength(fh);
		#endif
			const int fd = fileno(static_cast<FILE*>(fh));
			struct stat file_status;
			return (fstat(fd, &file_status) != 0) ? 0 : file_status.st_size;
		}
	}
}

#undef kPathSeparatorLit
#undef kPathSeparator
