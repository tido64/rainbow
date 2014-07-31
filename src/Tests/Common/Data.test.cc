// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Data.h"
#include "FileSystem/Path.h"

// TODO: The following newlines were added for Catch to create unique names.


TEST_CASE("Data manages data and performs disk operations", "[data]")
{
	const char secret[] = "It's a double-rainbow!\n";
	Data blob(secret, strlen(secret), Data::Ownership::Reference);

	SECTION("Data manages arbitrary data blobs")
	{
		REQUIRE(blob.size() == strlen(secret));
		REQUIRE(strcmp(blob, secret) == 0);
	}

	SECTION("Data writes and reads from disk")
	{
		const char file[] = "Rainbow__Data.test";

		REQUIRE(blob.save(file));
		const Data &from_disk = Data::load_document(file);
		REQUIRE(from_disk);
		REQUIRE(strcmp(from_disk, secret) == 0);
		Path path(file, Path::RelativeTo::UserDataPath);
		remove(path);
	}
}
