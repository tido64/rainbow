// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <cstring>
#include <sys/stat.h>

#include <gtest/gtest.h>

#include "Common/Random.h"
#include "FileSystem/Path.h"

#ifdef RAINBOW_OS_WINDOWS
#	include <cstdint>
#	include <direct.h>
#	define getcwd(buf, size) _getcwd(buf, size)
#	define kPathSeparator           "\\"
#	define kPathSeparatorCharacter  '\\'
#	define rmdir(path) _rmdir(path)
using mode_t = uint16_t;
#else
#	include <unistd.h>
#	define kPathSeparator           "/"
#	define kPathSeparatorCharacter  '/'
#endif
#define kTestFileName    "rainbow.file"
#define kTestPath        "rainbow-test"
#define kTestRandomPath  "rainbow-%x"

// For tests of File, see DataTest::SaveAndLoad.

TEST(PathTest, CreatesDirectories)
{
	char random[32];
	sprintf(random, kTestRandomPath, rainbow::random(0x10000000u, 0xffffffffu));

	Path path(random, Path::RelativeTo::Root);
	const size_t end = strlen(path) - 1;
	path += "1" kPathSeparator "2" kPathSeparator "3" kPathSeparator
	        "4" kPathSeparator;

	ASSERT_EQ(0, path.create());
	struct stat sb;
	for (size_t i = strlen(path) - 1; i > end; --i)
	{
		if (path[i] == kPathSeparatorCharacter)
		{
			char& c = const_cast<char&>(path[i]);
			c = '\0';
			ASSERT_EQ(0, stat(path, &sb));
			ASSERT_EQ(static_cast<mode_t>(S_IFDIR), sb.st_mode & S_IFDIR);
			ASSERT_EQ(0, rmdir(path));
		}
	}
}

TEST(PathTest, CurrentPath)
{
	char cwd[256];
	ASSERT_EQ(cwd, getcwd(cwd, sizeof(cwd)));
	ASSERT_EQ(0, strcmp(Path::current(), cwd));
	const Path path;
	ASSERT_EQ(0, strcmp(path, cwd));
}

TEST(PathTest, RelativeToCurrentPath)
{
	char cwd[256];
	ASSERT_EQ(cwd, getcwd(cwd, sizeof(cwd)));
	strcat(cwd, kPathSeparator kTestFileName);

	const Path path(kTestFileName, Path::RelativeTo::CurrentPath);
	ASSERT_EQ(0, strcmp(path, cwd));
}

TEST(PathTest, RelativeToRoot)
{
	const Path path(kTestPath, Path::RelativeTo::Root);
	ASSERT_EQ(0, strcmp(path, kTestPath));
}

TEST(PathTest, SetsPath)
{
	Path path;
	ASSERT_NE(0, strcmp(path, kTestPath));
	path = kTestPath;
	ASSERT_EQ(0, strcmp(path, kTestPath));
}

TEST(PathTest, AppendsPathComponents)
{
	Path path;
	path = kTestPath;
	ASSERT_EQ(0, strcmp(path, kTestPath));
	path += kTestFileName;
	ASSERT_EQ(0, strcmp(path, kTestPath kPathSeparator kTestFileName));
	path += kPathSeparator;
	ASSERT_EQ(0, strcmp(path, kTestPath kPathSeparator kTestFileName
	                              kPathSeparator kPathSeparator));
	path += kTestFileName;
	ASSERT_EQ(0, strcmp(path, kTestPath kPathSeparator kTestFileName
	                              kPathSeparator kPathSeparator kTestFileName));
}
