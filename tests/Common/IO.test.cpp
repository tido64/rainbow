#include <sys/stat.h>

#include "Common/IO.h"
#include "Common/Random.h"

// For tests of open(), read(), size() and write(), see DataTest::SaveAndLoad.

TEST(IOTest, CreateDirectories)
{
	Random::seed(0);
	char random[8];
	sprintf(random, "%x", Random::next(0x100000, 0xffffff));

	char path[256];
	strcpy(path, "/tmp/rainbow-");
	strcat(path, random);
	const size_t end = strlen(path) - 1;
	strcat(path, "/1/2/3/4/");

	ASSERT_EQ(0, Rainbow::IO::mkdirp(path, 0755));
	struct stat sb;
	for (size_t i = strlen(path) - 1; i > end; --i)
	{
		if (path[i] == '/')
		{
			path[i] = '\0';
			ASSERT_EQ(0, stat(path, &sb));
			ASSERT_EQ(static_cast<unsigned int>(S_IFDIR),
			          static_cast<unsigned int>(sb.st_mode & S_IFDIR));
			rmdir(path);
		}
	}
}
