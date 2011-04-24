#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

#define DEBUG 1
#include "Director.h"

bool active = true;  ///< Whether the window is in focus
bool done = false;   ///< Whether the user has requested to quit

unsigned short int screen_width = 640;        ///< Window width
unsigned short int screen_height = 960;       ///< Window height
int video_mode = SDL_HWPALETTE | SDL_OPENGL;  ///< Window video mode
const double fps = 1000.0 / 60.0;             ///< Preferred frames per second

Director director;  ///< Game director handles everything

bool init_GL();                         ///< Initialize 2d viewport
void on_key_press(SDL_keysym &keysym);  ///< Handle key press event
void resize(const int w, const int h);  ///< Handle window resize event


/// Rainbow rendered with the help of SDL library.

/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
int main(int argc, char *argv[])
{
	if (SDL_Init(/*SDL_INIT_AUDIO |*/ SDL_INIT_VIDEO) < 0)
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

	if (!video_info->hw_available)
		video_mode |= SDL_SWSURFACE;
	else
		video_mode |= SDL_HWSURFACE;
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
	SDL_WM_SetCaption("OnWire", "OnWire");

	init_GL();
	resize(screen_width, screen_height);

	director.init("onwire.lua");

	unsigned int now, time = SDL_GetTicks();
	while (!done)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_ACTIVEEVENT:
					active = (event.active.gain == 0) ? false : true;
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
				case SDL_KEYDOWN:
					on_key_press(event.key.keysym);
					break;
				case SDL_QUIT:
					active = false;
					done = true;
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
			director.update((now - time) / 1000.0f);
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

bool init_GL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	return true;
}

void on_key_press(SDL_keysym &keysym)
{
	switch (keysym.sym)
	{
		case SDLK_q:
			if (keysym.mod != KMOD_LCTRL) break;
		case SDLK_ESCAPE:
			active = false;
			done = true;
			break;
		default:
			director.key_press();
			break;
	}
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
