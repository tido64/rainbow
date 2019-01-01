// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_DRIVER_H_
#define GRAPHICS_DRIVER_H_

#include <type_traits>

#include "Common/NonCopyable.h"
#include "Graphics/Vulkan.h"

namespace rainbow::graphics
{
    class Driver : private NonCopyable<Driver>
    {
    public:
        Driver(vk::PlatformWindow window,
               const VkApplicationInfo& app_info =
                   vk::make_app_info(RAINBOW_WINDOW_TITLE, 1, 0, 0))
            : instance_(app_info), surface_(instance_, window),
              swapchain_(vk::PhysicalDevice{instance_}, surface_),
              pipeline_(swapchain_)
        {
        }

        auto default_pipeline() const -> const vk::Pipeline&
        {
            return pipeline_;
        }

        auto device() const -> const vk::LogicalDevice&
        {
            return swapchain_.device();
        }

        auto swapchain() const -> const vk::Swapchain& { return swapchain_; }

        template <typename T>
        auto make_buffer() const
        {
            static_assert(std::is_base_of_v<vk::DynamicBuffer, T>);
            return T{device()};
        }

        template <typename T>
        auto make_buffer(size_t size) const
        {
            static_assert(std::is_base_of_v<vk::Buffer, T>);
            return T{device(), size};
        }

        auto make_projection_matrix() const
        {
            return vk::ProjectionMatrix{swapchain_};
        }

        explicit operator bool() const { return pipeline_; }

    private:
        vk::Instance instance_;
        vk::Surface surface_;
        vk::Swapchain swapchain_;
        vk::Pipeline pipeline_;
    };
}  // namespace rainbow::graphics

#endif
