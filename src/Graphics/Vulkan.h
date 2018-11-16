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
#include "Common/TypeCast.h"
#include "Common/Variant.h"
#include "Memory/Array.h"
#include "ThirdParty/VulkanMemoryAllocator.fwd.h"

typedef struct SDL_Window SDL_Window;

namespace rainbow
{
    struct Image;
}

// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/vkspec.html
// https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain
// VK_LAYER_PATH=~/Downloads/vulkansdk-macos-1.1.92.1/macOS/etc/vulkan/explicit_layer.d
// VK_ICD_FILENAMES=~/Downloads/vulkansdk-macos-1.1.92.1/macOS/etc/vulkan/icd.d/MoltenVK_icd.json
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

    struct DescriptorBindings
    {
        VkBuffer index_buffer = VK_NULL_HANDLE;
        VkImageView texture = VK_NULL_HANDLE;
    };

    struct DeviceProperties
    {
        char device_name[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE];
        size_t memory_heap_size;
        uint32_t api_version;
    };

    struct FrameInfo
    {
        DescriptorBindings bindings;
        VkDescriptorSet descriptor_set = VK_NULL_HANDLE;
        VkCommandBuffer command_buffer = VK_NULL_HANDLE;
        VkFramebuffer framebuffer = VK_NULL_HANDLE;
        VkFence fence = VK_NULL_HANDLE;
    };

    class Instance : private NonCopyable<Instance>
    {
    public:
        explicit Instance(const VkApplicationInfo&);
        ~Instance();

        operator VkInstance() const { return instance_; }

        explicit operator bool() const { return instance_ != VK_NULL_HANDLE; }

    private:
        VkInstance instance_ = VK_NULL_HANDLE;
    };

    class Surface : private NonCopyable<Surface>
    {
    public:
        Surface(Instance&, PlatformWindow);
        ~Surface();

        operator VkSurfaceKHR() const { return surface_; }

        explicit operator bool() const { return surface_ != VK_NULL_HANDLE; }

    private:
        const Instance& instance_;
        VkSurfaceKHR surface_ = VK_NULL_HANDLE;

        auto create(PlatformWindow) -> VkResult;
    };

    class PhysicalDevice
    {
    public:
        explicit PhysicalDevice(const Instance&);

        auto memory_type_index() const { return memory_type_index_; }

        auto min_uniform_buffer_offset_alignment() const
        {
            return min_uniform_buffer_offset_alignment_;
        }

        operator VkPhysicalDevice() const { return device_; }

        explicit operator bool() const { return device_ != VK_NULL_HANDLE; }

    private:
        VkPhysicalDevice device_ = VK_NULL_HANDLE;
        VkDeviceSize min_uniform_buffer_offset_alignment_ = 0;
        uint32_t memory_type_index_ = -1;
    };

    class LogicalDevice : private NonCopyable<LogicalDevice>
    {
    public:
        LogicalDevice(PhysicalDevice, const Surface&);
        ~LogicalDevice();

        auto allocator() const { return allocator_; }
        auto command_pool() const { return command_pool_; }
        auto memory_type_index() const { return memory_type_index_; }

        auto min_uniform_buffer_offset_alignment() const
        {
            return min_uniform_buffer_offset_alignment_;
        }

        auto queue() const { return queue_; }

        operator VkDevice() const { return device_; }

        explicit operator bool() const { return device_ != VK_NULL_HANDLE; }

    private:
        VkDevice device_ = VK_NULL_HANDLE;
        VmaAllocator allocator_ = VK_NULL_HANDLE;
        VkCommandPool command_pool_ = VK_NULL_HANDLE;
        VkQueue queue_ = VK_NULL_HANDLE;
        VkDeviceSize min_uniform_buffer_offset_alignment_ = 0;
        uint32_t memory_type_index_ = -1;

#ifndef NDEBUG
    public:
        auto properties() const -> const DeviceProperties&
        {
            return properties_;
        }

    private:
        DeviceProperties properties_;
#endif
    };

    class Swapchain : private NonCopyable<Swapchain>
    {
    public:
        Swapchain(PhysicalDevice, const Surface&);
        ~Swapchain();

        auto device() const -> const LogicalDevice& { return device_; }
        auto image_extent() const { return image_extent_; }
        auto image_format() const { return surface_format_.format; }

        auto image_views() const -> const std::vector<VkImageView>&
        {
            return views_;
        }

        operator VkSwapchainKHR() const { return swapchain_; }

        explicit operator bool() const { return swapchain_ != VK_NULL_HANDLE; }

    private:
        LogicalDevice device_;
        VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
        std::vector<VkImageView> views_;
        VkExtent2D image_extent_;
        VkSurfaceFormatKHR surface_format_;
    };

    class Pipeline : private NonCopyable<Pipeline>
    {
    public:
        Pipeline(const Swapchain&, VkPipeline base_pipeline = VK_NULL_HANDLE);
        ~Pipeline();

        auto descriptor_pool() const { return descriptor_pool_; }
        auto descriptor_set_layout() const { return descriptor_set_layout_; }
        auto device() const -> const LogicalDevice& { return device_; }
        auto layout() const { return layout_; }
        auto render_pass() const { return render_pass_; }

        operator VkPipeline() const { return pipeline_; }

        explicit operator bool() const { return pipeline_ != VK_NULL_HANDLE; }

    private:
        const LogicalDevice& device_;
        VkDescriptorSetLayout descriptor_set_layout_ = VK_NULL_HANDLE;
        VkDescriptorPool descriptor_pool_ = VK_NULL_HANDLE;
        VkShaderModule vertex_shader_ = VK_NULL_HANDLE;
        VkShaderModule fragment_shader_ = VK_NULL_HANDLE;
        VkPipelineLayout layout_ = VK_NULL_HANDLE;
        VkRenderPass render_pass_ = VK_NULL_HANDLE;
        VkPipeline pipeline_ = VK_NULL_HANDLE;
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
        Buffer(const LogicalDevice&, size_t, BufferUsageFlags);
        ~Buffer();

        void copy(void* src, size_t size, size_t offset = 0);

    private:
        VkBuffer buffer_ = VK_NULL_HANDLE;
        VmaAllocation allocation_ = VK_NULL_HANDLE;
        VmaAllocator allocator_ = VK_NULL_HANDLE;
    };

    class CommandBuffer : private NonCopyable<CommandBuffer>
    {
    public:
        static void run_once(const LogicalDevice&,
                             Callback<void(VkCommandBuffer)>);

        CommandBuffer(const Swapchain&, const Pipeline&);
        ~CommandBuffer();

        auto bindings() const -> DescriptorBindings&
        {
            return frame_info_[current_].bindings;
        }

        auto current_index() const { return current_; }

        auto descriptor_set() const
        {
            return frame_info_[current_].descriptor_set;
        }

        auto pipeline() const -> const Pipeline& { return pipeline_; }

        void begin();
        void end();

        operator VkCommandBuffer() const
        {
            return frame_info_[current_].command_buffer;
        }

    private:
        const Swapchain& swapchain_;
        const Pipeline& pipeline_;
        // TODO: Keep a local copy of current FrameInfo?
        uint32_t current_ = 0;
        std::unique_ptr<FrameInfo[]> frame_info_;
        VkSemaphore image_available_ = VK_NULL_HANDLE;
        VkSemaphore render_finished_ = VK_NULL_HANDLE;
    };

    class DynamicBuffer
    {
    public:
        template <typename T>
        void copy(T data)
        {
            copy(data.data(), data.size() * sizeof(data[0]));
        }

        operator VkBuffer() const { return buffer_; }

    protected:
        DynamicBuffer(const LogicalDevice& device, BufferUsageFlags usage)
            : usage_(usage), device_(device)
        {
        }

        ~DynamicBuffer();

        void copy(void* src, size_t size);

    private:
        uint32_t capacity_ = 0;
        BufferUsageFlags usage_;
        VkBuffer buffer_ = VK_NULL_HANDLE;
        VmaAllocation allocation_ = VK_NULL_HANDLE;
        const LogicalDevice& device_;
    };

    class DynamicVertexBuffer final : public DynamicBuffer
    {
    public:
        DynamicVertexBuffer(const LogicalDevice& device)
            : DynamicBuffer(device,
                            BufferUsageFlags::TransferDst |
                                BufferUsageFlags::VertexBuffer)
        {
        }
    };

    class IndexBuffer final : private Buffer
    {
    public:
        static constexpr uint32_t kMaxSprites = 0xffffu / 4;

        IndexBuffer(const LogicalDevice&);

        using Buffer::operator VkBuffer;
    };

    class ProjectionMatrix final : private Buffer
    {
    public:
        ProjectionMatrix(const Swapchain& swapchain);

        auto aligned_size() const { return aligned_size_; }
        constexpr auto count() const -> uint32_t { return 16; }
        constexpr auto size() const -> uint32_t { return 16 * sizeof(float); }

        void set(const VkViewport&, uint32_t index);

        using Buffer::operator VkBuffer;

    private:
        size_t aligned_size_ = 0;
    };

    class StagingBuffer final : public Buffer
    {
    public:
        StagingBuffer(const LogicalDevice& device, size_t size)
            : Buffer(device, size, BufferUsageFlags::TransferSrc)
        {
        }
    };

    class Texture
    {
    public:
        Texture(const LogicalDevice&,
                const Image&,
                Filter mag_filter = Filter::CubicIMG,
                Filter min_filter = Filter::Linear);

        auto descriptor() const
        {
            return VkDescriptorImageInfo{
                /* sampler */ sampler_,
                /* imageView */ view_,
                /* imageLayout */ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            };
        };

        void destroy(const LogicalDevice&);

        void update(const LogicalDevice&, const Image&) const;

    private:
        VkSampler sampler_ = VK_NULL_HANDLE;
        VkImageView view_ = VK_NULL_HANDLE;
        VkImage image_ = VK_NULL_HANDLE;
        VmaAllocation allocation_ = VK_NULL_HANDLE;
    };

    class UniformBuffer final : public Buffer
    {
    public:
        UniformBuffer(const LogicalDevice& device, size_t size)
            : Buffer(device,
                     size,
                     BufferUsageFlags::TransferDst |
                         BufferUsageFlags::UniformBuffer)
        {
        }
    };

    class VertexBuffer final : public Buffer
    {
    public:
        VertexBuffer(const LogicalDevice& device, size_t size)
            : Buffer(device,
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

    auto make_shader_module(const LogicalDevice&, ArraySpan<uint8_t> code)
        -> VkShaderModule;

    auto make_shader_stage_info(ShaderStageFlags,
                                VkShaderModule,
                                const char* name = "main")
        -> VkPipelineShaderStageCreateInfo;

    void update_descriptor(const CommandBuffer&, const ProjectionMatrix&);

    void update_descriptor(const CommandBuffer&,
                           const Texture&,
                           uint32_t binding = 1);
}  // namespace rainbow::vk

#endif
