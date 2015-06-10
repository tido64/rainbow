// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/Macros.h"
#ifdef RAINBOW_SDL

#include <tuple>

#include <SDL_config.h>  // Ensure we include the correct SDL_config.h.
#include <SDL.h>

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

#include "Common/Chrono.h"
#include "Common/Data.h"
#include "Config.h"
#include "Director.h"
#include "FileSystem/Path.h"
#include "Input/Controller.h"
#include "Input/Key.h"
#include "Input/Pointer.h"
#ifdef RAINBOW_TEST
#	include "Tests/Tests.h"
#endif

// Mac OS X: Use native fullscreen mode instead.
#define USE_BORDERLESS_WINDOWED_MODE \
	(!defined(RAINBOW_OS_MACOS) && !defined(RAINBOW_JS))

namespace
{
	static_assert(SDL_VERSION_ATLEAST(2,0,3),
	              "Rainbow requires SDL version 2.0.3 or higher");

	using GameController = std::tuple<int, SDL_GameController*>;

	const uint32_t kMouseButtons[] = {
	    SDL_BUTTON_LEFT,
	    SDL_BUTTON_MIDDLE,
	    SDL_BUTTON_RIGHT,
	    SDL_BUTTON_X1,
	    SDL_BUTTON_X2};

	const double kMsPerFrame = 1000.0 / 60.0;

	bool is_fullscreen(const SDL_Keysym &keysym)
	{
#ifdef RAINBOW_OS_MACOS
		return false;
		static_cast<void>(keysym);
#else
		return keysym.sym == SDLK_RETURN && (keysym.mod & KMOD_LALT);
#endif
	}

	bool is_quit(const SDL_Keysym &keysym)
	{
#ifdef RAINBOW_OS_MACOS
		return false;
		static_cast<void>(keysym);
#else
		return keysym.sym == SDLK_q && (keysym.mod & KMOD_LCTRL);
#endif
	}

	bool should_run_tests(int &argc, char **&argv)
	{
#if !defined(USE_LUA_SCRIPT) || USE_LUA_SCRIPT
		static_cast<void>(argc);
		static_cast<void>(argv);
		return !Path("main.lua").is_file();
#else
		const bool run = argc < 2 ? false : strcmp(argv[1], "--test") == 0;
		if (run)
		{
			--argc;
			++argv;
		}
		return run;
#endif  // USE_LUA_SCRIPT
	}

	Vec2i window_size(const rainbow::Config &config)
	{
		return (!config.width() || !config.height())
		    ? Vec2i(1280, 720)
		    : Vec2i(config.width(), config.height());
	}

	class SDLContext
	{
	public:
		SDLContext(const rainbow::Config &config);
		~SDLContext();

		Vec2i drawable_size() const;
		Vec2i window_size() const;

		void swap() const;
		void toggle_fullscreen();

		explicit operator bool() const;

	private:
		SDL_Window *window_;     ///< Window handle.
		bool vsync_;             ///< Whether vertical sync is enabled.
		uint32_t fullscreen_;    ///< Whether the window is in full screen mode.
		SDL_Point position_;     ///< Window's position while windowed.
		Vec2i size_;             ///< Window's size while windowed.
		SDL_GLContext context_;  ///< OpenGL context handle.
	};

	class RainbowController
	{
	public:
		RainbowController(SDLContext &context, const rainbow::Config &config);

		const char* error() const;

		bool run();

		void on_controller_connected(const int device_index);
		void on_controller_disconnected(const int instance_id);
		void on_mouse_down(const uint32_t button,
		                   const Vec2i &point,
		                   const unsigned long timestamp);
		void on_mouse_motion(const uint32_t buttons,
		                     const Vec2i &point,
		                     const unsigned long timestamp);
		void on_mouse_up(const uint32_t button,
		                 const Vec2i &point,
		                 const unsigned long timestamp);
		void on_window_resized();

	private:
		SDLContext &context_;
		Chrono chrono_;
		Director director_;
		const bool suspend_on_focus_lost_;
		std::vector<GameController> game_controllers_;
	};
}

SDLContext::SDLContext(const rainbow::Config &config)
    : window_(nullptr), vsync_(false), fullscreen_(0),
      position_({SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED}),
      size_(::window_size(config)), context_(nullptr)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
	{
		LOGF("SDL: Unable to initialise video: %s", SDL_GetError());
		return;
	}

#ifdef RAINBOW_OS_MACOS
	// Prevent the full screen window from being minimized when losing focus.
	SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
#endif
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
	if (config.msaa() > 0)
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, config.msaa());
	}

	const uint32_t allow_high_dpi = config.high_dpi() ? SDL_WINDOW_ALLOW_HIGHDPI
	                                                  : 0;
	window_ = SDL_CreateWindow(
	    RAINBOW_BUILD, position_.x, position_.y, size_.x, size_.y,
	    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | allow_high_dpi);
	if (!window_)
	{
		R_ABORT("SDL: Failed to create window: %s", SDL_GetError());
		return;
	}

	context_ = SDL_GL_CreateContext(window_);
	if (!context_)
	{
		R_ABORT("SDL: Failed to create GL context: %s", SDL_GetError());
		return;
	}

