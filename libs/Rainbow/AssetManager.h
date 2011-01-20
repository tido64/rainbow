/// Native assets loading. Supports zip archives.

/// \see http://www.nih.at/libzip/
///
/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef ASSETMANAGER_H_
#define ASSETMANAGER_H_

#include <Rainbow/Hardware/Platform.h>

#if defined(RAINBOW_ANDROID)
#	include <zip.h>
#	define RAINBOW_ZIP

typedef zip* Assets;

#elif defined(RAINBOW_IOS)

typedef NSBundle* Assets;

#else
#	include <cstdio>
#	include <zip.h>
#	define RAINBOW_ZIP

//typedef unsigned char Assets;
typedef zip* Assets;

#endif

class AssetManager
{
public:
	static inline AssetManager& Instance()
	{
		static AssetManager am;
		return am;
	}

	~AssetManager()
	{
		this->close();
	}

	/// Closes any open archive streams.
	void close()
	{
		if (this->archive == 0) return;

	#if defined(RAINBOW_ZIP)

		this->archive = reinterpret_cast<zip *>(zip_close(this->archive));

	#endif
	}

	/// Returns the full path to a resource or asset.
	const char* get_full_path(const char *filename)
	{
	#if defined(RAINBOW_ANDROID)

		unsigned int length = 0;
		while (filename[length] != '\0')
			++length;
		char *tmp = new char[length + 8];
		strcpy(tmp, "assets/");
		strcat(tmp, filename);
		return tmp;

	#elif defined(RAINBOW_IOS)

		NSString *file = [NSString stringWithUTF8String:(filename)];
		NSString *path = [archive pathForResource:[file stringByDeletingPathExtension] ofType:[file pathExtension]];
		return [path UTF8String];

	#else

		return filename;

	#endif
	}

	/// Loads a file into provided buffer.
	/// \param buffer    The buffer to fill
	/// \param filename  The file to load
	unsigned int load(unsigned char *&buffer, const char *filename)
	{
		unsigned int sz = 0;

	#ifdef RAINBOW_ZIP

		assert(this->archive != 0);
		filename = this->get_full_path(filename);
		zip_file *zf = zip_fopen(this->archive, filename, 0);
		assert(zf != 0);

		// Get uncompressed size of file
		struct zip_stat stat;
		unsigned int read = zip_stat(this->archive, filename, 0, &stat);
		assert(read == 0);
		sz = stat.size;

		// Allocate buffer
		buffer = (unsigned char *)malloc(sz + 1);
		assert(buffer != 0);

		// Fill buffer
		read = zip_fread(zf, buffer, sz);
		zip_fclose(zf);
		assert(read == sz);

	#else

		filename = this->get_full_path(filename);
		FILE *fp = fopen(filename, "rb");
		assert(fp != 0);

		// Get size of file
		fseek(fp, 0, SEEK_END);
		sz = ftell(fp);

		// Allocate buffer
		buffer = (unsigned char *)malloc(sz + 1);
		assert(buffer != 0);

		// Fill buffer
		fseek(fp, 0, SEEK_SET);
		unsigned int read = fread(buffer, 1, sz, fp);
		fclose(fp);
		assert(read == sz);

	#endif

		return sz;
	}

	/// Set the source to load resources from.
	inline void set_source(const char *src = 0)
	{
	#if defined(RAINBOW_IOS)

		this->archive = [NSBundle mainBundle];

	#elif defined(RAINBOW_ZIP)

		int errorp = 0;
		this->archive = zip_open(src, 0, &errorp);
		assert(errorp == 0);

	#endif
	}

private:
	Assets archive;  ///< Source of assets

	// Singleton directives
	AssetManager() : archive(0) { }
	AssetManager(const AssetManager &);
	AssetManager& operator=(const AssetManager &);
};

#endif
