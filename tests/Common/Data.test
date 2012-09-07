#include "Common/Data.h"

extern char userdata_path[];

class DataTest : public testing::Test
{
protected:
	const char *const file;
	const char *const secret;

	DataTest() :
		file("Rainbow__Data.test"),
		secret("It's a double-rainbow!\n")
	{
		strcpy(userdata_path, "/tmp/");
	}
};

TEST_F(DataTest, Copy)
{
	Data blob;
	blob.copy(secret, strlen(secret));
	ASSERT_EQ(strlen(secret), blob.size());
	ASSERT_STREQ(secret, blob);
}

TEST_F(DataTest, SaveAndLoad)
{
	{
		Data blob;
		blob.copy(secret, strlen(secret));
		ASSERT_EQ(strlen(secret), blob.size());
		ASSERT_STREQ(secret, blob);
		ASSERT_TRUE(blob.save(file));
	}
	{
		Data blob(file, 0);
		ASSERT_TRUE(blob);
		ASSERT_STREQ(secret, blob);
	}
	strcat(userdata_path, file);
	remove(userdata_path);
}
