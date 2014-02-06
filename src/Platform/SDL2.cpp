// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/Macros.h"
#ifdef RAINBOW_SDL

#include <SDL_config.h>
#include <SDL.h>
#if SDL_VERSION_ATLEAST(1,3,0)

#ifdef RAINBOW_OS_WINDOWS
#	if defined(_MSC_VER) && defined(NDEBUG)
		// TODO: http://public.kitware.com/Bug/view.php?id=12566
#		pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#	endif
#	include "Graphics/OpenGL.h"
#	include <GL/glew.c>
#endif

#ifdef RAINBOW_TEST
#	include "../tests/test.h"
#endif

#include "Config.h"
#include "Director.h"
#include "Common/Chrono.h"
#include "Common/Data.h"
#include "ConFuoco/Mixer.h"
#include "FileSystem/Path.h"
#include "Graphics/Renderer.h"
#include "Input/Input.h"
#include "Input/Key.h"
#include "Input/Touch.h"

namespace
{
	typedef unsigned int uint_t;

	const double kMsPerFrame = 1000.0 / 60.0;

	bool is_fullscreen(const SDL_Keysym &keysym)
	{
		return keysym.sym == SDLK_RETURN
	#ifdef RAINBOW_OS_MACOS
		    && (keysym.mod & KMOD_LGUI);
	#else
		    && (keysym.mod & KMOD_LALT);
	#endif
	}

	bool is_quit(const SDL_Keysym &keysym)
	{
	#ifndef RAINBOW_OS_MACOS
		return keysym.sym == SDLK_q && (keysym.mod & KMOD_LCTRL);
	#else
		return false;
		static_cast<void>(keysym);
	#endif
	}

	class RenderWindow
	{
	public:
		RenderWindow(const uint_t width, const uint_t height);
		~RenderWindow();

		void swap() const;
		void toggle_fullscreen();

		void on_mouse_down(const SDL_MouseButtonEvent &event,
		                   const unsigned long timestamp);
		void on_mouse_motion(const SDL_MouseMotionEvent &event,
		                     const unsigned long timestamp);
		void on_mouse_up(const SDL_MouseButtonEvent &event,
		                 const unsigned long timestamp);
		void on_window_resized();

		operator bool() const;
		operator SDL_Window*();

	private:
		bool initialised;           ///< Whether the renderer is properly initialised.
		bool vsync;                 ///< Whether vertical sync is enabled.
		uint_t fullscreen;          ///< Whether the window is in full screen mode.
		SDL_GLContext context;      ///< OpenGL context handle.
		SDL_Window *window;         ///< Window handle.
		SDL_Point window_position;  ///< Window's position while windowed.
		Touch mouse;                ///< Current mouse events.
	};
}

int main(int argc, char *argv[])
{
	if (argc < 2)
		Path::set_current();
	else
		Path::set_current(argv[1]);

	// Look for 'main.lua'.
	{
		const Path main("main.lua");
		if (!main.is_file())
		{
		#ifdef RAINBOW_TEST
			Path::set_current(Path());
			testing::InitGoogleTest(&argc, argv);
			return RUN_ALL_TESTS();
		#else
			return 0;
		#endif
		}
	}

	ConFuoco::Mixer mixer;
	if (!ConFuoco::Mixer::Instance)
		return 1;

	uint_t window_width = 1280;
	uint_t window_height = 720;

	Rainbow::Config config;
	if (config.width() && config.height())
	{
		window_width = config.width();
		window_height = config.height();
	}

	RenderWindow window(window_width, window_height);
	if (!window)
		return 1;

	// Load game
	Director director;
	director.init(Data::load_asset("main.lua"), window_width, window_height);

	Chrono chrono;
	SDL_Event event;
	while (!director.terminated())
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					director.terminate();
					break;
				case SDL_WINDOWEVENT:
					switch (event.window.event)
					{
						case SDL_WINDOWEVENT_SIZE_CHANGED:
							window.on_window_resized();
							director.on_focus_gained();
							break;
						case SDL_WINDOWEVENT_FOCUS_GAINED:
							if (config.suspend())
								director.on_focus_gained();
							break;
						case SDL_WINDOWEVENT_FOCUS_LOST:
							if (config.suspend())
								director.on_focus_lost();
							break;
						case SDL_WINDOWEVENT_CLOSE:
							director.terminate();
							break;
						default:
							break;
					}
					break;
				case SDL_KEYDOWN: {
					const SDL_Keysym &keysym = event.key.keysym;
					if (is_quit(keysym))
						director.terminate();
					else if (is_fullscreen(keysym))
					{
						// Unfocus Director while we resize the window to avoid
						// glitches. Focus is restored when we receive an
						// SDL_WINDOWEVENT_SIZE_CHANGED.
						director.on_focus_lost();
						window.toggle_fullscreen();
					}
					else
						Input::Instance->key_down(Key::from_raw(&keysym));
					break;
				}
				case SDL_KEYUP:
					Input::Instance->key_up(Key::from_raw(&event.key.keysym));
					break;
				case SDL_MOUSEMOTION:
					window.on_mouse_motion(event.motion, chrono.current());
					break;
				case SDL_MOUSEBUTTONDOWN:
					window.on_mouse_down(event.button, chrono.current());
					break;
				case SDL_MOUSEBUTTONUP:
					window.on_mouse_up(event.button, chrono.current());
					break;
				default:
					break;
			}
		}
		chrono.update();
		if (!director.active())
		{
			if (director.terminated())
				break;

			Chrono::sleep(static_cast<uint_t>(kMsPerFrame));
		}
		else
		{
			// Update game logic
			director.update(chrono.delta());

			// Draw
			Renderer::clear();
			director.draw();
			window.swap();
		}
	}
	return 0;
}

