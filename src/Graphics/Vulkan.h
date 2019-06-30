// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_VULKAN_H_
#define GRAPHICS_VULKAN_H_

#include <utility>
#include <vector>

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include "Common/Callback.h"
#include "Common/NonCopyable.h"
#include "Common/String.h"
#include "Common/TypeCast.h"
#include "Common/Variant.h"
#include "Memory/Array.h"
#include "ThirdParty/VulkanMemoryAllocator.fwd.h"

using SDL_Window = struct SDL_Window;

namespace rainbow
{
    struct Image;
}  // namespace rainbow

// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/vkspec.html
// https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain
// VK_LAYER_PATH=/~/_deps/src/vulkansdk/macOS/etc/vulkan/explicit_layer.d
// VK_ICD_FILENAMES=/~/_deps/src/vulkansdk/macOS/etc/vulkan/icd.d/MoltenVK_icd.json
namespace rainbow::vk
{
    using PlatformWindow = variant<SDL_Window*>;

    enum class BufferUsageFlags : uint32_t
    {
        TransferSrc = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        TransferDst = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        UniformTexelBuffer = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT,
        StorageTexelBuffer = VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT,
        UniformBuffer = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        StorageBuffer = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        IndexBuffer = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VertexBuffer = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        IndirectBuffer = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
        TransformFeedbackBufferEXT =
            VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER_BIT_EXT,
        TransformFeedbackCounterBufferEXT =
            VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER_BIT_EXT,
        ConditionalRenderingEXT = VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT,
        RayTracingNV = VK_BUFFER_USAGE_RAY_TRACING_BIT_NV,
    };

    constexpr auto operator|(BufferUsageFlags lhs, BufferUsageFlags rhs)
    {
        return static_cast<BufferUsageFlags>(to_underlying_type(lhs) |
                                             to_underlying_type(rhs));
    }

    enum class Filter
    {
        Nearest = VK_FILTER_NEAREST,
        Linear = VK_FILTER_LINEAR,
        CubicIMG = VK_FILTER_CUBIC_IMG,
    };

    enum class ShaderStageFlags : uint32_t
    {
        Vertex = VK_SHADER_STAGE_VERTEX_BIT,
        TessellationControl = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
        TessellationEvaluation = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
        Geometry = VK_SHADER_STAGE_GEOMETRY_BIT,
        Fragment = VK_SHADER_STAGE_FRAGMENT_BIT,
        Compute = VK_SHADER_STAGE_COMPUTE_BIT,
        AllGraphics = VK_SHADER_STAGE_ALL_GRAPHICS,
        All = VK_SHADER_STAGE_ALL,
        RaygenNV = VK_SHADER_STAGE_RAYGEN_BIT_NV,
        AnyHitNV = VK_SHADER_STAGE_ANY_HIT_BIT_NV,
        ClosestHitNV = VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV,
        MissNV = VK_SHADER_STAGE_MISS_BIT_NV,
        IntersectionNV = VK_SHADER_STAGE_INTERSECTION_BIT_NV,
        CallableNV = VK_SHADER_STAGE_CALLABLE_BIT_NV,
        TaskNV = VK_SHADER_STAGE_TASK_BIT_NV,
        MeshNV = VK_SHADER_STAGE_MESH_BIT_NV,
    };

    constexpr auto operator|(ShaderStageFlags lhs, ShaderStageFlags rhs)
    {
        return static_cast<ShaderStageFlags>(to_underlying_type(lhs) |
                                             to_underlying_type(rhs));
    }

    class Context : private NonCopyable<Context>
    {
    public:
        Context(const VkApplicationInfo&, PlatformWindow);
        ~Context();

        [[nodiscard]]
        auto allocator() const { return allocator_; }

        [[nodiscard]]
        auto command_pool() const { return command_pool_; }

        [[nodiscard]]
        auto device() const { return device_; }

        [[nodiscard]]
        auto image_extent() const { return image_extent_; }

        [[nodiscard]]
        auto image_views() const -> const std::vector<VkImageView>&
        {
            return views_;
        }

        [[nodiscard]]
        auto memory_type_index() const { return memory_type_index_; }

        [[nodiscard]]
        auto min_uniform_buffer_offset_alignment() const
        {
            return min_uniform_buffer_offset_alignment_;
        }

        [[nodiscard]]
        auto physical_device() const { return physical_device_; }

        [[nodiscard]]
        auto queue() const { return queue_; }

        [[nodiscard]]
        auto surface_format() const { return surface_format_.format; }

        [[nodiscard]]
        auto swapchain() const { return swapchain_; }

        void clear_swapchain();
        void reset_swapchain();

        explicit operator bool() const { return swapchain_ != VK_NULL_HANDLE; }

