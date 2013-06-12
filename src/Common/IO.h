#ifndef COMMON_IO_H_
#define COMMON_IO_H_

#include <cstddef>
#include <cstdio>

#include "Platform/Macros.h"
#if defined(RAINBOW_ANDROID)
#	include <android/asset_manager.h>
#elif defined(RAINBOW_WIN)
typedef int mode_t;
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
		inline void close(FileHandle fh)
		{
		#if defined(RAINBOW_ANDROID)
			if (fh.type == kIOTypeAsset)
				AAsset_close(fh);
			else
		#endif
				fclose(fh);
		}

		/// Find and return the full path for file.
		void find(char *result, const char *const file, const Type type);

		/// Find and open a file.
		FileHandle find_and_open(const char *const file, const Type type);

		/// Make a directory, including any intermediate directories.
		/// \see mkdir()
		/// \see stat()
		/// \param path  Directory path to make.
		/// \param mode  Permission bits to set on created directories.
		/// \return 0 upon success. Otherwise, -1 is returned and errno is set to
		///         indicate the error.
		int mkdirp(const char *const path, const mode_t mode);

		/// Open file at path.
		FileHandle open(const char *const file, const Type type);

		/// Read \p size bytes from file into buffer \p dst.
		/// \param[out] dst   Destination buffer.
		/// \param      size  Number of bytes to read.
		/// \param      fh    Handle of file to read from.
		/// \return Number of bytes read.
		inline size_t read(void *dst, const size_t size, FileHandle fh)
		{
		#if defined(RAINBOW_ANDROID)
			if (fh.type == kIOTypeAsset)
				return AAsset_read(fh, dst, size);
		#endif
			return fread(dst, sizeof(char), size, fh);
		}

		/// Return the file size.
		size_t size(FileHandle fh);

		inline size_t write(const void *ptr, const size_t size, FileHandle fh)
		{
			return fwrite(ptr, sizeof(char), size, fh);
		}
	}
}

#endif
