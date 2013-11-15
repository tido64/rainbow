#include <sys/stat.h>

#include "Common/Random.h"
#include "FileSystem/Path.h"

// For tests of File, see DataTest::SaveAndLoad.

TEST(PathTest, CreateDirectories)
{
	Random::seed(0);
	char random[32];
	sprintf(random, "/tmp/rainbow-%x", Random::next(0x10000000u, 0xffffffffu));

	Path path(random, Path::RelativeTo::Root);
	const size_t end = strlen(path) - 1;
	path += "1/2/3/4/";

	ASSERT_EQ(0, path.create());
	struct stat sb;
	for (size_t i = strlen(path) - 1; i > end; --i)
	{
		if (path[i] == '/')
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