#ifdef RAINBOW_JS
	vsync_ = true;
#else
	SDL_GL_SetSwapInterval(1);
	vsync_ = SDL_GL_GetSwapInterval() == 1;
#endif

#ifndef NDEBUG
	const Vec2i &resolution = drawable_size();
	LOGI("SDL: Resolution: %ix%i", resolution.x, resolution.y);
	int msaa = 0;
	SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &msaa);
	if (msaa > 0)
		LOGI("SDL: Anti-aliasing: %ix MSAA", msaa);
	else
		LOGI("SDL: Anti-aliasing: Off");
	LOGI("SDL: Vertical sync: %s", vsync_ ? "Yes" : "No");
#endif
}

SDLContext::~SDLContext()
{
	if (!SDL_WasInit(SDL_INIT_VIDEO))
		return;

	if (window_)
	{
		if (context_)
			SDL_GL_DeleteContext(context_);
		SDL_DestroyWindow(window_);
	}
	SDL_Quit();
}

Vec2i SDLContext::drawable_size() const
{
	Vec2i size;
	SDL_GL_GetDrawableSize(window_, &size.x, &size.y);
	return size;
}

Vec2i SDLContext::window_size() const
{
	Vec2i size;
	SDL_GetWindowSize(window_, &size.x, &size.y);
	return size;
}

void SDLContext::swap() const
{
	if (!vsync_)
		Chrono::sleep(0);
	SDL_GL_SwapWindow(window_);
}

void SDLContext::toggle_fullscreen()
{
	fullscreen_ ^= SDL_WINDOW_FULLSCREEN_DESKTOP;

#if !USE_BORDERLESS_WINDOWED_MODE
	SDL_SetWindowFullscreen(window_, fullscreen_);
#else
	SDL_bool bordered = SDL_TRUE;
	SDL_Rect window_rect{position_.x, position_.y, size_.x, size_.y};
	if (fullscreen_)
	{
		SDL_GetWindowPosition(window_, &position_.x, &position_.y);
		bordered = SDL_FALSE;
		SDL_GetDisplayBounds(SDL_GetWindowDisplayIndex(window_), &window_rect);
		window_rect.x = SDL_WINDOWPOS_CENTERED;
		window_rect.y = SDL_WINDOWPOS_CENTERED;
	}
	SDL_SetWindowBordered(window_, bordered);
	SDL_SetWindowSize(window_, window_rect.w, window_rect.h);
	SDL_SetWindowPosition(window_, window_rect.x, window_rect.y);
#endif  // !USE_BORDERLESS_WINDOWED_MODE
}

SDLContext::operator bool() const
{
	return context_;
}

RainbowController::RainbowController(
    SDLContext &context, const rainbow::Config &config)
    : context_(context), suspend_on_focus_lost_(config.suspend())
{
	if (director_.terminated())
		return;

	for (int i = 0; i < SDL_NumJoysticks(); ++i)
		on_controller_connected(i);

	director_.init(context_.drawable_size());
	on_window_resized();
}

const char* RainbowController::error() const
{
	return director_.error();
}

