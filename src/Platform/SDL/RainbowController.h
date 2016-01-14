// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef PLATFORM_SDL_RAINBOWCONTROLLER_H_
#define PLATFORM_SDL_RAINBOWCONTROLLER_H_

#include <SDL_config.h>  // Ensure we include the correct SDL_config.h.
#include <SDL_gamecontroller.h>

#include "Director.h"

class SDLContext;

namespace rainbow { class Config; }

using GameController = std::tuple<int, SDL_GameController*>;

class RainbowController
{
public:
	RainbowController(SDLContext& context, const rainbow::Config& config);

	auto error() const -> const char* { return director_.error(); }

	bool run();

private:
	SDLContext& context_;
	Chrono chrono_;
	Director director_;
	const bool suspend_on_focus_lost_;
	std::vector<GameController> game_controllers_;

	void on_controller_connected(int device_index);
	void on_controller_disconnected(int instance_id);
	void on_mouse_down(uint32_t button,
	                   const Vec2i& point,
	                   unsigned long timestamp);
	void on_mouse_motion(uint32_t buttons,
	                     const Vec2i& point,
	                     unsigned long timestamp);
	void on_mouse_up(uint32_t button,
	                 const Vec2i& point,
	                 unsigned long timestamp);
	void on_window_resized();
};

#endif
