// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/Macros.h"
#if defined(RAINBOW_JS)
#   pragma clang diagnostic ignored "-Wunused-function"
#   include <emscripten.h>
#elif defined(RAINBOW_OS_WINDOWS)
#   if defined(_MSC_VER) && defined(NDEBUG)
        // TODO: https://gitlab.kitware.com/cmake/cmake/issues/12566
#       pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#   endif
#   include "Graphics/OpenGL.h"
#   include <GL/glew.c>
#endif

#include "Common/Functional.h"
#include "Config.h"
#include "FileSystem/FileSystem.h"
#include "Platform/Diagnostics.h"
#include "Platform/SDL/Context.h"
#include "Platform/SDL/RainbowController.h"
#ifdef RAINBOW_TEST
#   include "Tests/Tests.h"
#endif

using rainbow::RainbowController;
using rainbow::SDLContext;
using rainbow::out;

namespace
{
    auto run_tests([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
        -> int
    {
#ifdef RAINBOW_TEST
        return rainbow::run_tests(argc, argv);
#else
        return 0;
#endif  // RAINBOW_TEST
    }

    bool should_run_tests([[maybe_unused]] out<int> argc,
                          [[maybe_unused]] out<char**> argv)
    {
#ifdef RAINBOW_TEST
        if (argc < 2)
            return false;

        // Check for --test flag.
        if (strcmp(argv[1], "--test") == 0)
        {
            --argc;
            ++argv;
            return true;
        }

        // Check for Google Test flags.
        constexpr char kGTestFlag[] = "--gtest";
        constexpr size_t kGTestFlagLength = rainbow::array_size(kGTestFlag) - 1;
        return strncmp(argv[1], kGTestFlag, kGTestFlagLength) == 0;
#else
        return false;
#endif  // RAINBOW_TEST
    }
}

#ifdef RAINBOW_JS

SDLContext* g_context = nullptr;
RainbowController* g_controller = nullptr;

void emscripten_main() { g_controller->run(); }

auto main() -> int
{
    const rainbow::Config config;
    g_context = std::make_unique<SDLContext>(config).release();
    g_controller =
        std::make_unique<RainbowController>(*g_context, config).release();
    emscripten_set_main_loop(&emscripten_main, 0, 1);
    return 0;
}

#else

auto main(int argc, char* argv[]) -> int
{
    rainbow::filesystem::initialize({argv, static_cast<size_t>(argc)});

    if (should_run_tests(std::ref(argc), std::ref(argv)))
    {
        rainbow::diagnostics::attach_crash_dumper();
        return run_tests(argc, argv);
    }

    const rainbow::Config config;
    SDLContext context(config);
    if (!context)
        return 1;

    RainbowController controller(context, config);
    while (controller.run()) {}
    return controller.error() != nullptr;
}

#endif  // RAINBOW_JS