bool RainbowController::run()
{
	if (director_.terminated())
		return false;

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				director_.terminate();
				return false;
			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_SIZE_CHANGED:
						on_window_resized();
						director_.on_focus_gained();
						break;
					case SDL_WINDOWEVENT_FOCUS_GAINED:
						if (suspend_on_focus_lost_)
							director_.on_focus_gained();
						break;
					case SDL_WINDOWEVENT_FOCUS_LOST:
						if (suspend_on_focus_lost_)
							director_.on_focus_lost();
						break;
					case SDL_WINDOWEVENT_CLOSE:
						director_.terminate();
						return false;
					default:
						break;
				}
				break;
			case SDL_KEYDOWN:
				if (event.key.repeat == 0)
				{
					const SDL_Keysym &keysym = event.key.keysym;
					if (is_quit(keysym))
					{
						director_.terminate();
						return false;
					}
					if (is_fullscreen(keysym))
					{
						// Unfocus Director while we resize the window to avoid
						// glitches. Focus is restored when we receive an
						// SDL_WINDOWEVENT_SIZE_CHANGED.
						director_.on_focus_lost();
						context_.toggle_fullscreen();
					}
					else
						director_.input().on_key_down(Key::from_raw(&keysym));
				}
				break;
			case SDL_KEYUP:
				director_.input().on_key_up(Key::from_raw(&event.key.keysym));
				break;
			case SDL_MOUSEMOTION:
				on_mouse_motion(event.motion.state,
				                director_.renderer().convert_to_flipped_view(
				                    Vec2i(event.motion.x, event.motion.y)),
				                event.motion.timestamp);
				break;
			case SDL_MOUSEBUTTONDOWN:
				on_mouse_down(event.button.button,
				              director_.renderer().convert_to_flipped_view(
				                  Vec2i(event.button.x, event.button.y)),
				              event.button.timestamp);
				break;
			case SDL_MOUSEBUTTONUP:
				on_mouse_up(event.button.button,
				            director_.renderer().convert_to_flipped_view(
				                Vec2i(event.button.x, event.button.y)),
				            event.button.timestamp);
				break;
			case SDL_CONTROLLERAXISMOTION:
				director_.input().on_controller_axis_motion(
				    ControllerAxisMotion(
				        event.caxis.which,
				        static_cast<Controller::Axis>(event.caxis.axis),
				        event.caxis.value, event.caxis.timestamp));
				break;
			case SDL_CONTROLLERBUTTONDOWN:
				director_.input().on_controller_button_down(ControllerButton(
				    event.cbutton.which,
				    static_cast<Controller::Button>(event.cbutton.button),
				    event.cbutton.timestamp));
				break;
			case SDL_CONTROLLERBUTTONUP:
				director_.input().on_controller_button_up(ControllerButton(
				    event.cbutton.which,
				    static_cast<Controller::Button>(event.cbutton.button),
				    event.cbutton.timestamp));
				break;
			case SDL_CONTROLLERDEVICEADDED:
				on_controller_connected(event.cdevice.which);
				break;
			case SDL_CONTROLLERDEVICEREMOVED:
				on_controller_disconnected(event.cdevice.which);
				break;
			default:
				break;
		}
	}
	chrono_.update();
	if (!director_.active())
		Chrono::sleep(static_cast<unsigned int>(kMsPerFrame));
	else
	{
		// Update game logic
		director_.update(chrono_.delta());

		// Draw
		director_.draw();
		context_.swap();
	}
	return true;
}

void RainbowController::on_controller_connected(const int device_index)
{
	if (!SDL_IsGameController(device_index))
		return;

	auto controller = SDL_GameControllerOpen(device_index);
	auto id = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller));

#if defined(RAINBOW_OS_MACOS) && !SDL_VERSION_ATLEAST(2,0,4)
	if (std::any_of(game_controllers_.cbegin(),
	                game_controllers_.cend(),
	                [id](const GameController &controller) {
	                    return std::get<0>(controller) == id;
	                }))
		return;
#endif  // See https://bugzilla.libsdl.org/show_bug.cgi?id=2869

	game_controllers_.emplace_back(id, controller);
	director_.input().on_controller_connected(id);
}

void RainbowController::on_controller_disconnected(const int instance_id)
{
	for (auto i = game_controllers_.begin(); i < game_controllers_.end(); ++i)
	{
		if (std::get<0>(*i) == instance_id)
		{
			auto controller = std::get<1>(*i);
			if (SDL_GameControllerGetAttached(controller))
				SDL_GameControllerClose(controller);

			rainbow::quick_erase(game_controllers_, i);
			director_.input().on_controller_disconnected(instance_id);
			break;
		}
	}
}

void RainbowController::on_mouse_down(const uint32_t button,
                                      const Vec2i &point,
                                      const unsigned long timestamp)
{
	Pointer p(button, point.x, point.y, timestamp);
	director_.input().on_pointer_began(1, &p);
}

void RainbowController::on_mouse_motion(const uint32_t buttons,
                                        const Vec2i &point,
                                        const unsigned long timestamp)
{
	if (buttons > 0)
	{
		Pointer p[SDL_BUTTON_X2];
		size_t i = 0;
		for (auto button : kMouseButtons)
		{
			if (buttons & SDL_BUTTON(button))
			{
				p[i] = Pointer(button, point.x, point.y, timestamp);
				++i;
			}
		}
		director_.input().on_pointer_moved(i, p);
	}
	else
	{
		Pointer p(0, point.x, point.y, timestamp);
		director_.input().on_pointer_moved(1, &p);
	}
}

void RainbowController::on_mouse_up(const uint32_t button,
                                    const Vec2i &point,
                                    const unsigned long timestamp)
{
	Pointer p(button, point.x, point.y, timestamp);
	director_.input().on_pointer_ended(1, &p);
}

void RainbowController::on_window_resized()
{
	const Vec2i &size = context_.window_size();
	if (size == director_.renderer().window_size())
		return;

	const Vec2i &viewport = context_.drawable_size();
	director_.renderer().set_window_size(size, viewport.x / size.x);
}

#ifdef RAINBOW_JS

SDLContext *g_context = nullptr;
RainbowController *g_controller = nullptr;

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

int main(int argc, char *argv[])
{
	if (argc < 2)
		Path::set_current();
	else
		Path::set_current(argv[1]);

	if (should_run_tests(argc, argv))
	{
#ifdef RAINBOW_TEST
		Path::set_current(Path());
		return rainbow::run_tests(argc, argv);
#else
		return 0;
#endif  // RAINBOW_TEST
	}

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
#endif  // RAINBOW_SDL
