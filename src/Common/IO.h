#ifndef COMMON_IO_H_
#define COMMON_IO_H_

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <sys/stat.h>

#include "Platform/Macros.h"
#if defined(RAINBOW_ANDROID)
#	include <cerrno>
#	include <android/native_activity.h>
#	include "Common/Debug.h"

extern ANativeActivity *gNativeActivity;

#else
#	ifdef RAINBOW_WIN
#		define fileno _fileno
#	endif

extern char data_path[];      ///< Path to asset data.
extern char userdata_path[];  ///< Path to user-specific data.

#endif

namespace Rainbow
{
	namespace IO
	{
		enum Type
		{
			kIOTypeAsset,     ///< Open file as read-only.
			kIOTypeDocument,  ///< Open file as read-write.
			kIOTypeWrite      ///< Create/open file for writing.
		};

	#if defined(RAINBOW_ANDROID)
	#	include "Common/impl/IO_Android.inc"
	#else
		typedef FILE* FileHandle;
	#endif

		/// Close the file handle.
		inline void close(FileHandle fh);

		/// Find and return the full path for file.
		inline void find(char *result, const char *const file, const Type type);

		/// Find and open a file.
		inline FileHandle find_and_open(const char *const file, const Type type);

		/// Open file at path.
		inline FileHandle open(const char *const file, const Type type);

		/// Read \p size bytes from file into buffer \p dst.
		/// \param[out] dst   Destination buffer.
		/// \param      size  Number of bytes to read.
		/// \param      fh    Handle of file to read from.
		/// \return Number of bytes read.
		inline size_t read(void *dst, const size_t size, FileHandle fh);

		/// Return the file size.
		inline size_t size(FileHandle fh);

		inline size_t write(const void *ptr, const size_t size, FileHandle fh);

		void close(FileHandle fh)
		{
		#if defined(RAINBOW_ANDROID)
			if (fh.type == kIOTypeAsset)
				AAsset_close(fh);
			else
		#endif
				fclose(fh);
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
					#if defined(RAINBOW_ANDROID)
					{
						const char *userdata_path = gNativeActivity->externalDataPath;
						if (!userdata_path)
						{
							userdata_path = gNativeActivity->internalDataPath;
							if (!userdata_path)
								break;
						}
						struct stat sb;
						if (stat(userdata_path, &sb) != 0)
						{
							if (errno != ENOENT || mkdirp(userdata_path, S_IRWXU | S_IRWXG) != 0)
								break;
						}
						else if ((sb.st_mode & S_IFDIR) == 0)
							break;
						strcpy(result, userdata_path);
						strcat(result, "/");
						strcat(result, file);
					}
					#elif defined(RAINBOW_IOS)
					{
						NSError *err = nil;
						NSString *supportDir = [[[NSFileManager defaultManager]
								    URLForDirectory:NSLibraryDirectory
								           inDomain:NSUserDomainMask
								  appropriateForURL:nil
								             create:YES
								              error:&err] path];
						if (!supportDir)
							return;
						NSString *path = [[NSString alloc]
								initWithBytesNoCopy:(void*)file
								             length:strlen(file)
								           encoding:NSUTF8StringEncoding
								       freeWhenDone:NO];
						path = [supportDir stringByAppendingPathComponent:path];
						strcpy(result, [path UTF8String]);
					}
					#else
					{
						strcpy(result, userdata_path);
						strcat(result, file);
					}
					#endif
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

		FileHandle open(const char *const file, const Type type)
		{
			FileHandle fh = nullptr;
			switch (type)
			{
				case kIOTypeAsset:
					#if defined(RAINBOW_ANDROID)
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

		size_t read(void *dst, const size_t size, FileHandle fh)
		{
		#if defined(RAINBOW_ANDROID)
			if (fh.type == kIOTypeAsset)
				return AAsset_read(fh, dst, size);
		#endif
			return fread(dst, sizeof(char), size, fh);
		}

		size_t size(FileHandle fh)
		{
		#if defined(RAINBOW_ANDROID)
			if (fh.type == kIOTypeAsset)
				return AAsset_getLength(fh);
		#endif
			const int fd = fileno(static_cast<FILE*>(fh));
			struct stat file_status;
			return (fstat(fd, &file_status) != 0) ? 0 : file_status.st_size;
		}

		size_t write(const void *ptr, const size_t size, FileHandle fh)
		{
			return fwrite(ptr, sizeof(char), size, fh);
		}
	}
}

#endif
