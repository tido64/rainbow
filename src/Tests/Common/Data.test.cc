// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <cstring>

#include <catch.hpp>

#include "Common/Algorithm.h"
#include "Common/Data.h"
#include "FileSystem/Path.h"

namespace
{
	const char kSecretData[] = "It's a double-rainbow!\n";
}

TEST_CASE("Data can reference in-memory data", "[data]")
{
	Data blob = Data::from_literal(kSecretData);
	REQUIRE(blob.bytes() == kSecretData);
	REQUIRE(blob.size() == rainbow::array_size(kSecretData) - 1);
	REQUIRE(memcmp(blob, kSecretData, blob.size()) == 0);
}

TEST_CASE("Data manages data and performs disk operations", "[data]")
{
	Data blob(kSecretData, rainbow::array_size(kSecretData),
	          Data::Ownership::Reference);

	SECTION("Data manages arbitrary data blobs")
	{
		REQUIRE(blob.size() == rainbow::array_size(kSecretData));
		REQUIRE(strcmp(blob, kSecretData) == 0);
	}

	SECTION("Data writes and reads from disk")
	{
		const char file[] = "Rainbow__Data.test";

		REQUIRE(blob.save(file));
		const Data &from_disk = Data::load_document(file);
		REQUIRE(from_disk);
		REQUIRE(strcmp(from_disk, kSecretData) == 0);
		Path path(file, Path::RelativeTo::UserDataPath);
		remove(path);
	}
}
