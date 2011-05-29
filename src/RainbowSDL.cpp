/// Copyright 2010-11 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

#include "Director.h"

bool active = true;  ///< Whether the window is in focus
bool done = false;   ///< Whether the user has requested to quit

unsigned short int screen_width = 640;   ///< Window width
unsigned short int screen_height = 960;  ///< Window height
const double fps = 1000.0 / 60.0;        ///< Preferred frames per second
const float milli = 1.0f / 1000.0f;      ///< 1 millisecond

Touch mouse_input;  ///< Mouse input
Director director;  ///< Game director handles everything

void init_GL();                                     ///< Initialise 2d viewport
void on_key_press(SDL_keysym &);                    ///< Handle key press event
void on_mouse_button_down(SDL_MouseButtonEvent &);  ///< Handle mouse button down event
void on_mouse_button_up(SDL_MouseButtonEvent &);    ///< Handle mouse button up event
void on_mouse_motion(SDL_MouseMotionEvent &);       ///< Handle mouse motion event
void resize(const int w, const int h);              ///< Handle window resize event


int main(int argc, char *argv[])
{
	//if (argc < 2)
	//{
	//	printf("Syntax: %s <main.lua>\n", argv[0]);
	//	return 0;
	//}

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
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	SDL_Surface *surface = SDL_SetVideoMode(screen_width, screen_height, 0, video_mode);
	if (!surface)
	{
		fprintf(stderr, "SDL unable to set video mode: %s\n", SDL_GetError());
		SDL_Quit();
		exit(1);
	}
	SDL_WM_SetCaption(RAINBOW_BUILD, "Rainbow");

	init_GL();
	resize(screen_width, screen_height);

	mouse_input.hash = 1;
	director.init("onwire.lua");

	unsigned int now = 0, time = SDL_GetTicks();
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
					on_key_press(event.key.keysym);
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
					resize(event.resize.w, event.resize.h);
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
			now = SDL_GetTicks();
			director.update((now - time) * milli);
			time = now;

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

void on_key_press(SDL_keysym &keysym)
{
	switch (keysym.sym)
	{
		case SDLK_q:
			if (keysym.mod == (KMOD_NUM | KMOD_LCTRL) || (keysym.mod == KMOD_LCTRL))
			{
				active = false;
				done = true;
				break;
			}
		default:
			director.key_press();
			break;
	}
}

inline void on_mouse_button_down(SDL_MouseButtonEvent &mouse)
{
	mouse_input.initial.x = mouse.x;
	mouse_input.initial.y = mouse.y;
	mouse_input.position = mouse_input.initial;
	Input::Instance().touch_began(&mouse_input, 1);
}

inline void on_mouse_button_up(SDL_MouseButtonEvent &mouse)
{
	mouse_input.position.x = mouse.x;
	mouse_input.position.y = mouse.y;
	Input::Instance().touch_ended(&mouse_input, 1);
}

inline void on_mouse_motion(SDL_MouseMotionEvent &mouse)
{
	mouse_input.position.x = mouse.x;
	mouse_input.position.y = mouse.y;
	Input::Instance().touch_moved(&mouse_input, 1);
}

void resize(const int w, const int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, w, h);

	director.update_video(w, h);
}