RenderWindow::RenderWindow(const uint_t width, const uint_t height)
    : initialised(false), vsync(false), fullscreen(0), context(nullptr),
      window(nullptr),
      window_position({ SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED })
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		R_ERROR("[SDL] Unable to initialise video: %s\n", SDL_GetError());
		return;
	}

#ifdef RAINBOW_OS_MACOS
	// Prevent the full screen window from being minimized when losing focus.
	SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
#endif
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);

	this->window = SDL_CreateWindow(
	    RAINBOW_BUILD, this->window_position.x, this->window_position.y,
	    width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!this->window)
	{
		R_ERROR("[SDL] Failed to create window: %s\n", SDL_GetError());
		return;
	}

	this->context = SDL_GL_CreateContext(this->window);
	if (!this->context)
	{
		R_ERROR("[SDL] Failed to create GL context: %s\n", SDL_GetError());
		return;
	}

	SDL_GL_SetSwapInterval(1);
	this->vsync = SDL_GL_GetSwapInterval() == 1;
	R_DEBUG("[SDL] Sync with vertical retrace: %s\n",
	        this->vsync ? "Yes" : "No");

	this->initialised = Renderer::init();
	if (!this->initialised)
	{
		R_ERROR("[Rainbow] Failed to initialize OpenGL\n");
		return;
	}

	Renderer::set_resolution(width, height);
}

RenderWindow::~RenderWindow()
{
	if (!SDL_WasInit(SDL_INIT_VIDEO))
		return;

	if (this->window)
	{
		if (this->context)
		{
			if (this->initialised)
				Renderer::release();
			SDL_GL_DeleteContext(this->context);
		}
		SDL_DestroyWindow(this->window);
	}
	SDL_Quit();
}

void RenderWindow::swap() const
{
	if (!this->vsync)
		Chrono::sleep(0);
	SDL_GL_SwapWindow(this->window);
}

void RenderWindow::toggle_fullscreen()
{
	this->fullscreen ^= SDL_WINDOW_FULLSCREEN_DESKTOP;
#ifdef RAINBOW_OS_MACOS
	// We can't do borderless windowed mode on Mac OS X because the Apple menu
	// is always on top.
	SDL_SetWindowFullscreen(this->window, this->fullscreen);
#else
	SDL_bool bordered = SDL_TRUE;
	const Vec2i &resolution = Renderer::resolution();
	SDL_Rect window_rect{this->window_position.x, this->window_position.y,
	                     resolution.width, resolution.height};
	if (this->fullscreen)
	{
		SDL_GetWindowPosition(
		    this->window, &this->window_position.x, &this->window_position.y);
		bordered = SDL_FALSE;
		SDL_GetDisplayBounds(
		    SDL_GetWindowDisplayIndex(this->window), &window_rect);
		window_rect.x = SDL_WINDOWPOS_CENTERED;
		window_rect.y = SDL_WINDOWPOS_CENTERED;
	}
	SDL_SetWindowBordered(this->window, bordered);
	SDL_SetWindowSize(this->window, window_rect.w, window_rect.h);
	SDL_SetWindowPosition(this->window, window_rect.x, window_rect.y);
#endif
}

void RenderWindow::on_mouse_down(const SDL_MouseButtonEvent &event,
                                 const unsigned long timestamp)
{
	const Vec2i &point =
	    Renderer::convert_to_flipped_view(Vec2i(event.x, event.y));
	this->mouse.x = point.x;
	this->mouse.y = point.y;
	this->mouse.x0 = this->mouse.x;
	this->mouse.y0 = this->mouse.y;
	this->mouse.timestamp = timestamp;
	Input::Instance->touch_began(&this->mouse, 1);
}

void RenderWindow::on_mouse_motion(const SDL_MouseMotionEvent &event,
                                   const unsigned long timestamp)
{
	this->mouse.x0 = this->mouse.x;
	this->mouse.y0 = this->mouse.y;
	const Vec2i &point =
	    Renderer::convert_to_flipped_view(Vec2i(event.x, event.y));
	this->mouse.x = point.x;
	this->mouse.y = point.y;
	this->mouse.timestamp = timestamp;
	Input::Instance->touch_moved(&this->mouse, 1);
}

void RenderWindow::on_mouse_up(const SDL_MouseButtonEvent &event,
                               const unsigned long timestamp)
{
	const Vec2i &point =
	    Renderer::convert_to_flipped_view(Vec2i(event.x, event.y));
	this->mouse.x = point.x;
	this->mouse.y = point.y;
	this->mouse.x0 = this->mouse.x;
	this->mouse.y0 = this->mouse.y;
	this->mouse.timestamp = timestamp;
	Input::Instance->touch_ended(&this->mouse, 1);
}

void RenderWindow::on_window_resized()
{
	int width, height;
	SDL_GetWindowSize(this->window, &width, &height);
	const Vec2i &window_size = Renderer::window_size();
	if (width == window_size.width && height == window_size.height)
		return;
	Renderer::set_window_size(width, height);
}

RenderWindow::operator bool() const
{
	return this->initialised;
}

RenderWindow::operator SDL_Window*()
{
	return this->window;
}

#endif  // SDL_VERSION_ATLEAST(1,3,0)
#endif  // RAINBOW_SDL
