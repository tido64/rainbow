// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <catch.hpp>

#include "Common/Global.h"

namespace
{
	enum class InitStatus
	{
		Success,
		Fail
	};

	class GlobalTest : public Global<GlobalTest>
	{
	public:
		GlobalTest(const InitStatus status)
		{
			if (status == InitStatus::Success)
				make_global();
		}

		using Global::is_global;
	};
}

TEST_CASE("A Global is not global unless explicitly made so", "[global]")
{
	GlobalTest success(InitStatus::Success);
	REQUIRE(success.is_global());
	REQUIRE(GlobalTest::Get() == &success);

	GlobalTest fail(InitStatus::Fail);
	REQUIRE_FALSE(fail.is_global());
	REQUIRE(GlobalTest::Get() == &success);
}
