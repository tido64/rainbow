// Copyright 2010-13 Bifrost Entertainment. All rights reserved.

#include "Platform/Definitions.h"
#ifdef RAINBOW_SDL

#include <SDL.h>
#if SDL_COMPILEDVERSION < SDL_VERSIONNUM(2,0,0)

#ifdef RAINBOW_WIN
#	include "Graphics/OpenGL.h"
#	include <GL/glew.c>
#endif

#ifdef RAINBOW_MAC
#	define R_META KMOD_META
#else
#	define R_META KMOD_LCTRL
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

bool active = true;  ///< Whether the window is in focus
bool done = false;   ///< Whether the user has requested to quit

unsigned int screen_width = 960;     ///< Window width
unsigned int screen_height = 640;    ///< Window height
const double fps = 1000.0 / 60.0;    ///< Preferred frames per second
const float milli = 1.0f / 1000.0f;  ///< 1 millisecond
Touch mouse_input;                   ///< Mouse input
Chrono chrono;                       ///< Clock.

char data_path[256] = { 0 };
char userdata_path[256] = { 0 };

void on_mouse_button_down(SDL_MouseButtonEvent &);  ///< Handle mouse button down event
void on_mouse_button_up(SDL_MouseButtonEvent &);    ///< Handle mouse button up event
void on_mouse_motion(SDL_MouseMotionEvent &);       ///< Handle mouse motion event
void resize(Director &, const int w, const int h);  ///< Handle window resize event
int  sdl_eventfilter(const SDL_Event *);            ///< Filters SDL events


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
		R_ERROR("Unable to initialise SDL: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_SetEventFilter(&sdl_eventfilter);

	const int video_mode = SDL_OPENGL;
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_Surface *surface = SDL_SetVideoMode(screen_width, screen_height, 0, video_mode);
	if (!surface)
	{
		R_ERROR("SDL unable to set video mode: %s\n", SDL_GetError());
		SDL_Quit();
		exit(1);
	}
	SDL_WM_SetCaption(RAINBOW_BUILD, "Rainbow");

	if (!Renderer::init())
	{
		R_ERROR("[Rainbow] Failed to initialise OpenGL\n");
		Renderer::release();
		SDL_Quit();
		exit(1);
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
				case SDL_ACTIVEEVENT:
					active = (event.active.gain != 0);
					ConFuoco::Mixer::Instance->suspend(!active);
					break;
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_q && (event.key.keysym.mod & R_META))
					{
						active = false;
						done = true;
						break;
					}
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
				case SDL_QUIT:
					active = false;
					done = true;
					break;
				case SDL_VIDEORESIZE:
					surface = SDL_SetVideoMode(event.resize.w, event.resize.h, 0, video_mode);
					if (!surface)
					{
						R_ERROR("SDL unable to set video mode: %s\n", SDL_GetError());
						active = false;
						done = true;
					}
					resize(director, event.resize.w, event.resize.h);
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

void resize(Director &director, const int w, const int h)
{
	Renderer::resize(w, h);
	director.set_video(w, h);
}

int sdl_eventfilter(const SDL_Event *e)
{
	switch (e->type)
	{
		case SDL_ACTIVEEVENT:
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		case SDL_MOUSEMOTION:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		case SDL_QUIT:
		case SDL_VIDEORESIZE:
			return 1;
		default:
			return 0;
	}
}

#endif  // SDL_COMPILEDVERSION < SDL_VERSIONNUM(2,0,0)
#endif  // RAINBOW_SDL
