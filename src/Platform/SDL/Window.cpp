// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/SDL/Window.h"

#include "Config.h"
#include "FileSystem/File.h"
#include "Graphics/Driver.h"
#include "Graphics/SpriteVertex.h"
#include "Graphics/TextureManager.h"
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
    graphics::Driver driver(window);
    if (!driver)
        return;

    auto mvp = driver.make_projection_matrix();
    graphics::TextureManager texture_manager(driver.device());
    auto index_buffer = vk::IndexBuffer{driver.device()};

    chrono.tick();
    LOGI("Vulkan: Initialization time: %" PRId64 " ms", chrono.delta());

    SpriteVertex vertices[]{
        {{}, {0.0f / 508, 97.0f / 288}, {604, 311}},
        {{}, {72.0f / 508, 97.0f / 288}, {676, 311}},
        {{}, {72.0f / 508, 0.0f / 288}, {676, 408}},
        {{}, {72.0f / 508, 0.0f / 288}, {676, 408}},
        {{}, {0.0f / 508, 0.0f / 288}, {604, 408}},
        {{}, {0.0f / 508, 97.0f / 288}, {604, 311}},
    };
    auto sprite_buffer = driver.make_buffer<vk::VertexBuffer>(sizeof(vertices));
    sprite_buffer.copy(ArraySpan<SpriteVertex>{vertices, array_size(vertices)});

    auto texture = texture_manager.get("p1_spritesheet.png");

    while (true)
    {
        SDL_Event event;  // NOLINT(cppcoreguidelines-pro-type-member-init)
        while (SDL_PollEvent(&event) != 0)
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    texture_manager.release("p1_spritesheet.png");
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

        auto& command_buffer = driver.default_command_buffer();
        command_buffer.begin();
        vk::update_descriptor(command_buffer, mvp);
        vk::update_descriptor(command_buffer, texture);
        vk::draw(command_buffer, sprite_buffer, array_size(vertices), index_buffer);
        command_buffer.end();

        chrono.tick();
    }
}
