#include <cstdlib>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

#define DEBUG 1
#include "OnWireGame.h"

bool active = true;
bool done = false;
const unsigned short int screen_width = 320;
const unsigned short int screen_height = 480;
int video_mode = 0;
const double fps = 1000.0 / 60.0;

void draw();
bool init_GL();
void on_key_press(SDL_keysym &keysym);
void quit(int code);
void resize(const int w, const int h);
int tick(void *);

int main()
{
	if (SDL_Init(/*SDL_INIT_AUDIO |*/ SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "Unable to initialise SDL: %s\n", SDL_GetError());
		exit(1);
	}

	const SDL_VideoInfo *video_info = SDL_GetVideoInfo();
	if (!video_info)
	{
		fprintf(stderr, "Video query failed: %s\n", SDL_GetError());
		SDL_Quit();
		exit(1);
	}

	video_mode = SDL_HWPALETTE | SDL_OPENGL;
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
		fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
		SDL_Quit();
		exit(1);
	}
	SDL_WM_SetCaption("OnWire", "OnWire");

	init_GL();
	resize(screen_width, screen_height);

	OnWireGame the_game;
	the_game.reset(1337);
	SDL_Thread *clock_thread = SDL_CreateThread(tick, &the_game);

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
						fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
						done = true;
					}
					resize(event.resize.w, event.resize.h);
					break;
				case SDL_KEYDOWN:
					on_key_press(event.key.keysym);
					break;
				case SDL_QUIT:
					puts("Quitting...");
					active = false;
					done = true;
					break;
				default:
					break;
			}
		}
		if (!active)
			SDL_Delay(fps);
		else
		{
			// Update physics
			now = SDL_GetTicks();
			Physics::Instance()->step((now - time) / 1000.0f);
			time = now;

			// Update game logic
			the_game.update();

			// Draw
			glClear(GL_COLOR_BUFFER_BIT);
			the_game.draw();
			SDL_GL_SwapBuffers();
		}
	}

	puts("Quitting...");
	SDL_WaitThread(clock_thread, 0);
	SDL_Quit();
	return 0;
}

bool init_GL()
{
	// Initialize 2d viewport
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

	Screen::Instance()->init(w, h);
}

int tick(void *g)
{
	const unsigned int delay = 1000;
	OnWireGame *game = static_cast<OnWireGame *>(g);
	SDL_Delay(delay);

	unsigned int start = SDL_GetTicks();
	unsigned int wait = 0;
	while (!done)
	{
		if (!active)
		{
			SDL_Delay(delay);
			start = SDL_GetTicks();
			continue;
		}

		game->tick();
		wait = delay - (SDL_GetTicks() - start);
		if (wait > 0)
			SDL_Delay(wait);
		start = SDL_GetTicks();
	}
	return 0;
}
