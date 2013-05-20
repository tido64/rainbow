#ifndef COMMON_IO_H_
#define COMMON_IO_H_

#include <cstdio>

#include "Platform/Macros.h"
#if defined(RAINBOW_ANDROID)
#	include <cstring>
#	include <android/asset_manager.h>

extern struct AAssetManager *g_asset_manager;

#elif defined(RAINBOW_IOS)

extern NSString *supportDir;

#else
#	include <cstddef>
#	include <cstring>

extern char data_path[];      ///< Path to asset data.
extern char userdata_path[];  ///< Path to user-specific data.

#endif

namespace Rainbow
{
	namespace IO
	{
	#if defined(RAINBOW_ANDROID)
		typedef AAsset* FileHandle;
	#elif defined(RAINBOW_IOS)
		typedef NSString* FileHandle;
	#else
		typedef FILE* FileHandle;
	#endif

		enum Type
		{
			ASSET,
			USER,
			WRITE,
			INTERNAL
		};

		inline void close(FileHandle fh)
		{
		#if defined(RAINBOW_ANDROID)
			AAsset_close(fh);
		#elif defined(RAINBOW_IOS)
			static_cast<void>(fh);
		#else
			fclose(fh);
		#endif
		}

		inline FileHandle open(const char *const file, unsigned int flag)
		{
		#if defined(RAINBOW_ANDROID)

			// Android doesn't ignore multiple '/' in paths.
			int j = -1;
			char path[256];
			for (size_t i = 0; i < strlen(file); ++i)
			{
				if (file[i] == '/' && file[i + 1] == '/')
					continue;
				path[++j] = file[i];
			}
			path[j] = '\0';
			return AAssetManager_open(g_asset_manager, path, AASSET_MODE_UNKNOWN);
			static_cast<void>(flag);

		#elif defined(RAINBOW_IOS)

			NSString *path = [[NSString alloc]
					initWithBytesNoCopy:(void*)file
					             length:strlen(file)
					           encoding:NSUTF8StringEncoding
					       freeWhenDone:NO];
			switch (flag)
			{
				case ASSET:
					path = [[NSBundle mainBundle]
						pathForResource:[path stringByDeletingPathExtension]
						ofType:[path pathExtension]];
					break;
				case USER:
					if (!supportDir)
					{
						NSError *err = nil;
						supportDir = [[[NSFileManager defaultManager]
							URLForDirectory:NSLibraryDirectory
							inDomain:NSUserDomainMask appropriateForURL:nil
							create:YES error:&err] path];

						if (!supportDir)
							return nil;

						//supportDir = [supportDir stringByAppendingPathComponent:@"Preferences"];
					}
					path = [supportDir stringByAppendingPathComponent:path];
					break;
			}
			return path;

		#else

			const char asset[] = "rb";
			const char user[] = "r+b";
			const char write[] = "wb";

			const char *mode = nullptr;
			char path[256];
			switch (flag)
			{
				case ASSET:
					strcpy(path, data_path);
					mode = asset;
					break;
				case USER:
					strcpy(path, userdata_path);
					mode = user;
					break;
				case WRITE:
					strcpy(path, userdata_path);
					mode = write;
					break;
				default:
					path[0] = '\0';
					mode = write;
					break;
			}
			strcat(path, file);
			return fopen(path, mode);

		#endif
		}

		inline size_t read(void *dst, const size_t size, FileHandle fh)
		{
		#if defined(RAINBOW_ANDROID)
			return AAsset_read(fh, dst, size);
		#elif defined(RAINBOW_IOS)
			static_cast<void>(dst);
			static_cast<void>(size);
			static_cast<void>(fh);
			return 0;
		#else
			return fread(dst, sizeof(unsigned char), size, fh);
		#endif
		}

		inline int seek(FileHandle fh, const size_t offset)
		{
		#if defined(RAINBOW_ANDROID)
			return AAsset_seek(fh, offset, SEEK_SET);
		#elif defined(RAINBOW_IOS)
			static_cast<void>(fh);
			static_cast<void>(offset);
			return 0;
		#else
			return fseek(fh, offset, SEEK_SET);
		#endif
		}

		inline size_t size(FileHandle fh)
		{
		#if defined(RAINBOW_ANDROID)
			return AAsset_getLength(fh);
		#elif defined(RAINBOW_IOS)
			static_cast<void>(fh);
			return 0;
		#else
			fseek(fh, 0, SEEK_END);
			const size_t size = ftell(fh);
			fseek(fh, 0, SEEK_SET);
			return size;
		#endif
		}

		inline size_t tell(FileHandle fh)
		{
		#if defined(RAINBOW_ANDROID) || defined(RAINBOW_IOS)
			static_cast<void>(fh);
			return 0;
		#else
			return ftell(fh);
		#endif
		}

		inline size_t write(const void *ptr, const size_t size, FileHandle fh)
		{
		#if defined(RAINBOW_ANDROID) || defined(RAINBOW_IOS)
			static_cast<void>(ptr);
			static_cast<void>(size);
			static_cast<void>(fh);
			return 0;
		#else
			return fwrite(ptr, sizeof(unsigned char), size, fh);
		#endif
		}
	}
}

#endif
