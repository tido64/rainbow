// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Vulkan.h"

namespace rainbow::graphics
{
    class Driver
    {
    public:
        Driver(vk::PlatformWindow window,
               const VkApplicationInfo& app_info =
                   vk::make_app_info(RAINBOW_WINDOW_TITLE, 1, 0, 0))
            : instance_(app_info), surface_(instance_, window),
              swapchain_(vk::PhysicalDevice{instance_}, surface_),
              pipeline_(swapchain_), command_buffer_(swapchain_, pipeline_)
        {
        }

        auto default_command_buffer() -> vk::CommandBuffer&
        {
            return command_buffer_;
        }

        auto default_pipeline() const -> const vk::Pipeline&
        {
            return pipeline_;
        }

        auto device() const -> const vk::LogicalDevice&
        {
            return swapchain_.device();
        }

        template <typename T>
        auto make_buffer(size_t size)
        {
            static_assert(std::is_base_of_v<vk::Buffer, T>);
            return T{device(), size};
        }

        auto make_projection_matrix()
        {
            return vk::ProjectionMatrix{swapchain_};
        }

        explicit operator bool() const { return pipeline_; }

    private:
        vk::Instance instance_;
        vk::Surface surface_;
        vk::Swapchain swapchain_;
        vk::Pipeline pipeline_;
        vk::CommandBuffer command_buffer_;
    };
}  // namespace rainbow::graphics
