// Copyright 2013 Bifrost Entertainment. All rights reserved.

#include "Platform/Definitions.h"
#ifdef RAINBOW_SDL

#include <SDL.h>
#if SDL_VERSION_ATLEAST(2,0,0)

#ifdef RAINBOW_WIN
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
#include "Graphics/Renderer.h"
#include "Input/Key.h"
#include "Input/Input.h"
#include "Input/Touch.h"

bool active = true;  ///< Whether the window is in focus.
bool done = false;   ///< Whether the user has requested to quit.

unsigned int screen_width = 1280;    ///< Window width.
unsigned int screen_height = 720;    ///< Window height.
const double fps = 1000.0 / 60.0;    ///< Preferred frames per second.
const float milli = 1.0f / 1000.0f;  ///< 1 millisecond.
Touch mouse_input;                   ///< Mouse input.
Chrono chrono;                       ///< Clock.

char data_path[256] = { 0 };
char userdata_path[256] = { 0 };

void on_mouse_button_down(SDL_MouseButtonEvent &);  ///< Handle mouse button down event.
void on_mouse_button_up(SDL_MouseButtonEvent &);    ///< Handle mouse button up event.
void on_mouse_motion(SDL_MouseMotionEvent &);       ///< Handle mouse motion event.
int sdl_exit(const char *const msg = nullptr,
             SDL_Window *window = nullptr,
             SDL_GLContext context = nullptr);


int main(int argc, char *argv[])
{
	if (argc < 2)
	{
	#ifdef RAINBOW_TEST
		testing::InitGoogleTest(&argc, argv);
		return RUN_ALL_TESTS();
	#else
		return 0;
	#endif
	}
	strcpy(data_path, argv[1]);  // Set data path
	strcat(data_path, "/");
	strcpy(userdata_path, data_path);  // Set user data path
	strcat(userdata_path, "user/");

	ConFuoco::Mixer mixer;
	if (!ConFuoco::Mixer::Instance)
		return 1;

	Rainbow::Config config;
	if (config.get_width() && config.get_height())
	{
		screen_width = config.get_width();
		screen_height = config.get_height();
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		R_ERROR("[SDL] Unable to initialise video: %s\n", SDL_GetError());
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);

	SDL_Window *window = SDL_CreateWindow(
			RAINBOW_BUILD, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			screen_width, screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!window)
		return sdl_exit("[SDL] Failed to create window: %s\n");

	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (!context)
		return sdl_exit("[SDL] Failed to create GL context: %s\n", window);

	if (!Renderer::init())
	{
		R_ERROR("[Rainbow] Failed to initialise OpenGL\n");
		Renderer::release();
		sdl_exit(nullptr, window, context);
		return 1;
	}
	Renderer::resize(screen_width, screen_height);

	// Load game
	Director director;
	director.init(Data("main.lua"), screen_width, screen_height);

	chrono.update();
	while (!done)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					active = false;
					done = true;
					break;
				case SDL_WINDOWEVENT:
					switch (event.type)
					{
						case SDL_WINDOWEVENT_FOCUS_GAINED:
							active = true;
							ConFuoco::Mixer::Instance->suspend(false);
							break;
						case SDL_WINDOWEVENT_FOCUS_LOST:
							active = false;
							ConFuoco::Mixer::Instance->suspend(true);
							break;
						case SDL_WINDOWEVENT_CLOSE:
							active = false;
							done = true;
							break;
						default:
							break;
					}
					break;
				case SDL_KEYDOWN:
				#ifndef RAINBOW_MAC
					if (event.key.keysym.sym == SDLK_q && (event.key.keysym.mod & KMOD_LCTRL))
					{
						active = false;
						done = true;
						break;
					}
				#endif
					Input::Instance->key_down(Key::from_raw(&event.key.keysym));
					break;
				case SDL_KEYUP:
					Input::Instance->key_up(Key::from_raw(&event.key.keysym));
					break;
				case SDL_MOUSEMOTION:
					on_mouse_motion(event.motion);
					break;
				case SDL_MOUSEBUTTONDOWN:
					on_mouse_button_down(event.button);
					break;
				case SDL_MOUSEBUTTONUP:
					on_mouse_button_up(event.button);
					break;
				default:
					break;
			}
		}
		chrono.update();
		if (!active)
		{
			if (done)
				break;

			SDL_Delay(static_cast<Uint32>(fps));
		}
		else
		{
			// Update game logic
			director.update(chrono.diff());

			// Draw
			Renderer::clear();
			director.draw();
			SDL_GL_SwapWindow(window);
		}
	}
	Renderer::release();
	return sdl_exit(nullptr, window, context);
}

void on_mouse_button_down(SDL_MouseButtonEvent &mouse)
{
	mouse_input.x = mouse.x;
	mouse_input.y = screen_height - mouse.y;
	mouse_input.x0 = mouse_input.x;
	mouse_input.y0 = mouse_input.y;
	mouse_input.timestamp = chrono.current();
	Input::Instance->touch_began(&mouse_input, 1);
}

void on_mouse_button_up(SDL_MouseButtonEvent &mouse)
{
	mouse_input.x = mouse.x;
	mouse_input.y = screen_height - mouse.y;
	mouse_input.x0 = mouse_input.x;
	mouse_input.y0 = mouse_input.y;
	mouse_input.timestamp = chrono.current();
	Input::Instance->touch_ended(&mouse_input, 1);
}

void on_mouse_motion(SDL_MouseMotionEvent &mouse)
{
	mouse_input.x0 = mouse_input.x;
	mouse_input.y0 = mouse_input.y;
	mouse_input.x = mouse.x;
	mouse_input.y = screen_height - mouse.y;
	mouse_input.timestamp = chrono.current();
	Input::Instance->touch_moved(&mouse_input, 1);
}

int sdl_exit(const char *const msg, SDL_Window *window, SDL_GLContext context)
{
	if (msg)
		R_ERROR(msg, SDL_GetError());
	if (window)
	{
		if (context)
			SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
	}
	SDL_Quit();
	return msg ? 1 : 0;
}

#endif  // SDL_VERSION_ATLEAST(2,0,0)
#endif  // RAINBOW_SDL
