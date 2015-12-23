// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/Global.h"

namespace
{
	enum class InitStatus
	{
		Success,
		Fail
	};

	class GlobalObject : public Global<GlobalObject>
	{
	public:
		GlobalObject(InitStatus status)
		{
			if (status == InitStatus::Success)
				make_global();
		}

		using Global::is_global;
	};
}

TEST(GlobalTest, GlobalOnlyWhenExplicitlyMadeSo)
{
	GlobalObject success(InitStatus::Success);

	ASSERT_TRUE(success.is_global());
	ASSERT_EQ(&success, GlobalObject::Get());

	GlobalObject fail(InitStatus::Fail);

	ASSERT_FALSE(fail.is_global());
	ASSERT_EQ(&success, GlobalObject::Get());
}
