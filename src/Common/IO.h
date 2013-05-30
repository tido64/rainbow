#ifndef COMMON_IO_H_
#define COMMON_IO_H_

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <sys/stat.h>

#include "Platform/Macros.h"
#if defined(RAINBOW_ANDROID)
#	include <android/asset_manager.h>

extern struct AAssetManager *g_asset_manager;

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
	#if defined(RAINBOW_ANDROID)
		typedef AAsset* FileHandle;
	#else
		typedef FILE* FileHandle;
	#endif

		enum Type
		{
			kIOTypeAsset,     ///< Open file as read-only.
			kIOTypeDocument,  ///< Open file as read-write.
			kIOTypeWrite      ///< Create/open file for writing.
		};

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
			AAsset_close(fh);
		#else
			fclose(fh);
		#endif
		}

		void find(char *result, const char *const file, const Type type)
		{
			result[0] = '\0';
			switch (type)
			{
				case kIOTypeAsset: {
					#if defined(RAINBOW_ANDROID)
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
					#elif defined(RAINBOW_IOS)
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
					#else
						strcpy(result, data_path);
						strcat(result, file);
					#endif
					break;
				}
				case kIOTypeDocument: {
					#if defined(RAINBOW_ANDROID)
					#elif defined(RAINBOW_IOS)
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
					#else
						strcpy(result, userdata_path);
						strcat(result, file);
					#endif
					break;
				}
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
			switch (type)
			{
				case kIOTypeAsset:
					#if defined(RAINBOW_ANDROID)
						return AAssetManager_open(g_asset_manager, file, AASSET_MODE_UNKNOWN);
					#else
						return fopen(file, "rb");
					#endif
				case kIOTypeDocument:
					#if defined(RAINBOW_ANDROID)
						return nullptr;
					#else
						return fopen(file, "r+b");
					#endif
				case kIOTypeWrite:
					#if defined(RAINBOW_ANDROID)
						return nullptr;
					#else
						return fopen(file, "wb");
					#endif
				default:
					return nullptr;
			}
		}

		size_t read(void *dst, const size_t size, FileHandle fh)
		{
		#if defined(RAINBOW_ANDROID)
			return AAsset_read(fh, dst, size);
		#else
			return fread(dst, sizeof(char), size, fh);
		#endif
		}

		size_t size(FileHandle fh)
		{
		#if defined(RAINBOW_ANDROID)
			return AAsset_getLength(fh);
		#else
			struct stat file_status;
			return (fstat(fileno(fh), &file_status) != 0) ? 0 : file_status.st_size;
		#endif
		}

		size_t write(const void *ptr, const size_t size, FileHandle fh)
		{
		#if defined(RAINBOW_ANDROID) || defined(RAINBOW_IOS)
			return 0;
			static_cast<void>(ptr);
			static_cast<void>(size);
			static_cast<void>(fh);
		#else
			return fwrite(ptr, sizeof(char), size, fh);
		#endif
		}
	}
}

#endif
