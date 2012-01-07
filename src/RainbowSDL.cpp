// Copyright 2010-12 Bifrost Entertainment. All rights reserved.

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

#ifdef RAINBOW_TEST
#	include "../tests/test.h"
#endif

#include "Director.h"
#include "Common/Chrono.h"
#include "Common/Data.h"
#include "Graphics/OpenGL.h"
#include "Input/Key.h"
#include "Input/Input.h"
#include "Input/Touch.h"

bool active = true;  ///< Whether the window is in focus
bool done = false;   ///< Whether the user has requested to quit

unsigned short int screen_width = 640;   ///< Window width
unsigned short int screen_height = 960;  ///< Window height
const double fps = 1000.0 / 60.0;        ///< Preferred frames per second
const float milli = 1.0f / 1000.0f;      ///< 1 millisecond
Touch mouse_input;                       ///< Mouse input

void init_GL();                                     ///< Initialise 2d viewport
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

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "Unable to initialise SDL: %s\n", SDL_GetError());
		exit(1);
	}

	const SDL_VideoInfo *video_info = SDL_GetVideoInfo();
	if (!video_info)
	{
		fprintf(stderr, "SDL video query failed: %s\n", SDL_GetError());
		SDL_Quit();
		exit(1);
	}

	int video_mode = SDL_HWPALETTE | SDL_OPENGL | ((!video_info->hw_available) ? SDL_SWSURFACE : SDL_HWSURFACE);
	if (video_info->blit_hw)
		video_mode |= SDL_HWACCEL;
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_Surface *surface = SDL_SetVideoMode(screen_width, screen_height, 0, video_mode);
	if (!surface)
	{
		fprintf(stderr, "SDL unable to set video mode: %s\n", SDL_GetError());
		SDL_Quit();
		exit(1);
	}
	SDL_WM_SetCaption(RAINBOW_BUILD, "Rainbow");

	init_GL();

	Data::set_datapath(argv[1]);  // Set data path
	Director director;            // Instantiate the director

	// Resize viewport
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
						done = true;
					}
					resize(director, event.resize.w, event.resize.h);
					break;
				default:
					break;
			}
		}
		if (!active)
			SDL_Delay(static_cast<Uint32>(fps));
		else
		{
			// Update game logic
			director.update();

			// Draw
			glClear(GL_COLOR_BUFFER_BIT);
			director.draw();
			SDL_GL_SwapBuffers();
		}
	}
	SDL_Quit();
	return 0;
}

void init_GL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
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
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, w, h);

	director.set_video(w, h);
}
