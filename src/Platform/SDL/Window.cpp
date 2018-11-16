// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/SDL/Window.h"

#include "Config.h"
#include "Graphics/Vulkan.h"

using rainbow::Config;
using rainbow::sdl::Window;

namespace
{
    static_assert(SDL_VERSION_ATLEAST(2, 0, 0),
                  "Rainbow requires SDL version 2.0.0 or higher");

    auto ensure_nonzero(int32_t value, int32_t fallback)
    {
        return value <= 0 ? fallback : value;
    }
}  // namespace

Window::Window(const Config& config)
{
    if (!is_initialized())
        return;

    setup(config);

    window_ = SDL_CreateWindow(
        RAINBOW_WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        ensure_nonzero(config.width(), 1280),
        ensure_nonzero(config.height(), 720),
        SDL_WINDOW_SHOWN | (config.high_dpi() ? SDL_WINDOW_ALLOW_HIGHDPI : 0));
    if (window_ == nullptr)
        R_ABORT("SDL: Failed to create window: %s", SDL_GetError());
}

Window::~Window()
{
    if (window_ == nullptr)
        return;

    SDL_DestroyWindow(window_);
}

void rainbow::sdl::open_window(const Config& config)
{
    Window window(config);
    if (!window)
        return;

    Chrono chrono;
    vk::Instance instance(vk::make_app_info(RAINBOW_WINDOW_TITLE, 1, 0, 0));
    if (!instance)
        return;

    vk::Surface surface(instance, window);
    if (!surface)
        return;

    vk::PhysicalDevice device(instance);
    if (!device)
        return;

    vk::Swapchain swapchain(device, surface);
    if (!swapchain)
        return;

    vk::Pipeline pipeline(swapchain);
    if (!pipeline)
        return;

    vk::CommandBuffer command_buffer(swapchain, pipeline);
    vk::ModelViewProjection mvp(swapchain);

    chrono.tick();
    LOGI("Vulkan: Initialization time: %" PRId64 " ms", chrono.delta());

    for (auto i = 0u; i < 300; ++i)
    {
        vk::update_descriptor(command_buffer, mvp);
        vk::VertexBuffer buf(swapchain.device(), 128);
        SDL_Event event;  // NOLINT(cppcoreguidelines-pro-type-member-init)
        while (SDL_PollEvent(&event) != 0)
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    return;

                case SDL_WINDOWEVENT:
                    switch (event.window.event)
                    {
                        case SDL_WINDOWEVENT_CLOSE:
                            return;
                        default:
                            break;
                    }
                    break;
            }
        }

        command_buffer.begin();
        command_buffer.end();

        chrono.tick();
    }
}
