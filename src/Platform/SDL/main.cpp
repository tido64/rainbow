// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/Macros.h"
#if defined(RAINBOW_JS)
#	pragma clang diagnostic ignored "-Wunused-function"
#	include <emscripten.h>
#elif defined(RAINBOW_OS_WINDOWS)
#	if defined(_MSC_VER) && defined(NDEBUG)
		// TODO: http://public.kitware.com/Bug/view.php?id=12566
#		pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#	endif
#	include "Graphics/OpenGL.h"
#	include <GL/glew.c>
#endif

#include "Config.h"
#include "Director.h"
#include "FileSystem/Path.h"
#include "Platform/SDL/Context.h"
#include "Platform/SDL/RainbowController.h"
#ifdef RAINBOW_TEST
#	include "Tests/Tests.h"
#endif

namespace
{
	int run_tests(int& argc, char**& argv)
	{
#ifdef RAINBOW_TEST
		Path::set_current(Path());
		return rainbow::run_tests(argc, argv);
#else
		return 0;
		static_cast<void>(argc);
		static_cast<void>(argv);
#endif  // RAINBOW_TEST
	}

	bool should_run_tests(int& argc, char**& argv)
	{
#if USE_LUA_SCRIPT
		return !Path("main.lua").is_file();
		static_cast<void>(argc);
		static_cast<void>(argv);
#else
		const bool run = (argc < 2 ? false : strcmp(argv[1], "--test") == 0);
		if (run)
		{
			--argc;
			++argv;
		}
		return run;
#endif  // USE_LUA_SCRIPT
	}
}

#ifdef RAINBOW_JS

SDLContext* g_context = nullptr;
RainbowController* g_controller = nullptr;

void emscripten_main() { g_controller->run(); }

int main()
{
	const rainbow::Config config;
	g_context = new SDLContext(config);
	g_controller = new RainbowController(*g_context, config);
	emscripten_set_main_loop(&emscripten_main, 0, 1);
	return 0;
}

#else

int main(int argc, char* argv[])
{
	if (argc < 2)
		Path::set_current();
	else
		Path::set_current(argv[1]);

	if (should_run_tests(argc, argv))
		return run_tests(argc, argv);

	const rainbow::Config config;
	SDLContext context(config);
	if (!context)
		return 1;

	RainbowController controller(context, config);
	while (controller.run()) {}
	if (controller.error())
	{
		LOGF("%s", controller.error());
		return 1;
	}
	return 0;
}

#endif  // RAINBOW_JS
