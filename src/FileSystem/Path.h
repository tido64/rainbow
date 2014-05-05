// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef FILESYSTEM_PATH_H_
#define FILESYSTEM_PATH_H_

#include "Platform/Macros.h"
#ifdef RAINBOW_OS_MACOS
#include <CoreFoundation/CoreFoundation.h>
#endif

#include "Common/NonCopyable.h"

class Path
{
public:
	enum class RelativeTo
	{
		CurrentPath,
		UserDataPath,
		Root
	};

	static const char* current();
	static void set_current();
	static void set_current(const char *const path);

	Path();
	explicit Path(const char *const file,
	              const RelativeTo rel = RelativeTo::CurrentPath);

#ifdef RAINBOW_TEST
	int create();
#endif

#ifdef RAINBOW_OS_MACOS
	CFURLRef CreateCFURL() const;
#endif

	/// Returns whether there is a file at this path.
	bool is_file() const;

	Path& operator=(const char *const path);
	Path& operator+=(const char *const path);

	inline operator const char*() const;

#ifdef RAINBOW_OS_IOS
	operator NSURL*() const;
#endif

private:
	char path[256];
};

Path::operator const char*() const
{
	return this->path;
}

#endif
