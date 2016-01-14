// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef PLATFORM_SDL_CONTEXT_H_
#define PLATFORM_SDL_CONTEXT_H_

#include <SDL_config.h>  // Ensure we include the correct SDL_config.h.
#include <SDL_video.h>

#include "Common/Vec2.h"

namespace rainbow { class Config; }

class SDLContext
{
public:
	SDLContext(const rainbow::Config& config);
	~SDLContext();

	Vec2i drawable_size() const;
	Vec2i window_size() const;

	void swap() const;
	void toggle_fullscreen();

	explicit operator bool() const { return context_; }

private:
	SDL_Window* window_;     ///< Window handle.
	bool vsync_;             ///< Whether vertical sync is enabled.
	uint32_t fullscreen_;    ///< Whether the window is in full screen mode.
	SDL_GLContext context_;  ///< OpenGL context handle.
};

#endif
