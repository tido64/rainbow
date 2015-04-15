// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <sys/stat.h>

#include <catch.hpp>

#include "Common/Random.h"
#include "FileSystem/Path.h"

#ifdef RAINBOW_OS_WINDOWS
#	define kPathSeparator           "\\"
#	define kPathSeparatorCharacter  '\\'
#else
#	define kPathSeparator           "/"
#	define kPathSeparatorCharacter  '/'
#endif
#define kTestFileName    "rainbow.file"
#define kTestPath        "rainbow-test"
#define kTestRandomPath  "rainbow-%x"

// For tests of File, see DataTest::SaveAndLoad.

TEST_CASE("Create directories", "[path]")
{
	Random::seed(0);
	char random[32];
	sprintf(random, kTestRandomPath, Random::next(0x10000000u, 0xffffffffu));

	Path path(random, Path::RelativeTo::Root);
	const size_t end = strlen(path) - 1;
	path += "1" kPathSeparator "2" kPathSeparator "3" kPathSeparator "4" kPathSeparator;

	REQUIRE(path.create() == 0);
	struct stat sb;
	for (size_t i = strlen(path) - 1; i > end; --i)
	{
		if (path[i] == kPathSeparatorCharacter)
		{
			char &c = const_cast<char&>(path[i]);
			c = '\0';
			REQUIRE(stat(path, &sb) == 0);
			REQUIRE((sb.st_mode & S_IFDIR) == S_IFDIR);
			rmdir(path);
		}
	}
}

TEST_CASE("Current path", "[path]")
{
	char cwd[256];
	REQUIRE(getcwd(cwd, sizeof(cwd)) == cwd);
	REQUIRE(strcmp(Path::current(), cwd) == 0);
	const Path path;
	REQUIRE(strcmp(path, cwd) == 0);
}

TEST_CASE("Relative to current path", "[path]")
{
	char cwd[256];
	REQUIRE(getcwd(cwd, sizeof(cwd)) == cwd);
	strcat(cwd, kPathSeparator kTestFileName);

	const Path path(kTestFileName, Path::RelativeTo::CurrentPath);
	REQUIRE(strcmp(path, cwd) == 0);
}

TEST_CASE("Relative to root", "[path]")
{
	const Path path(kTestPath, Path::RelativeTo::Root);
	REQUIRE(strcmp(path, kTestPath) == 0);
}

TEST_CASE("Set path", "[path]")
{
	Path path;
	REQUIRE(strcmp(path, kTestPath) != 0);
	path = kTestPath;
	REQUIRE(strcmp(path, kTestPath) == 0);
}

TEST_CASE("Appends path components", "[path]")
{
	Path path;
	path = kTestPath;
	REQUIRE(strcmp(path, kTestPath) == 0);
	path += kTestFileName;
	REQUIRE(strcmp(path, kTestPath kPathSeparator kTestFileName) == 0);
	path += kPathSeparator;
	REQUIRE(strcmp(path, kTestPath kPathSeparator kTestFileName kPathSeparator kPathSeparator) == 0);
	path += kTestFileName;
	REQUIRE(strcmp(path, kTestPath kPathSeparator kTestFileName kPathSeparator kPathSeparator kTestFileName) == 0);
}
