#include <sys/stat.h>

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

TEST(PathTest, CreateDirectories)
{
	Random::seed(0);
	char random[32];
	sprintf(random, kTestRandomPath, Random::next(0x10000000u, 0xffffffffu));

	Path path(random, Path::RelativeTo::Root);
	const size_t end = strlen(path) - 1;
	path += "1" kPathSeparator "2" kPathSeparator "3" kPathSeparator "4" kPathSeparator;

	ASSERT_EQ(0, path.create());
	struct stat sb;
	for (size_t i = strlen(path) - 1; i > end; --i)
	{
		if (path[i] == kPathSeparatorCharacter)
		{
			char &c = const_cast<char&>(path[i]);
			c = '\0';
			ASSERT_EQ(0, stat(path, &sb));
			ASSERT_EQ(static_cast<unsigned int>(S_IFDIR),
			          static_cast<unsigned int>(sb.st_mode & S_IFDIR));
			rmdir(path);
		}
	}
}

TEST(PathTest, CurrentPath)
{
	char cwd[256];
	ASSERT_EQ(cwd, getcwd(cwd, sizeof(cwd)));
	ASSERT_STREQ(cwd, Path::current());
	const Path path;
	ASSERT_STREQ(cwd, path);
}

TEST(PathTest, RelativeToCurrent)
{
	char cwd[256];
	ASSERT_EQ(cwd, getcwd(cwd, sizeof(cwd)));
	strcat(cwd, kPathSeparator kTestFileName);

	const Path path(kTestFileName, Path::RelativeTo::CurrentPath);
	ASSERT_STREQ(cwd, path);
}

TEST(PathTest, RelativeToRoot)
{
	const Path path(kTestPath, Path::RelativeTo::Root);
	ASSERT_STREQ(kTestPath, path);
}

TEST(PathTest, SetPath)
{
	Path path;
	ASSERT_STRNE(kTestPath, path);
	path = kTestPath;
	ASSERT_STREQ(kTestPath, path);
}

TEST(PathTest, AppendPathComponent)
{
	Path path;
	path = kTestPath;
	ASSERT_STREQ(kTestPath, path);
	path += kTestFileName;
	ASSERT_STREQ(kTestPath kPathSeparator kTestFileName, path);
	path += kPathSeparator;
	ASSERT_STREQ(kTestPath kPathSeparator kTestFileName kPathSeparator kPathSeparator, path);
	path += kTestFileName;
	ASSERT_STREQ(kTestPath kPathSeparator kTestFileName kPathSeparator kPathSeparator kTestFileName, path);
}
