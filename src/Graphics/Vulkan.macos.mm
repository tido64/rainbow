// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Vulkan.h"

#include <AppKit/AppKit.h>
#include <QuartzCore/CAMetalLayer.h>
#include <SDL_syswm.h>
#include <vulkan/vulkan_macos.h>

using namespace rainbow::vk;

// TODO: Suppress "designated initializers are a C99 feature" until C++20.
#pragma clang diagnostic ignored "-Wc99-extensions"

auto Context::create_surface(PlatformWindow window) -> VkResult
{
    R_ASSERT(holds_alternative<SDL_Window*>(window), "This shouldn't happen.");

    SDL_SysWMinfo wm_info;
    SDL_VERSION(&wm_info.version);
    if (!SDL_GetWindowWMInfo(window.get_unchecked<SDL_Window*>(), &wm_info))
        R_ABORT("SDL: Failed to get window information: %s", SDL_GetError());

    NSView* content_view = wm_info.info.cocoa.window.contentView;
    content_view.wantsLayer = YES;
    content_view.layer = [CAMetalLayer layer];

    const VkMacOSSurfaceCreateInfoMVK create_info{
        .sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK,
        .pNext = nullptr,
        .flags = 0,
        .pView = content_view,
    };

    return vkCreateMacOSSurfaceMVK(instance_, &create_info, nullptr, &surface_);
}
