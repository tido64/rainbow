// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Global.h"

#include <gtest/gtest.h>

namespace
{
    enum class InitStatus
    {
        Success,
        Fail
    };

    class GlobalObject : public rainbow::Global<GlobalObject>
    {
    public:
        explicit GlobalObject(InitStatus status)
        {
            if (status == InitStatus::Success)
                make_global();
        }

        using Global::is_global;
    };
}  // namespace

TEST(GlobalTest, GlobalOnlyWhenExplicitlyMadeSo)
{
    GlobalObject success(InitStatus::Success);

    ASSERT_TRUE(success.is_global());
    ASSERT_EQ(GlobalObject::Get(), &success);

    GlobalObject fail(InitStatus::Fail);

    ASSERT_FALSE(fail.is_global());
    ASSERT_EQ(GlobalObject::Get(), &success);
}
