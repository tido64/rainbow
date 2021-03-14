// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Tests/Tests.h"

#include <string_view>

#include "Platform/Macros.h"
#ifdef RAINBOW_OS_WINDOWS
#    include <objbase.h>
#endif
#include <gtest/gtest.h>

#include "Common/Random.h"

using namespace std::literals::string_view_literals;
using rainbow::out;

#ifdef RAINBOW_OS_WINDOWS

namespace
{
    struct CoLibrary {
        CoLibrary() { CoInitializeEx(nullptr, COINIT_MULTITHREADED); }
        ~CoLibrary() { CoUninitialize(); }
    };
}  // namespace

#    define CO_INIT() CoLibrary com
#else
#    define CO_INIT()
#endif

auto rainbow::run_tests(int argc, char* argv[]) -> int
{
    CO_INIT();
    random.seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

bool rainbow::should_run_tests([[maybe_unused]] out<int> argc,
                               [[maybe_unused]] out<char**> argv)
{
    if (argc < 2)
        return false;

    std::string_view first_flag(argv[1]);

    // Check for --test flag.
    if (first_flag == "--test"sv) {
        --argc;
        ++argv;
        return true;
    }

    // Check for Google Test flags.
    constexpr auto kGTestFlag = "--gtest"sv;
    return first_flag.length() >= kGTestFlag.length() &&
           kGTestFlag.compare(
               0, kGTestFlag.length(), first_flag, 0, kGTestFlag.length()) == 0;
}
