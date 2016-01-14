// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/SDL/Context.h"

#include <SDL.h>

#include "Common/Logging.h"
#include "Config.h"

namespace
{
	static_assert(SDL_VERSION_ATLEAST(2, 0, 4),
	              "Rainbow requires SDL version 2.0.4 or higher");

	Vec2i window_size(const rainbow::Config& config)
	{
		return (!config.width() || !config.height()
		            ? Vec2i(1280, 720)
		            : Vec2i(config.width(), config.height()));
	}
}

SDLContext::SDLContext(const rainbow::Config& config)
    : window_(nullptr), vsync_(false), fullscreen_(0), context_(nullptr)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
	{
		LOGF("SDL: Unable to initialise video: %s", SDL_GetError());
		return;
	}

#ifdef RAINBOW_OS_MACOS
	// Prevent the full screen window from being minimized when losing focus.
	SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
#endif
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
	if (config.msaa() > 0)
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, config.msaa());
	}

	const uint32_t flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN |
	                       (config.high_dpi() ? SDL_WINDOW_ALLOW_HIGHDPI : 0);
	const Vec2i& size = ::window_size(config);
	window_ = SDL_CreateWindow(RAINBOW_BUILD,
	                           SDL_WINDOWPOS_CENTERED,
	                           SDL_WINDOWPOS_CENTERED,
	                           size.x,
	                           size.y,
	                           flags);
	if (!window_)
	{
		R_ABORT("SDL: Failed to create window: %s", SDL_GetError());
		return;
	}

	context_ = SDL_GL_CreateContext(window_);
	if (!context_)
	{
		R_ABORT("SDL: Failed to create GL context: %s", SDL_GetError());
		return;
	}

#ifdef RAINBOW_JS
	vsync_ = true;
#else
	SDL_GL_SetSwapInterval(1);
	vsync_ = SDL_GL_GetSwapInterval() == 1;
#endif

#ifndef NDEBUG
	const Vec2i& resolution = drawable_size();
	LOGI("SDL: Resolution: %ix%i", resolution.x, resolution.y);
	int msaa = 0;
	SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &msaa);
	if (msaa > 0)
		LOGI("SDL: Anti-aliasing: %ix MSAA", msaa);
	else
		LOGI("SDL: Anti-aliasing: Disabled");
	LOGI("SDL: Vertical sync: %s", (vsync_ ? "Enabled" : "Disabled"));
#endif
}

SDLContext::~SDLContext()
{
	if (!SDL_WasInit(SDL_INIT_VIDEO))
		return;

	if (window_)
	{
		if (context_)
			SDL_GL_DeleteContext(context_);
		SDL_DestroyWindow(window_);
	}
	SDL_Quit();
}

Vec2i SDLContext::drawable_size() const
{
	Vec2i size;
	SDL_GL_GetDrawableSize(window_, &size.x, &size.y);
	return size;
}

Vec2i SDLContext::window_size() const
{
	Vec2i size;
	SDL_GetWindowSize(window_, &size.x, &size.y);
	return size;
}

void SDLContext::swap() const
{
	if (!vsync_)
		Chrono::sleep(0);
	SDL_GL_SwapWindow(window_);
}

void SDLContext::toggle_fullscreen()
{
	fullscreen_ ^= SDL_WINDOW_FULLSCREEN_DESKTOP;
	SDL_SetWindowFullscreen(window_, fullscreen_);
}
