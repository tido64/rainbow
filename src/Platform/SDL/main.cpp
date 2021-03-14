// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef NDEBUG
#    include <absl/debugging/failure_signal_handler.h>
#    include <absl/debugging/symbolize.h>
#endif

#include "Platform/Macros.h"
#if defined(RAINBOW_JS)
#    pragma clang diagnostic ignored "-Wunused-function"
#    include <emscripten.h>
#elif defined(RAINBOW_OS_WINDOWS)
#    if defined(_MSC_VER) && defined(NDEBUG)
// TODO: https://gitlab.kitware.com/cmake/cmake/issues/12566
#        pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#    endif
#    include "Graphics/OpenGL.h"
#    include "Platform/Windows/Console.h"
#endif

#include "Common/Functional.h"
#include "Config.h"
#include "FileSystem/Bundle.h"
#include "FileSystem/FileSystem.h"
#include "Platform/SDL/Context.h"
#include "Platform/SDL/RainbowController.h"
#ifdef RAINBOW_TEST
#    include "Tests/Tests.h"
#endif

using rainbow::Bundle;
using rainbow::RainbowController;
using rainbow::SDLContext;

#ifdef RAINBOW_JS

SDLContext* g_context = nullptr;
RainbowController* g_controller = nullptr;

void emscripten_main()
{
    g_controller->run();
}

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
#    ifndef NDEBUG
    absl::InitializeSymbolizer(argv[0]);
    absl::InstallFailureSignalHandler(absl::FailureSignalHandlerOptions{});
#    endif

    const Bundle bundle({argv, static_cast<size_t>(argc)});
    rainbow::filesystem::initialize(bundle, argv[0], true);

#    ifdef RAINBOW_TEST
    if (rainbow::should_run_tests(std::ref(argc), std::ref(argv)))
        return rainbow::run_tests(argc, argv);
#    endif

#    ifdef RAINBOW_OS_WINDOWS
    rainbow::windows::Console console;
#    endif

    const rainbow::Config config;
    SDLContext context(config);
    if (!context)
        return 1;

    RainbowController controller(context, config);
    while (controller.run()) {
    }
    return controller.error().value();
}

#endif  // RAINBOW_JS