    private:
        VkDevice device_ = VK_NULL_HANDLE;
        VkQueue queue_ = VK_NULL_HANDLE;
        VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
        VkExtent2D image_extent_ = {0, 0};

        VmaAllocator allocator_ = VK_NULL_HANDLE;      // Buffers, textures
        VkCommandPool command_pool_ = VK_NULL_HANDLE;  // Command buffers

        VkDeviceSize min_uniform_buffer_offset_alignment_ = 0;  // Uniforms
        uint32_t memory_type_index_ = -1;                       // Buffers

        std::vector<VkImageView> views_;  // Pipeline, command buffer, mvp
        VkSurfaceFormatKHR surface_format_ = {
            VK_FORMAT_UNDEFINED,
            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};  // Swap chain, pipeline

        VkSurfaceKHR surface_ = VK_NULL_HANDLE;
        VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
        VkInstance instance_ = VK_NULL_HANDLE;

        auto create_surface(PlatformWindow) -> VkResult;
    };

    struct DescriptorBindings
    {
        VkBuffer index_buffer = VK_NULL_HANDLE;
        VkImageView texture = VK_NULL_HANDLE;
    };

    struct FrameInfo
    {
        DescriptorBindings bindings;
        VkDescriptorSet descriptor_set = VK_NULL_HANDLE;
        VkCommandBuffer command_buffer = VK_NULL_HANDLE;
        VkFramebuffer framebuffer = VK_NULL_HANDLE;
        VkFence fence = VK_NULL_HANDLE;
    };

    class Pipeline : private NonCopyable<Pipeline>
    {
    public:
        Pipeline(const Context&, VkPipeline base_pipeline = VK_NULL_HANDLE);
        ~Pipeline();

        [[nodiscard]]
        auto descriptor_pool() const { return descriptor_pool_; }

        [[nodiscard]]
        auto descriptor_set_layout() const { return descriptor_set_layout_; }

        [[nodiscard]]
        auto device() const { return device_; }

        [[nodiscard]]
        auto layout() const { return layout_; }

        [[nodiscard]]
        auto render_pass() const { return render_pass_; }

        operator VkPipeline() const { return pipeline_; }

        explicit operator bool() const { return pipeline_ != VK_NULL_HANDLE; }

    private:
        VkDevice device_ = VK_NULL_HANDLE;
        VkDescriptorSetLayout descriptor_set_layout_ = VK_NULL_HANDLE;
        VkDescriptorPool descriptor_pool_ = VK_NULL_HANDLE;
        VkShaderModule vertex_shader_ = VK_NULL_HANDLE;
        VkShaderModule fragment_shader_ = VK_NULL_HANDLE;
        VkPipelineLayout layout_ = VK_NULL_HANDLE;
        VkRenderPass render_pass_ = VK_NULL_HANDLE;
        VkPipeline pipeline_ = VK_NULL_HANDLE;
        VkPipelineCache pipeline_cache_ = VK_NULL_HANDLE;
    };

    class Buffer : private NonCopyable<Buffer>
    {
    public:
        Buffer(Buffer&&) noexcept;

        template <typename T>
        void copy(T data)
        {
            copy(data.data(), data.size() * sizeof(data[0]));
        }

        operator VkBuffer() const { return buffer_; }

    protected:
        Buffer(const Context&, size_t, BufferUsageFlags);
        ~Buffer();

        void copy(const void* src, size_t size, size_t offset = 0);

    private:
        VkBuffer buffer_ = VK_NULL_HANDLE;
        VmaAllocation allocation_ = VK_NULL_HANDLE;
        VmaAllocator allocator_ = VK_NULL_HANDLE;
    };

    class CommandBuffer : private NonCopyable<CommandBuffer>
    {
    public:
        static void run_once(const Context&, Callback<void(VkCommandBuffer)>);

        CommandBuffer(const Context&, const Pipeline&);
        ~CommandBuffer();

        [[nodiscard]]
        auto bindings() const -> DescriptorBindings&
        {
            return current().bindings;
        }

        [[nodiscard]]
        auto current_index() const { return current_; }

        [[nodiscard]]
        auto descriptor_set() const { return current().descriptor_set; }

        [[nodiscard]]
        auto pipeline() const -> const Pipeline& { return pipeline_; }

        void begin();
        void end();

        operator VkCommandBuffer() const { return current().command_buffer; }

    private:
        const Context& context_;
        const Pipeline& pipeline_;
        // TODO: Keep a local copy of current FrameInfo?
        uint32_t current_ = 0;
        std::unique_ptr<FrameInfo[]> frame_info_;
        VkSemaphore image_available_ = VK_NULL_HANDLE;
        VkSemaphore render_finished_ = VK_NULL_HANDLE;

