#ifndef COMMON_IO_H_
#define COMMON_IO_H_

#include <cstdio>

#include "Platform.h"
#if defined(RAINBOW_ANDROID)
#	include <android/asset_manager.h>

extern struct AAssetManager *g_asset_manager;

#else
#	include <cstddef>

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
			ASSET,
			SUPPORT,
			INTERNAL
		};

		inline void close(FileHandle fh)
		{
		#if defined(RAINBOW_ANDROID)
			AAsset_close(fh);
		#else
			fclose(fh);
		#endif
		}

		inline FileHandle open(const char *const file, unsigned int flag)
		{
		#if defined(RAINBOW_ANDROID)
			static_cast<void>(flag);
			return AAssetManager_open(g_asset_manager, file, AASSET_MODE_UNKNOWN);
		#else
			const char asset[] = "rb";
			const char support[] = "r+b";

			const char *mode = nullptr;
			char path[256];
			switch (flag)
			{
				case ASSET:
					strcpy(path, data_path);
					mode = asset;
					break;
				case SUPPORT:
					strcpy(path, userdata_path);
					mode = support;
					break;
				default:
					path[0] = '\0';
					mode = support;
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
		#else
			return fread(dst, sizeof(unsigned char), size, fh);
		#endif
		}

		inline int seek(FileHandle fh, const size_t offset)
		{
		#if defined(RAINBOW_ANDROID)
			return AAsset_seek(fh, offset, SEEK_SET);
		#else
			return fseek(fh, offset, SEEK_SET);
		#endif
		}

		inline size_t size(FileHandle fh)
		{
		#if defined(RAINBOW_ANDROID)
			return AAsset_getLength(fh);
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
