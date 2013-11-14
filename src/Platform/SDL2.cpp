// Copyright 2013 Bifrost Entertainment. All rights reserved.

#include "Platform/Macros.h"
#ifdef RAINBOW_SDL

#include <SDL_config.h>
#include <SDL.h>
#if SDL_VERSION_ATLEAST(1,3,0)

#ifdef RAINBOW_OS_WINDOWS
#	include <direct.h>
#	include "Graphics/OpenGL.h"
#	include <GL/glew.c>
#	define kPathSeparator "\\"
#	define getcwd(buf, size) _getcwd(buf, size)
#else
#	include <unistd.h>
#	define kPathSeparator "/"
#endif

#ifdef RAINBOW_TEST
#	include "../tests/test.h"
#endif

#include "Config.h"
#include "Director.h"
#include "Common/Chrono.h"
#include "Common/Data.h"
#include "ConFuoco/Mixer.h"
#include "Graphics/Renderer.h"
#include "Input/Input.h"
#include "Input/Key.h"
#include "Input/Touch.h"

char data_path[256] = { 0 };
char userdata_path[256] = { 0 };

namespace
{
	typedef unsigned int uint_t;

	const double kFramesPerSecond = 1000.0 / 60.0;
	Chrono chrono;

	void setcwd(const char *const path)
	{
		strcpy(data_path, path);  // Set data path
		strcat(data_path, kPathSeparator);
		strcpy(userdata_path, data_path);  // Set user data path
		strcat(userdata_path, "user");
	}

	class RenderWindow
	{
	public:
		RenderWindow(const uint_t width, const uint_t height);
		~RenderWindow();

		inline void swap();

		void on_mouse_down(const SDL_MouseButtonEvent &event);
		void on_mouse_motion(const SDL_MouseMotionEvent &event);
		void on_mouse_up(const SDL_MouseButtonEvent &event);

		inline operator bool() const;

	private:
		bool initialised;
		bool vsync;
		const uint_t height;
		SDL_GLContext context;
		SDL_Window *window;
		Touch mouse;
	};

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
		if (this->initialised)
			Renderer::release();
		if (this->window)
		{
			if (this->context)
				SDL_GL_DeleteContext(this->context);
			SDL_DestroyWindow(this->window);
		}
		if (SDL_WasInit(SDL_INIT_VIDEO))
			SDL_Quit();
	}

	void RenderWindow::swap()
	{
		if (!this->vsync)
			Chrono::sleep(0);
		SDL_GL_SwapWindow(this->window);
	}

	void RenderWindow::on_mouse_down(const SDL_MouseButtonEvent &event)
	{
		this->mouse.x = event.x;
		this->mouse.y = this->height - event.y;
		this->mouse.x0 = this->mouse.x;
		this->mouse.y0 = this->mouse.y;
		this->mouse.timestamp = chrono.current();
		Input::Instance->touch_began(&this->mouse, 1);
	}

	void RenderWindow::on_mouse_motion(const SDL_MouseMotionEvent &event)
	{
		this->mouse.x0 = this->mouse.x;
		this->mouse.y0 = this->mouse.y;
		this->mouse.x = event.x;
		this->mouse.y = this->height - event.y;
		this->mouse.timestamp = chrono.current();
		Input::Instance->touch_moved(&this->mouse, 1);
	}

	void RenderWindow::on_mouse_up(const SDL_MouseButtonEvent &event)
	{
		this->mouse.x = event.x;
		this->mouse.y = this->height - event.y;
		this->mouse.x0 = this->mouse.x;
		this->mouse.y0 = this->mouse.y;
		this->mouse.timestamp = chrono.current();
		Input::Instance->touch_ended(&this->mouse, 1);
	}

	RenderWindow::operator bool() const
	{
		return this->initialised;
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		const size_t size = 256;
		char cwd[size];
		if (getcwd(cwd, size) != cwd)
			return 0;
		setcwd(cwd);
		strcat(cwd, kPathSeparator "main.lua");
		FILE *file = fopen(cwd, "rb");
		if (!file)
		{
		#ifdef RAINBOW_TEST
			testing::InitGoogleTest(&argc, argv);
			return RUN_ALL_TESTS();
		#else
			return 0;
		#endif
		}
		fclose(file);
	}
	else
		setcwd(argv[1]);

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
	director.init(Data("main.lua"), screen_width, screen_height);

	chrono.update();
	while (!director.has_terminated())
	{
		SDL_Event event;
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
				case SDL_KEYDOWN:
				#ifndef RAINBOW_OS_MACOS
					if (event.key.keysym.sym == SDLK_q && (event.key.keysym.mod & KMOD_LCTRL))
					{
						director.terminate();
						break;
					}
				#endif
					Input::Instance->key_down(Key::from_raw(&event.key.keysym));
					break;
				case SDL_KEYUP:
					Input::Instance->key_up(Key::from_raw(&event.key.keysym));
					break;
				case SDL_MOUSEMOTION:
					window.on_mouse_motion(event.motion);
					break;
				case SDL_MOUSEBUTTONDOWN:
					window.on_mouse_down(event.button);
					break;
				case SDL_MOUSEBUTTONUP:
					window.on_mouse_up(event.button);
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

			Chrono::sleep(static_cast<uint_t>(kFramesPerSecond));
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

#undef kPathSeparator
#endif  // SDL_VERSION_ATLEAST(1,3,0)
#endif  // RAINBOW_SDL
