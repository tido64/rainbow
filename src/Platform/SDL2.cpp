// Copyright 2013 Bifrost Entertainment. All rights reserved.

#include "Platform/Macros.h"
#ifdef RAINBOW_SDL

#include <SDL_config.h>
#include <SDL.h>
#if SDL_VERSION_ATLEAST(1,3,0)

#ifdef RAINBOW_OS_WINDOWS
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

		inline void swap();

		void on_mouse_down(const SDL_MouseButtonEvent &event,
		                   const unsigned long timestamp);
		void on_mouse_motion(const SDL_MouseMotionEvent &event,
		                     const unsigned long timestamp);
		void on_mouse_up(const SDL_MouseButtonEvent &event,
		                 const unsigned long timestamp);

		inline operator bool() const;

	private:
		bool initialised;
		bool vsync;
		const uint_t height;
		SDL_GLContext context;
		SDL_Window *window;
		Touch mouse;
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

	uint_t screen_width = 1280;
	uint_t screen_height = 720;

	Rainbow::Config config;
	if (config.width() && config.height())
	{
		screen_width = config.width();
		screen_height = config.height();
	}

	RenderWindow window(screen_width, screen_height);
	if (!window)
		return 1;

	// Load game
	Director director;
	director.init(Data::load_asset("main.lua"), screen_width, screen_height);

	Chrono chrono;
	SDL_Event event;
	while (!director.has_terminated())
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
						case SDL_WINDOWEVENT_FOCUS_GAINED:
							if (config.suspend())
							{
								director.activate();
								ConFuoco::Mixer::Instance->suspend(false);
							}
							break;
						case SDL_WINDOWEVENT_FOCUS_LOST:
							if (config.suspend())
							{
								director.deactivate();
								ConFuoco::Mixer::Instance->suspend(true);
							}
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
		if (!director.is_active())
		{
			if (director.has_terminated())
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

RenderWindow::RenderWindow(const uint_t width, const uint_t height) :
	initialised(false), vsync(false), height(height), context(nullptr),
	window(nullptr)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		R_ERROR("[SDL] Unable to initialise video: %s\n", SDL_GetError());
		return;
	}

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);

	this->window = SDL_CreateWindow(
			RAINBOW_BUILD, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
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
		R_ERROR("[Rainbow] Failed to initialise OpenGL\n");
		return;
	}

	Renderer::resize(width, height);
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

void RenderWindow::swap()
{
	if (!this->vsync)
		Chrono::sleep(0);
	SDL_GL_SwapWindow(this->window);
}

void RenderWindow::on_mouse_down(const SDL_MouseButtonEvent &event,
                                 const unsigned long timestamp)
{
	this->mouse.x = event.x;
	this->mouse.y = this->height - event.y;
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
	this->mouse.x = event.x;
	this->mouse.y = this->height - event.y;
	this->mouse.timestamp = timestamp;
	Input::Instance->touch_moved(&this->mouse, 1);
}

void RenderWindow::on_mouse_up(const SDL_MouseButtonEvent &event,
                               const unsigned long timestamp)
{
	this->mouse.x = event.x;
	this->mouse.y = this->height - event.y;
	this->mouse.x0 = this->mouse.x;
	this->mouse.y0 = this->mouse.y;
	this->mouse.timestamp = timestamp;
	Input::Instance->touch_ended(&this->mouse, 1);
}

RenderWindow::operator bool() const
{
	return this->initialised;
}

#endif  // SDL_VERSION_ATLEAST(1,3,0)
#endif  // RAINBOW_SDL
