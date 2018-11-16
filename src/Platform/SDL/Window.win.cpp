// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/SDL/Window.h"

#include <Windows.h>

#include "Config.h"

using rainbow::Config;
using rainbow::sdl::Window;

void Window::setup(const Config& config)
{
    if (config.high_dpi())
        SetProcessDPIAware();
}
