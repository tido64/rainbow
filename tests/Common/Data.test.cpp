#include "Common/Data.h"
#include "FileSystem/Path.h"

class DataTest : public testing::Test
{
protected:
	const char *const file;
	const char *const secret;

	DataTest() :
		file("Rainbow__Data.test"),
		secret("It's a double-rainbow!\n")
	{
		Path::set_current("/tmp");
	}
};

TEST_F(DataTest, Copy)
{
	Data blob(secret, strlen(secret), Data::kDataReference);
	ASSERT_EQ(strlen(secret), blob.size());
	ASSERT_STREQ(secret, blob);
}

TEST_F(DataTest, SaveAndLoad)
{
	{
		Data blob(secret, strlen(secret), Data::kDataReference);
		ASSERT_EQ(strlen(secret), blob.size());
		ASSERT_STREQ(secret, blob);
		ASSERT_TRUE(blob.save(file));
	}
	{
		const Data &blob = Data::load_document(file);
		ASSERT_TRUE(blob);
		ASSERT_STREQ(secret, blob);
	}
	Path path(file, Path::RelativeTo::UserDataPath);
	remove(path);
}
