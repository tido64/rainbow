#ifndef ASSETMANAGER_H_
#define ASSETMANAGER_H_

#include "Platform.h"

#if defined(RAINBOW_ANDROID)
#	include <zip.h>
#	define RAINBOW_ZIP

typedef zip* Assets;

#elif defined(RAINBOW_IOS)

typedef NSBundle* Assets;

#else
#	include <cassert>
#	include <cstdlib>
#	include <zip.h>
#	define RAINBOW_ZIP

//typedef unsigned char Assets;
typedef zip* Assets;

#endif


/// Native assets loading. Supports zip archives.

/// \see http://www.nih.at/libzip/
///
/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class AssetManager
{
public:
	static inline AssetManager& Instance()
	{
		static AssetManager am;
		return am;
	}

	~AssetManager() { this->close(); }

	/// Closes any open archive streams.
	void close();

	/// Returns the full path to a resource or asset.
	const char* get_full_path(const char *filename = 0);

	/// Loads a file into provided buffer.
	/// \param buffer    The buffer to fill
	/// \param filename  The file to load
	unsigned int load(unsigned char *&buffer, const char *filename);

	/// Set the source to load resources from.
	void set_source(const char *src = 0);

private:
	Assets archive;  ///< Source of assets

	// Singleton directives
	AssetManager() : archive(0) { }
	AssetManager(const AssetManager &);
	AssetManager& operator=(const AssetManager &);
};

inline void AssetManager::set_source(const char *src)
{
#if defined(RAINBOW_IOS)

	this->archive = [NSBundle mainBundle];

#elif defined(RAINBOW_ZIP)

	int errorp = 0;
	this->archive = zip_open(src, 0, &errorp);
	assert(errorp == 0);

#endif
}

#endif
