// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/SDL/Window.h"

#include "Config.h"
#include "FileSystem/File.h"
#include "Graphics/SpriteVertex.h"
#include "Graphics/Texture.h"
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
        SDL_WINDOW_SHOWN | (config.hidpi() ? SDL_WINDOW_ALLOW_HIGHDPI : 0));
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
    vk::Context context(vk::make_app_info("Rainbow", 1, 0, 0), window);
    if (!context)
        return;

    auto pipeline = vk::Pipeline{context};
    auto command_buffer = vk::CommandBuffer{context, pipeline};
    auto mvp = vk::ProjectionMatrix{context};
    auto index_buffer = vk::IndexBuffer{context};
    // graphics::TextureManager texture_manager(context.device());

    chrono.tick();
    LOGI("Vulkan: Initialized in %" PRId64 " ms", chrono.delta());

    SpriteVertex vertices[]{
        {{}, {0.0F / 508, 97.0F / 288}, {604, 311}},
        {{}, {72.0F / 508, 97.0F / 288}, {676, 311}},
        {{}, {72.0F / 508, 0.0F / 288}, {676, 408}},
        {{}, {72.0F / 508, 0.0F / 288}, {676, 408}},
        {{}, {0.0F / 508, 0.0F / 288}, {604, 408}},
        {{}, {0.0F / 508, 97.0F / 288}, {604, 311}},
    };
    auto sprite_buffer = vk::VertexBuffer{context, sizeof(vertices)};
    sprite_buffer.copy(ArraySpan<SpriteVertex>{vertices, array_size(vertices)});

    /*
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

        command_buffer.begin();
        vk::update_descriptor(command_buffer, mvp);
        vk::update_descriptor(command_buffer, texture);
        vk::draw(
            command_buffer, sprite_buffer, array_size(vertices), index_buffer);
        command_buffer.end();

        chrono.tick();
    }
    */
}
