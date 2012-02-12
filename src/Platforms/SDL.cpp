// Copyright 2010-12 Bifrost Entertainment. All rights reserved.

#include "Platform.h"
#ifdef RAINBOW_SDL

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

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

bool active = true;  ///< Whether the window is in focus
bool done = false;   ///< Whether the user has requested to quit

unsigned int screen_width = 960;     ///< Window width
unsigned int screen_height = 640;    ///< Window height
const double fps = 1000.0 / 60.0;    ///< Preferred frames per second
const float milli = 1.0f / 1000.0f;  ///< 1 millisecond
Touch mouse_input;                   ///< Mouse input

void on_mouse_button_down(SDL_MouseButtonEvent &);  ///< Handle mouse button down event
void on_mouse_button_up(SDL_MouseButtonEvent &);    ///< Handle mouse button up event
void on_mouse_motion(SDL_MouseMotionEvent &);       ///< Handle mouse motion event
void resize(Director &, const int w, const int h);  ///< Handle window resize event


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
	Data::set_datapath(argv[1]);  // Set data path

	if (!ConFuoco::Mixer::Instance().is_available())
		return 1;

	Config config;
	if (config.get_width() && config.get_height())
	{
		screen_width = config.get_width();
		screen_height = config.get_height();
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "Unable to initialise SDL: %s\n", SDL_GetError());
		exit(1);
	}

	const int video_mode = SDL_OPENGL;
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_Surface *surface = SDL_SetVideoMode(screen_width, screen_height, 0, video_mode);
	if (!surface)
	{
		fprintf(stderr, "SDL unable to set video mode: %s\n", SDL_GetError());
		SDL_Quit();
		exit(1);
	}
	SDL_WM_SetCaption(RAINBOW_BUILD, "Rainbow");

	if (!Renderer::init())
	{
		fprintf(stderr, "[Rainbow] Failed to initialise OpenGL\n");
		Renderer::release();
		SDL_Quit();
		exit(1);
	}

	Director director;
	resize(director, screen_width, screen_height);

	// Load game
	const char *const path = Data::get_path("main.lua");
	director.init(path);
	Data::free(path);

	mouse_input.hash = 1;
	Chrono::Instance().update();
	while (!done)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_ACTIVEEVENT:
					active = (event.active.gain != 0);
					break;
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_q && (event.key.keysym.mod & KMOD_LCTRL))
					{
						active = false;
						done = true;
						break;
					}
					Input::Instance().key_down(Key::from_raw(&event.key.keysym));
					break;
				case SDL_KEYUP:
					Input::Instance().key_up(Key::from_raw(&event.key.keysym));
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
				case SDL_QUIT:
					active = false;
					done = true;
					break;
				case SDL_VIDEORESIZE:
					surface = SDL_SetVideoMode(event.resize.w, event.resize.h, 0, video_mode);
					if (!surface)
					{
						fprintf(stderr, "SDL unable to set video mode: %s\n", SDL_GetError());
						active = false;
						done = true;
					}
					resize(director, event.resize.w, event.resize.h);
					break;
				default:
					break;
			}
		}
		if (!active)
		{
			if (done)
				break;

			Chrono::Instance().update();
			SDL_Delay(static_cast<Uint32>(fps));
		}
		else
		{
			// Update game logic
			director.update();

			// Draw
			Renderer::clear();
			director.draw();
			SDL_GL_SwapBuffers();
		}
	}
	Renderer::release();
	SDL_Quit();
	return 0;
}

void on_mouse_button_down(SDL_MouseButtonEvent &mouse)
{
	mouse_input.x = mouse.x;
	mouse_input.y = mouse.y;
	Input::Instance().touch_began(&mouse_input, 1);
}

void on_mouse_button_up(SDL_MouseButtonEvent &mouse)
{
	mouse_input.x = mouse.x;
	mouse_input.y = mouse.y;
	Input::Instance().touch_ended(&mouse_input, 1);
}

void on_mouse_motion(SDL_MouseMotionEvent &mouse)
{
	mouse_input.x = mouse.x;
	mouse_input.y = mouse.y;
	Input::Instance().touch_moved(&mouse_input, 1);
}

void resize(Director &director, const int w, const int h)
{
	Renderer::resize(w, h);
	director.set_video(w, h);
}

#endif