        [[nodiscard]]
        auto current() const -> FrameInfo& { return frame_info_[current_]; }
    };

    class DynamicBuffer : private NonCopyable<DynamicBuffer>
    {
    public:
        template <typename T>
        void copy(T data)
        {
            copy(data.data(), data.size() * sizeof(data[0]));
        }

        operator VkBuffer() const { return buffer_; }

    protected:
        DynamicBuffer(const Context& context, BufferUsageFlags usage)
            : allocator_(context.allocator()),
              memory_type_index_(context.memory_type_index()), usage_(usage)
        {
        }

        ~DynamicBuffer();

        void copy(const void* src, size_t size);

    private:
        VkBuffer buffer_ = VK_NULL_HANDLE;
        VmaAllocation allocation_ = VK_NULL_HANDLE;
        VmaAllocator allocator_ = VK_NULL_HANDLE;
        uint32_t capacity_ = 0;
        uint32_t memory_type_index_;
        BufferUsageFlags usage_;
    };

    class DynamicVertexBuffer final : public DynamicBuffer
    {
    public:
        DynamicVertexBuffer(const Context& context)
            : DynamicBuffer(context,
                            BufferUsageFlags::TransferDst |
                                BufferUsageFlags::VertexBuffer)
        {
        }
    };

    class IndexBuffer final : private Buffer
    {
    public:
        static constexpr uint32_t kMaxSprites = 0xffffU / 4;
        static constexpr uint32_t kIndexBufferSize = kMaxSprites * 6;

        IndexBuffer(const Context&);

        using Buffer::operator VkBuffer;
    };

    class ProjectionMatrix final : private Buffer
    {
    public:
        ProjectionMatrix(const Context&);

        [[nodiscard]]
        auto aligned_size() const { return aligned_size_; }

        [[nodiscard]]
        constexpr auto count() const -> uint32_t { return 16; }

        [[nodiscard]]
        constexpr auto size() const -> uint32_t { return 16 * sizeof(float); }

        void set(const VkViewport&, uint32_t index);

        using Buffer::operator VkBuffer;

    private:
        size_t aligned_size_ = 0;
    };

    class StagingBuffer final : public Buffer
    {
    public:
        StagingBuffer(const Context& context, size_t size)
            : Buffer(context, size, BufferUsageFlags::TransferSrc)
        {
        }
    };

    class UniformBuffer final : public Buffer
    {
    public:
        UniformBuffer(const Context& context, size_t size)
            : Buffer(context,
                     size,
                     BufferUsageFlags::TransferDst |
                         BufferUsageFlags::UniformBuffer)
        {
        }
    };

    class VertexBuffer final : public Buffer
    {
    public:
        VertexBuffer(const Context& context, size_t size)
            : Buffer(context,
                     size,
                     BufferUsageFlags::TransferDst |
                         BufferUsageFlags::VertexBuffer)
        {
        }
    };

    void bind(const CommandBuffer&, const DynamicVertexBuffer&);
    void bind(const CommandBuffer&, const IndexBuffer&);
    void bind(const CommandBuffer&, const VertexBuffer&);

    void draw(const CommandBuffer&, const VertexBuffer&, uint32_t vertex_count);
    void draw(const CommandBuffer&,
              const DynamicVertexBuffer&,
              uint32_t vertex_count,
              const IndexBuffer&);
    void draw(const CommandBuffer&,
              const VertexBuffer&,
              uint32_t vertex_count,
              const IndexBuffer&);

    template <size_t N>
    auto make_app_info(const char (&app_name)[N],
                       uint32_t major,
                       uint32_t minor,
                       uint32_t patch)
    {
        return VkApplicationInfo{
            /* sType */ VK_STRUCTURE_TYPE_APPLICATION_INFO,
            /* pNext */ nullptr,
            /* pApplicationName */ app_name,
            /* applicationVersion */ VK_MAKE_VERSION(major, minor, patch),
            /* pEngineName */ "Rainbow",
            /* engineVersion */ VK_MAKE_VERSION(1, 0, 0),
            /* apiVersion */ VK_API_VERSION_1_0,
        };
    }

    auto make_image_view(VkDevice, VkImage, VkFormat) -> VkImageView;

    auto make_shader_module(VkDevice, ArraySpan<uint8_t> code)
        -> VkShaderModule;

    auto make_shader_stage_info(ShaderStageFlags,
                                VkShaderModule,
                                const char* name = "main")
        -> VkPipelineShaderStageCreateInfo;

    auto to_string(VkResult result) -> czstring;

    void update_descriptor(const CommandBuffer&, const ProjectionMatrix&);

    void update_descriptor(const CommandBuffer&,
                           const VkDescriptorImageInfo&,
                           uint32_t binding = 1);
}  // namespace rainbow::vk

#endif
