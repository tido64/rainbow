// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Vulkan.h"

#include <QuartzCore/CAMetalLayer.h>
#include <UIKit/UIView.h>
#include <vulkan/vulkan_ios.h>

using namespace rainbow::vk;

// TODO: Suppress "designated initializers are a C99 feature" until C++20.
#pragma clang diagnostic ignored "-Wc99-extensions"

auto Context::create_surface(PlatformWindow window) -> VkResult
{
    R_ASSERT(holds_alternative<SDL_Window*>(window), "This shouldn't happen.");

    // TODO

    UIView* content_view = nil;
    R_ASSERT([content_view.layer isKindOfClass:[CAMetalLayer class]], "");
    //content_view.layer = [CAMetalLayer layer];

    const VkIOSSurfaceCreateInfoMVK create_info{
        .sType = VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK,
        .pNext = nullptr,
        .flags = 0,
        .pView = (__bridge void*)content_view,
    };

    return vkCreateIOSSurfaceMVK(instance_, &create_info, nullptr, &surface_);
}
