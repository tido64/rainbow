// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Vulkan.h"

#include <algorithm>
#include <numeric>
#include <optional>
#include <string_view>
#include <tuple>

#include <vulkan/vulkan.h>

#include "Common/Algorithm.h"
#include "Common/Logging.h"
#include "Graphics/Image.h"
#include "Graphics/SpriteVertex.h"
#include "Resources/Fixed2D.frag.spv.h"
#include "Resources/Fixed2D.vert.spv.h"
#include "ThirdParty/VulkanMemoryAllocator.h"

using rainbow::Image;

using namespace rainbow::vk;
using namespace std::literals::string_view_literals;

namespace
{
    auto to_string(VkResult result)
    {
#define CASE_VK_RESULT(result)                                                 \
    case VK_##result:                                                          \
        return #result

        switch (result)
        {
            CASE_VK_RESULT(NOT_READY);
            CASE_VK_RESULT(TIMEOUT);
            CASE_VK_RESULT(EVENT_SET);
            CASE_VK_RESULT(EVENT_RESET);
            CASE_VK_RESULT(INCOMPLETE);
            CASE_VK_RESULT(ERROR_OUT_OF_HOST_MEMORY);
            CASE_VK_RESULT(ERROR_OUT_OF_DEVICE_MEMORY);
            CASE_VK_RESULT(ERROR_INITIALIZATION_FAILED);
            CASE_VK_RESULT(ERROR_DEVICE_LOST);
            CASE_VK_RESULT(ERROR_MEMORY_MAP_FAILED);
            CASE_VK_RESULT(ERROR_LAYER_NOT_PRESENT);
            CASE_VK_RESULT(ERROR_EXTENSION_NOT_PRESENT);
            CASE_VK_RESULT(ERROR_FEATURE_NOT_PRESENT);
            CASE_VK_RESULT(ERROR_INCOMPATIBLE_DRIVER);
            CASE_VK_RESULT(ERROR_TOO_MANY_OBJECTS);
            CASE_VK_RESULT(ERROR_FORMAT_NOT_SUPPORTED);
            CASE_VK_RESULT(ERROR_FRAGMENTED_POOL);
            CASE_VK_RESULT(ERROR_OUT_OF_POOL_MEMORY);
            CASE_VK_RESULT(ERROR_INVALID_EXTERNAL_HANDLE);
            CASE_VK_RESULT(ERROR_SURFACE_LOST_KHR);
            CASE_VK_RESULT(ERROR_NATIVE_WINDOW_IN_USE_KHR);
            CASE_VK_RESULT(SUBOPTIMAL_KHR);
            CASE_VK_RESULT(ERROR_OUT_OF_DATE_KHR);
            CASE_VK_RESULT(ERROR_INCOMPATIBLE_DISPLAY_KHR);
            CASE_VK_RESULT(ERROR_VALIDATION_FAILED_EXT);
            CASE_VK_RESULT(ERROR_INVALID_SHADER_NV);
            CASE_VK_RESULT(ERROR_FRAGMENTATION_EXT);
            CASE_VK_RESULT(ERROR_NOT_PERMITTED_EXT);
            default:
                return "UNKNOWN_ERROR";
        }

#undef CASE_VK_RESULT
    }

    auto begin_frame(FrameInfo& info) -> FrameInfo&
    {
        info.bindings.index_buffer = VK_NULL_HANDLE;
        return info;
    }

    template <typename T, typename F, typename... Args>
    auto get_array(F&& f, Args&&... args)
    {
        uint32_t count;
        f(std::forward<Args>(args)..., &count, nullptr);

        auto array = std::make_unique<T[]>(count);
        f(std::forward<Args>(args)..., &count, array.get());

        return std::make_tuple(std::move(array), count);
    }

    auto image_format(const rainbow::Image& image)
    {
        using rainbow::Image;

        switch (image.format)
        {
            case Image::Format::Unknown:
                return VK_FORMAT_UNDEFINED;

            case Image::Format::ATITC:  // Adreno
                return VK_FORMAT_UNDEFINED;

            case Image::Format::BC1:
                switch (image.channels)
                {
                    case 3:
                        return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
                    case 4:
                        return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
                    default:
                        return VK_FORMAT_UNDEFINED;
                }

            case Image::Format::BC2:
                return VK_FORMAT_BC2_UNORM_BLOCK;

            case Image::Format::BC3:
                return VK_FORMAT_BC3_UNORM_BLOCK;

            case Image::Format::ETC1:  // OpenGL ES standard
                return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;

            case Image::Format::ETC2:  // OpenGL ES 3.0 standard
                switch (image.depth)
                {
                    case 24:
                        return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
                    case 25:
                        return VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK;
                    case 32:
                        return VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
                    default:
                        return VK_FORMAT_UNDEFINED;
                }

            case Image::Format::PVRTC:  // iOS, OMAP43xx, PowerVR
                return image.depth == 2 ? VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG
                                        : VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG;

            case Image::Format::PNG:
            case Image::Format::SVG:
                switch (image.channels)
                {
                    case 1:
                        return VK_FORMAT_R8_UNORM;
                    case 2:
                        return VK_FORMAT_R8G8_UNORM;
                    case 3:
                        return image.depth == 16 ? VK_FORMAT_B5G6R5_UNORM_PACK16
                                                 : VK_FORMAT_B8G8R8_UNORM;
                    case 4:
                        return image.depth == 16
                                   ? VK_FORMAT_B4G4R4A4_UNORM_PACK16
                                   : VK_FORMAT_B8G8R8A8_UNORM;
                    default:
                        return VK_FORMAT_UNDEFINED;
                }
        }

        // Suppresses warning C4715: not all control paths return a value
        return VK_FORMAT_UNDEFINED;
    }

    auto make_buffer(const LogicalDevice& device,
                     size_t size,
                     BufferUsageFlags usage)
    {
        const VkBufferCreateInfo info{
            /* sType */ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            /* pNext */ nullptr,
            /* flags */ 0,
            /* size */ size,
            /* usage */ static_cast<VkBufferUsageFlags>(usage),
            /* sharingMode */ VK_SHARING_MODE_EXCLUSIVE,
            /* queueFamilyIndexCount */ 0,
            /* pQueueFamilyIndices */ nullptr,
        };
        const VmaAllocationCreateInfo allocation_info{
            /* flags */ VMA_ALLOCATION_CREATE_MAPPED_BIT,
            /* usage */ VMA_MEMORY_USAGE_CPU_TO_GPU,
            /* requiredFlags */ VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            /* preferredFlags */ VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
            /* memoryTypeBits */ (1u << device.memory_type_index()),
            /* pool */ VK_NULL_HANDLE,
            /* pUserData */ nullptr,
        };

        VkBuffer buffer = VK_NULL_HANDLE;
        VmaAllocation allocation = VK_NULL_HANDLE;
        if (VkResult result = vmaCreateBuffer(device.allocator(),
                                              &info,
                                              &allocation_info,
                                              &buffer,
                                              &allocation,
                                              nullptr))
        {
            R_ABORT("Vulkan: Failed to create buffer: %s", to_string(result));
        }

        return std::make_tuple(buffer, allocation);
    }

    auto make_default_descriptor_pool(VkDevice device, uint32_t max_sets)
    {
        VkDescriptorPool pool;

        const VkDescriptorPoolSize pool_sizes[]{
            {
                /* type */ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                /* descriptorCount */ max_sets,
            },
            {
                /* type */ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                /* descriptorCount */ max_sets,
            },
        };

        const VkDescriptorPoolCreateInfo pool_info{
            /* sType */ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            /* pNext */ nullptr,
            /* flags */ 0,
            /* maxSets */ max_sets,
            /* poolSizeCount */
            rainbow::narrow_cast<uint32_t>(rainbow::array_size(pool_sizes)),
            /* pPoolSizes */ pool_sizes,
        };

        if (VkResult result =
                vkCreateDescriptorPool(device, &pool_info, nullptr, &pool))
        {
            R_ABORT("Vulkan: Failed to create descriptor pool (%s)",
                    to_string(result));
        }

        return pool;
    }

    auto make_default_descriptor_set_layout(VkDevice device)
    {
        VkDescriptorSetLayout layout;

        constexpr VkDescriptorSetLayoutBinding bindings[]{
            {
                /* binding */ 0,
                /* descriptorType */ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                /* descriptorCount */ 1,
                /* stageFlags */ VK_SHADER_STAGE_VERTEX_BIT,
                /* pImmutableSamplers */ nullptr,
            },
            {
                /* binding */ 1,
                /* descriptorType */ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                /* descriptorCount */ 1,
                /* stageFlags */ VK_SHADER_STAGE_FRAGMENT_BIT,
                /* pImmutableSamplers */ nullptr,
            },
        };

        const VkDescriptorSetLayoutCreateInfo layout_info{
            /* sType */ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            /* pNext */ nullptr,
            /* flags */ 0,
            /* bindingCount */
            rainbow::narrow_cast<uint32_t>(rainbow::array_size(bindings)),
            /* pBindings */ bindings,
        };

        if (VkResult result = vkCreateDescriptorSetLayout(
                device, &layout_info, nullptr, &layout))
        {
            R_ABORT("Vulkan: Failed to create descriptor set layout (%s)",
                    to_string(result));
        }

        return layout;
    }

    template <typename T, size_t N>
    auto make_shader_module(const LogicalDevice& device,
                            const std::array<T, N>& code)
    {
        return make_shader_module(
            device,
            ArraySpan<uint8_t>(const_cast<uint8_t*>(code.data()), code.size()));
    }

    auto memory_type_score(VkMemoryPropertyFlags property_flags,
                           VkMemoryHeap heap) -> uint32_t
    {
        constexpr struct
        {
            uint32_t property;
            uint32_t score;
        } preferred_props[]{
            {/* property */ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
             /* score */ 64},
            {/* property */ VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
             /* score */ 32},
            {/* property */ VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
             /* score */ 8},
            {/* property */ VK_MEMORY_PROPERTY_HOST_CACHED_BIT, /* score */ 8},
        };

        return std::reduce(  //
            std::begin(preferred_props),
            std::end(preferred_props),
            heap.size / 1e6,
            [property_flags](auto score, auto&& prop) {
                return ((property_flags & prop.property) != 0)
                           ? score + prop.score
                           : score;
            });
    }

    auto preferred_memory_type(VkPhysicalDevice device)
    {
        uint32_t current_best = 0;
        uint32_t current_score = 0;
        VkPhysicalDeviceMemoryProperties mem_props{};
        vkGetPhysicalDeviceMemoryProperties(device, &mem_props);
        for (auto i = 0u; i < mem_props.memoryTypeCount; ++i)
        {
            const VkMemoryType type = mem_props.memoryTypes[i];
            const VkMemoryHeap heap = mem_props.memoryHeaps[type.heapIndex];
            const uint32_t score = memory_type_score(type.propertyFlags, heap);
            if (score > current_score)
            {
                current_best = i;
                current_score = score;
            }
        }
        return std::make_tuple(current_best, current_score);
    }

    auto preferred_queue_family(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        auto [queue_families, count] = get_array<VkQueueFamilyProperties>(
            vkGetPhysicalDeviceQueueFamilyProperties, device);

        const auto i = std::find_if(  //
            queue_families.get(),
            queue_families.get() + count,
            [device, surface, i = 0u](auto&& family) mutable {
                if (family.queueCount > 0 &&
                    (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
                {
                    VkBool32 supported;
                    vkGetPhysicalDeviceSurfaceSupportKHR(
                        device, i++, surface, &supported);
                    return supported == VK_TRUE;
                }

                ++i;
                return false;
            });

        const auto index = std::distance(queue_families.get(), i);
        return index == count ? std::nullopt : std::make_optional(index);
    }

    auto preferred_surface_format(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        auto [formats, count] = get_array<VkSurfaceFormatKHR>(
            vkGetPhysicalDeviceSurfaceFormatsKHR, device, surface);

        if (count == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
        {
            return VkSurfaceFormatKHR{
                /* format */ VK_FORMAT_B8G8R8A8_UNORM,
                /* colorSpace */ VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
            };
        }

        for (size_t i = 0; i < count; ++i)
        {
            auto format = formats[i];
            if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
                format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return format;
            }
        }

        return formats[0];
    }

    auto platform_surface_extension() -> const char*
    {
        constexpr std::string_view kSurfaceExtensions[]{
            // Windows
            "VK_KHR_win32_surface"sv,
            // Nintendo Switch
            "VK_NN_vi_surface"sv,
            // Android (API level 24+)
            "VK_KHR_android_surface"sv,
            // iOS (MoltenVK)
            "VK_MVK_ios_surface"sv,
            // macOS (MoltenVK)
            "VK_MVK_macos_surface"sv,
            // Linux
            "VK_KHR_wayland_surface"sv,
            "VK_KHR_xcb_surface"sv,
            "VK_KHR_xlib_surface"sv,
        };

        auto [extensions, count] = get_array<VkExtensionProperties>(
            vkEnumerateInstanceExtensionProperties,
            static_cast<const char*>(nullptr));

        auto i = std::find_first_of(  //
            std::begin(kSurfaceExtensions),
            std::end(kSurfaceExtensions),
            extensions.get(),
            extensions.get() + count,
            [](auto&& name, auto&& props) {
                return name == props.extensionName;
            });
        if (i == std::end(kSurfaceExtensions))
            return nullptr;

        return i->data();
    }

    auto uniform_buffer_aligned_size(const LogicalDevice& device, size_t size)
    {
        const auto alignment = device.min_uniform_buffer_offset_alignment();
        return rainbow::truncate<VkDeviceSize>(
            std::ceil(rainbow::narrow_cast<float>(size) / alignment) *
            alignment);
    }

    void wait_and_reset(VkDevice device, VkFence fence)
    {
        vkWaitForFences(
            device, 1, &fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
        vkResetFences(device, 1, &fence);
    }

#ifndef NDEBUG
    constexpr std::string_view kValidationLayers[]{
        "VK_LAYER_LUNARG_standard_validation"sv,
    };

    void list_devices(ArraySpan<VkPhysicalDevice> devices,
                      VkPhysicalDevice pick)
    {
        LOGI("Detected GPUs:");
        std::for_each_n(devices.data(), devices.size(), [pick](auto&& device) {
            VkPhysicalDeviceProperties props{};
            vkGetPhysicalDeviceProperties(device, &props);
            LOGI("  %s%s",
                 props.deviceName,
                 device == pick ? " (selected)" : "");
        });
    }

    auto validation_layers()
    {
        std::vector<const char*> layers;

        auto [layer_props, count] =
            get_array<VkLayerProperties>(vkEnumerateInstanceLayerProperties);

        std::for_each_n(layer_props.get(), count, [&layers](auto&& props) {
            for (auto&& layer : kValidationLayers)
            {
                if (layer == props.layerName)
                {
                    layers.push_back(layer.data());
                    break;
                }
            }
        });

        return layers;
    }
#endif
}  // namespace

Instance::Instance(const VkApplicationInfo& app_info)
{
    const auto surface_extension = platform_surface_extension();
    if (surface_extension == nullptr)
    {
        LOGF("Vulkan: Failed to find platform surface extension");
        return;
    }

    VkInstanceCreateInfo create_info{
        /* sType */ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        /* pNext */ nullptr,
        /* flags */ 0,
        /* pApplicationInfo */ &app_info,
        /* enabledLayerCount */ 0,
        /* ppEnabledLayerNames */ nullptr,
        /* enabledExtensionCount */ 0,
        /* ppEnabledExtensionNames */ nullptr,
    };

#ifndef NDEBUG
    const auto& layers = validation_layers();
    create_info.enabledLayerCount = layers.size();
    create_info.ppEnabledLayerNames = layers.data();
#endif

    const char* const extensions[]{
        VK_KHR_SURFACE_EXTENSION_NAME,
        surface_extension,
#ifndef NDEBUG
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
    };
    create_info.enabledExtensionCount = array_size(extensions);
    create_info.ppEnabledExtensionNames = extensions;

    if (VkResult result = vkCreateInstance(&create_info, nullptr, &instance_))
        LOGF("Failed to initialise Vulkan (%s)", to_string(result));
}

Instance::~Instance()
{
    vkDestroyInstance(instance_, nullptr);
}

Surface::Surface(Instance& instance, PlatformWindow window)
    : instance_(instance)
{
    if (!instance)
        return;

    if (VkResult result = create(window))
        LOGF("Vulkan: Failed to create surface (%s)", to_string(result));
}

Surface::~Surface()
{
    if (surface_ == VK_NULL_HANDLE)
        return;

    vkDestroySurfaceKHR(instance_, surface_, nullptr);
}

PhysicalDevice::PhysicalDevice(const Instance& instance)
{
    if (!instance)
        return;

    auto [devices, device_count] =
        get_array<VkPhysicalDevice>(vkEnumeratePhysicalDevices, instance);
    if (device_count == 0)
    {
        LOGF("Vulkan: No GPUs found");
        return;
    }

    std::vector<VkExtensionProperties> available_extensions;
    device_ = std::reduce(  //
        devices.get(),
        devices.get() + device_count,
        device_,
        [&available_extensions,
         &memory_type_index = memory_type_index_,
         &min_uniform_buffer_offset_alignment =
             min_uniform_buffer_offset_alignment_,
         top_score = 0u](auto best_device, auto device) mutable {
            uint32_t score = 0;

            // Prefer discrete GPUs over integrated ones
            VkPhysicalDeviceProperties props{};
            vkGetPhysicalDeviceProperties(device, &props);
            switch (props.deviceType)
            {
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    score += 100000;
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    score += 200000;
                    break;
                default:
                    return best_device;
            }

            // Require swap chain support
            uint32_t extension_count;
            vkEnumerateDeviceExtensionProperties(
                device, nullptr, &extension_count, nullptr);
            available_extensions.resize(extension_count);

            vkEnumerateDeviceExtensionProperties(
                device, nullptr, &extension_count, available_extensions.data());
            if (std::none_of(  //
                    std::begin(available_extensions),
                    std::end(available_extensions),
                    [](auto&& props) {
                        return props.extensionName == "VK_KHR_swapchain"sv;
                    }))
            {
                return best_device;
            }

            // More memory is better
            auto [i, memory_score] = preferred_memory_type(device);
            score += memory_score;

            if (score > top_score)
            {
                memory_type_index = i;
                min_uniform_buffer_offset_alignment =
                    props.limits.minUniformBufferOffsetAlignment;
                top_score = score;
                return device;
            }

            return best_device;
        });

    IF_DEBUG(list_devices({devices.get(), device_count}, device_));

    if (device_ == VK_NULL_HANDLE)
        LOGF("Vulkan: No eligible GPUs found");
}

LogicalDevice::LogicalDevice(PhysicalDevice physical_device,
                             const Surface& surface)
    : min_uniform_buffer_offset_alignment_(
          physical_device.min_uniform_buffer_offset_alignment()),
      memory_type_index_(physical_device.memory_type_index())
{
    if (!physical_device || !surface)
        return;

    const auto index = preferred_queue_family(physical_device, surface);
    if (!index)
    {
        LOGF("Vulkan: Failed to find an eligible device queue family");
        return;
    }

    VkDeviceCreateInfo create_info{
        /* sType */ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        /* pNext */ nullptr,
        /* flags */ 0,
        /* queueCreateInfoCount */ 0,
        /* pQueueCreateInfos */ nullptr,
        /* enabledLayerCount */ 0,
        /* ppEnabledLayerNames */ nullptr,
        /* enabledExtensionCount */ 0,
        /* ppEnabledExtensionNames */ nullptr,
        /* pEnabledFeatures */ nullptr,
    };

    VkDeviceQueueCreateInfo queue_create_info{
        /* sType */ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        /* pNext */ nullptr,
        /* flags */ 0,
        /* queueFamilyIndex */ narrow_cast<uint32_t>(*index),
        /* queueCount */ 1,
        /* pQueuePriorities */ nullptr,
    };

    float priority = 1.0f;
    queue_create_info.pQueuePriorities = &priority;

    create_info.queueCreateInfoCount = 1;
    create_info.pQueueCreateInfos = &queue_create_info;

    constexpr const char* const extensions[]{
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };
    create_info.enabledExtensionCount = array_size(extensions);
    create_info.ppEnabledExtensionNames = extensions;

    // VkPhysicalDeviceFeatures features{};
    // create_info.pEnabledFeatures = &features;

    if (VkResult result =
            vkCreateDevice(physical_device, &create_info, nullptr, &device_))
    {
        LOGF("Vulkan: Failed to create logical device (%s)", to_string(result));
        return;
    }

    const VmaAllocatorCreateInfo allocator_info{
        /* flags */ VMA_ALLOCATOR_CREATE_EXTERNALLY_SYNCHRONIZED_BIT,
        /* physicalDevice */ physical_device,
        /* device */ device_,
        /* preferredLargeHeapBlockSize */ 0,
        /* pAllocationCallbacks */ nullptr,
        /* pDeviceMemoryCallbacks */ nullptr,
        /* frameInUseCount */ 1,
        /* pHeapSizeLimit */ nullptr,
        /* pVulkanFunctions */ nullptr,
        /* pRecordSettings */ nullptr,
    };

    vmaCreateAllocator(&allocator_info, &allocator_);

    const VkCommandPoolCreateInfo pool_info{
        /* sType */ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        /* pNext */ nullptr,
        /* flags */ VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        /* queueFamilyIndex */ narrow_cast<uint32_t>(*index),
    };

    vkCreateCommandPool(device_, &pool_info, nullptr, &command_pool_);

    vkGetDeviceQueue(device_, *index, 0, &queue_);

#ifndef NDEBUG
    VkPhysicalDeviceProperties props{};
    vkGetPhysicalDeviceProperties(physical_device, &props);
    std::copy(std::begin(props.deviceName),
              std::end(props.deviceName),
              std::begin(properties_.device_name));

    properties_.api_version = props.apiVersion;

    VkPhysicalDeviceMemoryProperties mem_props{};
    vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_props);
    const VkMemoryType type = mem_props.memoryTypes[memory_type_index_];
    const VkMemoryHeap heap = mem_props.memoryHeaps[type.heapIndex];
    properties_.memory_heap_size = heap.size;
#endif
}

LogicalDevice::~LogicalDevice()
{
    vkDestroyCommandPool(device_, command_pool_, nullptr);
    vmaDestroyAllocator(allocator_);
    vkDestroyDevice(device_, nullptr);
}

Swapchain::Swapchain(PhysicalDevice device, const Surface& surface)
    : device_(device, surface)
{
    if (!device_)
        return;

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities);

    surface_format_ = preferred_surface_format(device, surface);

    const VkSwapchainCreateInfoKHR create_info{
        /* sType */ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        /* pNext */ nullptr,
        /* flags */ 0,
        /* surface */ surface,
        /* minImageCount */ 2,
        /* imageFormat */ surface_format_.format,
        /* imageColorSpace */ surface_format_.colorSpace,
        /* imageExtent */ capabilities.currentExtent,
        /* imageArrayLayers */ 1,
        /* imageUsage */ VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        /* imageSharingMode */ VK_SHARING_MODE_EXCLUSIVE,
        /* queueFamilyIndexCount */ 0,
        /* pQueueFamilyIndices */ nullptr,
        /* preTransform */ capabilities.currentTransform,
        /* compositeAlpha */ VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        /* presentMode */ VK_PRESENT_MODE_FIFO_KHR,
        /* clipped */ VK_TRUE,
        /* oldSwapchain */ VK_NULL_HANDLE,
    };

    if (VkResult result =
            vkCreateSwapchainKHR(device_, &create_info, nullptr, &swapchain_))
    {
        LOGF("Vulkan: Failed to create swap chain (%s)", to_string(result));
        return;
    }

    image_extent_ = capabilities.currentExtent;

    auto [images, count] =
        get_array<VkImage>(vkGetSwapchainImagesKHR, device_, swapchain_);
    views_.reserve(count);
    std::transform(  //
        images.get(),
        images.get() + count,
        std::back_inserter(views_),
        [device = VkDevice{device_}, format = image_format()](auto&& image) {
            return make_image_view(device, image, format);
        });

    LOGI("Vulkan: Resolution: %ix%i",  //
         image_extent_.width,
         image_extent_.height);
}

Swapchain::~Swapchain()
{
    if (swapchain_ == VK_NULL_HANDLE)
        return;

    for (auto&& view : views_)
        vkDestroyImageView(device_, view, nullptr);

    vkDestroySwapchainKHR(device_, swapchain_, nullptr);
}

Pipeline::Pipeline(const Swapchain& swapchain, VkPipeline base_pipeline)
    : device_(swapchain.device())
{
    if (!swapchain)
        return;

    descriptor_set_layout_ = make_default_descriptor_set_layout(device_);
    descriptor_pool_ = make_default_descriptor_pool(
        device_, narrow_cast<uint32_t>(swapchain.image_views().size()));

    VkGraphicsPipelineCreateInfo pipeline_info{
        /* sType */ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        /* pNext */ nullptr,
        /* flags */ base_pipeline == VK_NULL_HANDLE
            ? static_cast<VkPipelineCreateFlags>(
                  VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT)
            : static_cast<VkPipelineCreateFlags>(
                  VK_PIPELINE_CREATE_DERIVATIVE_BIT),
        /* stageCount */ 0,
        /* pStages */ nullptr,
        /* pVertexInputState */ nullptr,
        /* pInputAssemblyState */ nullptr,
        /* pTessellationState */ nullptr,
        /* pViewportState */ nullptr,
        /* pRasterizationState */ nullptr,
        /* pMultisampleState */ nullptr,
        /* pDepthStencilState */ nullptr,
        /* pColorBlendState */ nullptr,
        /* pDynamicState */ nullptr,
        /* layout */ nullptr,
        /* renderPass */ nullptr,
        /* subpass */ 0,
        /* basePipelineHandle */ nullptr,
        /* basePipelineIndex */ 0,
    };

    // Shader stages

    vertex_shader_ = ::make_shader_module(device_, assets::kFixed2D_vert_spv);
    fragment_shader_ = ::make_shader_module(device_, assets::kFixed2D_frag_spv);

    const VkPipelineShaderStageCreateInfo shader_stages[]{
        make_shader_stage_info(ShaderStageFlags::Vertex, vertex_shader_),
        make_shader_stage_info(ShaderStageFlags::Fragment, fragment_shader_),
    };

    pipeline_info.stageCount = array_size(shader_stages);
    pipeline_info.pStages = shader_stages;

    // Vertex input description

    VkPipelineVertexInputStateCreateInfo vertex_input_info{
        /* sType */ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        /* pNext */ nullptr,
        /* flags */ 0,
        /* vertexBindingDescriptionCount */ 0,
        /* pVertexBindingDescriptions */ nullptr,
        /* vertexAttributeDescriptionCount */ 0,
        /* pVertexAttributeDescriptions */ nullptr,
    };

    constexpr VkVertexInputBindingDescription vertex_input_binding{
        /* binding */ 0,
        /* stride */ sizeof(SpriteVertex),
        /* inputRate */ VK_VERTEX_INPUT_RATE_VERTEX,
    };

    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = &vertex_input_binding;

    const VkVertexInputAttributeDescription vertex_input_attributes[]{
        {
            /* location */ 0,
            /* binding */ vertex_input_binding.binding,
            /* format */ VK_FORMAT_R8G8B8A8_UINT,
            /* offset */ offsetof(SpriteVertex, color),
        },
        {
            /* location */ 1,
            /* binding */ vertex_input_binding.binding,
            /* format */ VK_FORMAT_R32G32_SFLOAT,
            /* offset */ offsetof(SpriteVertex, texcoord),
        },
        {
            /* location */ 2,
            /* binding */ vertex_input_binding.binding,
            /* format */ VK_FORMAT_R32G32_SFLOAT,
            /* offset */ offsetof(SpriteVertex, position),
        },
    };

    vertex_input_info.vertexAttributeDescriptionCount =
        array_size(vertex_input_attributes);
    vertex_input_info.pVertexAttributeDescriptions = vertex_input_attributes;

    pipeline_info.pVertexInputState = &vertex_input_info;

    // Input assembly

    constexpr VkPipelineInputAssemblyStateCreateInfo assembly_info{
        /* sType */ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        /* pNext */ nullptr,
        /* flags */ 0,
        /* topology */ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        /* primitiveRestartEnable */ VK_FALSE,
    };

    pipeline_info.pInputAssemblyState = &assembly_info;

    // Tesselation

    pipeline_info.pTessellationState = nullptr;

    // Viewport

    VkPipelineViewportStateCreateInfo viewport_info{
        /* sType */ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        /* pNext */ nullptr,
        /* flags */ 0,
        /* viewportCount */ 0,
        /* pViewports */ nullptr,
        /* scissorCount */ 0,
        /* pScissors */ nullptr,
    };

    const VkViewport viewport{
        /* x */ 0.0f,
        /* y */ 0.0f,
        /* width */ narrow_cast<float>(swapchain.image_extent().width),
        /* height */ narrow_cast<float>(swapchain.image_extent().height),
        /* minDepth */ 0.0f,
        /* maxDepth */ 1.0f,
    };

    viewport_info.viewportCount = 1;
    viewport_info.pViewports = &viewport;

    const VkRect2D scissor{
        /* offset */ {0, 0},
        /* extent */ swapchain.image_extent(),
    };

    viewport_info.scissorCount = 1;
    viewport_info.pScissors = &scissor;

    pipeline_info.pViewportState = &viewport_info;

    // Rasterization

    constexpr VkPipelineRasterizationStateCreateInfo rasterizer_info{
        /* sType */ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        /* pNext */ nullptr,
        /* flags */ 0,
        /* depthClampEnable */ VK_FALSE,
        /* rasterizerDiscardEnable */ VK_FALSE,
        /* polygonMode */ VK_POLYGON_MODE_FILL,
        /* cullMode */ VK_CULL_MODE_BACK_BIT,
        /* frontFace */ VK_FRONT_FACE_COUNTER_CLOCKWISE,
        /* depthBiasEnable */ VK_FALSE,
        /* depthBiasConstantFactor */ 0.0f,
        /* depthBiasClamp */ 0.0f,
        /* depthBiasSlopeFactor */ 0.0f,
        /* lineWidth */ 1.0f,
    };

    pipeline_info.pRasterizationState = &rasterizer_info;

    // Multisample

    constexpr VkPipelineMultisampleStateCreateInfo multisample_info{
        /* sType */ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        /* pNext */ nullptr,
        /* flags */ 0,
        /* rasterizationSamples */ VK_SAMPLE_COUNT_1_BIT,
        /* sampleShadingEnable */ VK_FALSE,
        /* minSampleShading */ 0.0f,
        /* pSampleMask */ nullptr,
        /* alphaToCoverageEnable */ VK_FALSE,
        /* alphaToOneEnable */ VK_FALSE,
    };

    pipeline_info.pMultisampleState = &multisample_info;

    // Depth and stencil operations

    pipeline_info.pDepthStencilState = nullptr;

    // Blending

    VkPipelineColorBlendStateCreateInfo color_blend_info{
        /* sType */ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        /* pNext */ nullptr,
        /* flags */ 0,
        /* logicOpEnable */ VK_FALSE,
        /* logicOp */ VK_LOGIC_OP_CLEAR,
        /* attachmentCount */ 0,
        /* pAttachments */ nullptr,
        /* blendConstants */ {1.0f, 1.0f, 1.0f, 1.0f},
    };

    constexpr VkPipelineColorBlendAttachmentState color_blend_attachment{
        /* blendEnable */ VK_TRUE,
        /* srcColorBlendFactor */ VK_BLEND_FACTOR_SRC_ALPHA,
        /* dstColorBlendFactor */ VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        /* colorBlendOp */ VK_BLEND_OP_ADD,
        /* srcAlphaBlendFactor */ VK_BLEND_FACTOR_ONE,
        /* dstAlphaBlendFactor */ VK_BLEND_FACTOR_ZERO,
        /* alphaBlendOp */ VK_BLEND_OP_ADD,
        /* colorWriteMask */ VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT,
    };

    color_blend_info.attachmentCount = 1;
    color_blend_info.pAttachments = &color_blend_attachment;

    pipeline_info.pColorBlendState = &color_blend_info;

    // Dynamic state

    pipeline_info.pDynamicState = nullptr;

    // Pipeline layout

    const VkPipelineLayoutCreateInfo layout_info{
        /* sType */ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        /* pNext */ nullptr,
        /* flags */ 0,
        /* setLayoutCount */ 1,
        /* pSetLayouts */ &descriptor_set_layout_,
        /* pushConstantRangeCount */ 0,
        /* pPushConstantRanges */ nullptr,
    };

    vkCreatePipelineLayout(device_, &layout_info, nullptr, &layout_);

    pipeline_info.layout = layout_;

    // Render pass

    VkRenderPassCreateInfo render_pass_info{
        /* sType */ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        /* pNext */ nullptr,
        /* flags */ 0,
        /* attachmentCount */ 0,
        /* pAttachments */ nullptr,
        /* subpassCount */ 0,
        /* pSubpasses */ nullptr,
        /* dependencyCount */ 0,
        /* pDependencies */ nullptr,
    };

    const VkAttachmentDescription attachment{
        /* flags */ 0,
        /* format */ swapchain.image_format(),
        /* samples */ VK_SAMPLE_COUNT_1_BIT,
        /* loadOp */ VK_ATTACHMENT_LOAD_OP_CLEAR,
        /* storeOp */ VK_ATTACHMENT_STORE_OP_STORE,
        /* stencilLoadOp */ VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        /* stencilStoreOp */ VK_ATTACHMENT_STORE_OP_DONT_CARE,
        /* initialLayout */ VK_IMAGE_LAYOUT_UNDEFINED,
        /* finalLayout */ VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &attachment;

    VkSubpassDescription subpass{
        /* flags */ 0,
        /* pipelineBindPoint */ VK_PIPELINE_BIND_POINT_GRAPHICS,
        /* inputAttachmentCount */ 0,
        /* pInputAttachments */ nullptr,
        /* colorAttachmentCount */ 0,
        /* pColorAttachments */ nullptr,
        /* pResolveAttachments */ nullptr,
        /* pDepthStencilAttachment */ nullptr,
        /* preserveAttachmentCount */ 0,
        /* pPreserveAttachments */ nullptr,
    };

    constexpr VkAttachmentReference color_attachment_ref{
        /* attachment */ 0,
        /* layout */ VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;

    constexpr VkSubpassDependency dependency{
        /* srcSubpass */ VK_SUBPASS_EXTERNAL,
        /* dstSubpass */ 0,
        /* srcStageMask */ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        /* dstStageMask */ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        /* srcAccessMask */ 0,
        /* dstAccessMask */ VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        /* dependencyFlags */ 0,
    };

    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    vkCreateRenderPass(device_, &render_pass_info, nullptr, &render_pass_);

    pipeline_info.renderPass = render_pass_;
    pipeline_info.subpass = 0;

    // Pipeline derivative

    pipeline_info.basePipelineHandle = base_pipeline;
    pipeline_info.basePipelineIndex = -1;

    if (VkResult result = vkCreateGraphicsPipelines(
            device_, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline_))
    {
        LOGF("Vulkan: Failed to create graphics pipeline (%s)",
             to_string(result));
    }
}

Pipeline::~Pipeline()
{
    if (!device_)
        return;

    vkDestroyPipeline(device_, pipeline_, nullptr);
    vkDestroyRenderPass(device_, render_pass_, nullptr);
    vkDestroyPipelineLayout(device_, layout_, nullptr);
    vkDestroyShaderModule(device_, fragment_shader_, nullptr);
    vkDestroyShaderModule(device_, vertex_shader_, nullptr);
    vkDestroyDescriptorPool(device_, descriptor_pool_, nullptr);
    vkDestroyDescriptorSetLayout(device_, descriptor_set_layout_, nullptr);
}

Buffer::Buffer(Buffer&& buffer) noexcept
    : buffer_(buffer.buffer_), allocation_(buffer.allocation_),
      allocator_(buffer.allocator_)
{
    buffer.buffer_ = VK_NULL_HANDLE;
    buffer.allocation_ = VK_NULL_HANDLE;
}

Buffer::Buffer(const LogicalDevice& device, size_t size, BufferUsageFlags usage)
    : allocator_(device.allocator())
{
    std::tie(buffer_, allocation_) = make_buffer(device, size, usage);
}

Buffer::~Buffer()
{
    vmaDestroyBuffer(allocator_, buffer_, allocation_);
}

void Buffer::copy(void* src, size_t size, size_t offset)
{
    uint8_t* dst;
    vmaMapMemory(allocator_, allocation_, reinterpret_cast<void**>(&dst));
    memcpy(dst + offset, src, size);
    vmaUnmapMemory(allocator_, allocation_);
}

void CommandBuffer::run_once(const LogicalDevice& device,
                             rainbow::Callback<void(VkCommandBuffer)> callback)
{
    const VkCommandBufferAllocateInfo info{
        /* sType */ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        /* pNext */ nullptr,
        /* commandPool */ device.command_pool(),
        /* level */ VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        /* commandBufferCount */ 1,
    };

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers(device, &info, &command_buffer);

    constexpr VkCommandBufferBeginInfo begin_info{
        /* sType */ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        /* pNext */ nullptr,
        /* flags */ VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        /* pInheritanceInfo */ nullptr,
    };

    vkBeginCommandBuffer(command_buffer, &begin_info);

    callback(command_buffer);

    vkEndCommandBuffer(command_buffer);

    const VkSubmitInfo submit_info{
        /* sType */ VK_STRUCTURE_TYPE_SUBMIT_INFO,
        /* pNext */ nullptr,
        /* waitSemaphoreCount */ 0,
        /* pWaitSemaphores */ nullptr,
        /* pWaitDstStageMask */ nullptr,
        /* commandBufferCount */ 1,
        /* pCommandBuffers */ &command_buffer,
        /* signalSemaphoreCount */ 0,
        /* pSignalSemaphores */ nullptr,
    };

    auto queue = device.queue();
    vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(device, device.command_pool(), 1, &command_buffer);
}

CommandBuffer::CommandBuffer(const Swapchain& swapchain,
                             const Pipeline& pipeline)
    : swapchain_(swapchain), pipeline_(pipeline)
{
    if (!swapchain)
        return;

    auto buffer_count = narrow_cast<uint32_t>(swapchain.image_views().size());
    frame_info_ = std::make_unique<FrameInfo[]>(buffer_count);

    auto device = VkDevice{swapchain.device()};
    std::transform(  //
        std::begin(swapchain.image_views()),
        std::end(swapchain.image_views()),
        frame_info_.get(),
        [device,
         descriptor_pool = pipeline.descriptor_pool(),
         descriptor_set_layout = pipeline.descriptor_set_layout(),
         render_pass = pipeline.render_pass(),
         extent = swapchain.image_extent()](auto&& image_view) {
            const VkDescriptorSetAllocateInfo descriptor_info{
                /* sType */ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                /* pNext */ nullptr,
                /* descriptorPool */ descriptor_pool,
                /* descriptorSetCount */ 1,
                /* pSetLayouts */ &descriptor_set_layout,
            };

            VkDescriptorSet descriptor_set;
            vkAllocateDescriptorSets(device, &descriptor_info, &descriptor_set);

            const VkFramebufferCreateInfo framebuffer_info{
                /* sType */ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                /* pNext */ nullptr,
                /* flags */ 0,
                /* renderPass */ render_pass,
                /* attachmentCount */ 1,
                /* pAttachments */ &image_view,
                /* width */ extent.width,
                /* height */ extent.height,
                /* layers */ 1,
            };

            VkFramebuffer framebuffer;
            vkCreateFramebuffer(
                device, &framebuffer_info, nullptr, &framebuffer);

            constexpr VkFenceCreateInfo fence_info{
                /* sType */ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                /* pNext */ nullptr,
                /* flags */ 0,
            };

            VkFence fence;
            vkCreateFence(device, &fence_info, nullptr, &fence);

            return FrameInfo{
                DescriptorBindings{},
                /* descriptor_set */ descriptor_set,
                /* command_buffer */ VK_NULL_HANDLE,
                /* framebuffer */ framebuffer,
                /* fence */ fence,
            };
        });

    constexpr VkSemaphoreCreateInfo semaphore_info{
        VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        nullptr,
        0,
    };

    vkCreateSemaphore(device, &semaphore_info, nullptr, &image_available_);
    vkCreateSemaphore(device, &semaphore_info, nullptr, &render_finished_);

    auto command_buffers = std::make_unique<VkCommandBuffer[]>(buffer_count);
    const VkCommandBufferAllocateInfo buffer_alloc_info{
        /* sType */ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        /* pNext */ nullptr,
        /* commandPool */ swapchain.device().command_pool(),
        /* level */ VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        /* commandBufferCount */ buffer_count,
    };

    vkAllocateCommandBuffers(device, &buffer_alloc_info, command_buffers.get());

    for (auto i = 0u; i < buffer_count; ++i)
        frame_info_[i].command_buffer = command_buffers[i];
}

CommandBuffer::~CommandBuffer()
{
    auto device = VkDevice{swapchain_.device()};
    vkDeviceWaitIdle(device);

    vkDestroySemaphore(device, render_finished_, nullptr);
    vkDestroySemaphore(device, image_available_, nullptr);

    std::for_each_n(  //
        frame_info_.get(),
        swapchain_.image_views().size(),
        [device,
         command_pool = swapchain_.device().command_pool()](auto&& info) {
            vkFreeCommandBuffers(device, command_pool, 1, &info.command_buffer);
            vkDestroyFramebuffer(device, info.framebuffer, nullptr);
            vkDestroyFence(device, info.fence, nullptr);
        });
}

void CommandBuffer::begin()
{
    vkAcquireNextImageKHR(  //
        swapchain_.device(),
        swapchain_,
        std::numeric_limits<uint64_t>::max(),
        image_available_,
        VK_NULL_HANDLE,
        &current_);

    const auto& info = begin_frame(frame_info_[current_]);

    constexpr VkCommandBufferBeginInfo begin_info{
        /* sType */ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        /* pNext */ nullptr,
        /* flags */ VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        /* pInheritanceInfo */ nullptr,
    };

    vkBeginCommandBuffer(info.command_buffer, &begin_info);

    vkCmdBindDescriptorSets(  //
        info.command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipeline_.layout(),
        0,
        1,
        &info.descriptor_set,
        0,
        nullptr);

    constexpr VkClearValue clear_color{};
    const VkRenderPassBeginInfo render_pass_begin_info{
        /* sType */ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        /* pNext */ nullptr,
        /* renderPass */ pipeline_.render_pass(),
        /* framebuffer */ info.framebuffer,
        /* renderArea */
        {/* offset */ {0, 0}, /* extent */ swapchain_.image_extent()},
        /* clearValueCount */ 1,
        /* pClearValues */ &clear_color,
    };

    vkCmdBeginRenderPass(  //
        info.command_buffer,
        &render_pass_begin_info,
        VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(
        info.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_);
}

void CommandBuffer::end()
{
    const auto& info = frame_info_[current_];

    vkCmdEndRenderPass(info.command_buffer);
    vkEndCommandBuffer(info.command_buffer);

    constexpr VkPipelineStageFlags wait_stage =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    const VkSubmitInfo submit_info{
        /* sType */ VK_STRUCTURE_TYPE_SUBMIT_INFO,
        /* pNext */ nullptr,
        /* waitSemaphoreCount */ 1,
        /* pWaitSemaphores */ &image_available_,
        /* pWaitDstStageMask */ &wait_stage,
        /* commandBufferCount */ 1,
        /* pCommandBuffers */ &info.command_buffer,
        /* signalSemaphoreCount */ 1,
        /* pSignalSemaphores */ &render_finished_,
    };

    auto queue = swapchain_.device().queue();
    vkQueueSubmit(queue, 1, &submit_info, info.fence);

    wait_and_reset(swapchain_.device(), info.fence);
    vkResetCommandBuffer(info.command_buffer, 0);

    auto swapchain = VkSwapchainKHR{swapchain_};
    const VkPresentInfoKHR present_info{
        /* sType */ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        /* pNext */ nullptr,
        /* waitSemaphoreCount */ 1,
        /* pWaitSemaphores */ &render_finished_,
        /* swapchainCount */ 1,
        /* pSwapchains */ &swapchain,
        /* pImageIndices */ &current_,
        /* pResults */ nullptr,
    };

    vkQueuePresentKHR(queue, &present_info);
}

DynamicBuffer::~DynamicBuffer()
{
    vmaDestroyBuffer(device_.allocator(), buffer_, allocation_);
}

void DynamicBuffer::copy(void* src, size_t size)
{
    if (size > capacity_)
    {
        capacity_ = size;
        vmaDestroyBuffer(device_.allocator(), buffer_, allocation_);
        std::tie(buffer_, allocation_) = make_buffer(device_, size, usage_);
    }

    void* dst;
    vmaMapMemory(device_.allocator(), allocation_, &dst);
    memcpy(dst, src, size);
    vmaUnmapMemory(device_.allocator(), allocation_);
}

IndexBuffer::IndexBuffer(const LogicalDevice& device)
    : Buffer(device,
             0xffffu / 4 * 6 * sizeof(uint16_t),
             BufferUsageFlags::TransferDst | BufferUsageFlags::IndexBuffer)
{
    constexpr uint32_t kIndexBufferSize = kMaxSprites * 6;

    auto default_indices = std::make_unique<uint16_t[]>(kIndexBufferSize);
    for (uint32_t i = 0; i < kMaxSprites; ++i)
    {
        const auto index = i * 6;
        const auto vertex = narrow_cast<uint16_t>(i * 4);
        default_indices[index] = vertex;
        default_indices[index + 1] = vertex + 1;
        default_indices[index + 2] = vertex + 2;
        default_indices[index + 3] = vertex + 2;
        default_indices[index + 4] = vertex + 3;
        default_indices[index + 5] = vertex;
    }
    copy(default_indices.get(), kIndexBufferSize * sizeof(default_indices[0]));
}

ProjectionMatrix::ProjectionMatrix(const Swapchain& swapchain)
    : Buffer(swapchain.device(),
             uniform_buffer_aligned_size(swapchain.device(), size()) *
                 swapchain.image_views().size(),
             BufferUsageFlags::TransferDst | BufferUsageFlags::UniformBuffer),
      aligned_size_(uniform_buffer_aligned_size(swapchain.device(), size()))
{
    const auto extent = swapchain.image_extent();
    const VkViewport viewport{
        /* x */ 0.0f,
        /* y */ 0.0f,
        /* width */ float(extent.width),
        /* height */ float(extent.height),
        /* minDepth */ 0.0f,
        /* maxDepth */ 0.0f,
    };
    for (size_t i = 0; i < swapchain.image_views().size(); ++i)
        set(viewport, i);
}

void ProjectionMatrix::set(const VkViewport& viewport, uint32_t index)
{
    const float norm_width = 2.0f / (viewport.width - viewport.x);

#ifdef RAINBOW_OS_MACOS
    // The coordinate system is flipped in shaders:
    // https://github.com/KhronosGroup/MoltenVK/blob/v1.0.29/MoltenVK/MoltenVK/Utility/MVKEnvironment.h#L42
    constexpr float y_axis = 1.0f;
#else
    constexpr float y_axis = -1.0f;
#endif
    const float norm_height = y_axis * -2.0f / (viewport.height - viewport.y);

    // The orthographic projection matrix is defined as:
    //
    //   | 2 / (r - l)       0             0       -(r + l) / (r - l) |
    //   |      0       2 / (t - b)        0       -(t + b) / (t - b) |
    //   |      0            0       -2 / (f - n)  -(f + n) / (f - n) |
    //   |      0            0             0                1         |
    //
    // Where <c>b</c> = bottom, <c>f</c> = far, <c>l</c> = left, <c>n</c> =
    // near, <c>r</c> = right, <c>t</c> = top, and near = -1.0 and far = 1.0.
    // The matrix is stored in column-major order.
    // clang-format off
    float projection[16]{
        norm_width,         0.0f,  0.0f,  0.0f,
              0.0f,  norm_height,  0.0f,  0.0f,
              0.0f,         0.0f, -1.0f,  0.0f,
             -1.0f,       y_axis,  0.0f,  1.0f,
    };
    // clang-format on

    Buffer::copy(projection, sizeof(projection), aligned_size_ * index);
}

Texture::Texture(const LogicalDevice& device,
                 const Image& image,
                 Filter mag_filter,
                 Filter min_filter)
{
    const VkImageCreateInfo info{
        /* sType */ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        /* pNext */ nullptr,
        /* flags */ 0,
        /* imageType */ VK_IMAGE_TYPE_2D,
        /* format */ image_format(image),
        /* extent */
        {
            /* width */ image.width,
            /* height */ image.height,
            /* depth */ 1,
        },
        /* mipLevels */ 1,
        /* arrayLayers */ 1,
        /* samples */ VK_SAMPLE_COUNT_1_BIT,
        /* tiling */ VK_IMAGE_TILING_OPTIMAL,
        /* usage */ VK_IMAGE_USAGE_TRANSFER_DST_BIT |
            VK_IMAGE_USAGE_SAMPLED_BIT,
        /* sharingMode */ VK_SHARING_MODE_EXCLUSIVE,
        /* queueFamilyIndexCount */ 0,
        /* pQueueFamilyIndices */ nullptr,
        /* initialLayout */ VK_IMAGE_LAYOUT_UNDEFINED,
    };
    const VmaAllocationCreateInfo allocation_info{
        /* flags */ 0,
        /* usage */ VMA_MEMORY_USAGE_GPU_ONLY,
        /* requiredFlags */ 0,
        /* preferredFlags */ 0,
        /* memoryTypeBits */ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        /* pool */ VK_NULL_HANDLE,
        /* pUserData */ nullptr,
    };

    if (VkResult result = vmaCreateImage(device.allocator(),
                                         &info,
                                         &allocation_info,
                                         &image_,
                                         &allocation_,
                                         nullptr))
    {
        R_ABORT("Vulkan: Failed to create image (%s)", to_string(result));
    }

    view_ = make_image_view(device, image_, info.format);

    const VkSamplerCreateInfo sampler_info{
        /* sType */ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        /* pNext */ nullptr,
        /* flags */ 0,
        /* magFilter */ static_cast<VkFilter>(mag_filter),
        /* minFilter */ static_cast<VkFilter>(min_filter),
        /* mipmapMode */ VK_SAMPLER_MIPMAP_MODE_LINEAR,
        /* addressModeU */ VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
        /* addressModeV */ VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
        /* addressModeW */ VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
        /* mipLodBias */ 0.0f,
        /* anisotropyEnable */ VK_FALSE,
        /* maxAnisotropy */ 1.0f,
        /* compareEnable */ VK_FALSE,
        /* compareOp */ VK_COMPARE_OP_NEVER,
        /* minLod */ 0.0f,
        /* maxLod */ 0.0f,
        /* borderColor */ VK_BORDER_COLOR_INT_TRANSPARENT_BLACK,
        /* unnormalizedCoordinates */ VK_FALSE,
    };

    vkCreateSampler(device, &sampler_info, nullptr, &sampler_);

    update(device, image);
}

void Texture::update(const LogicalDevice& device, const Image& image) const
{
    if (image.data == nullptr)
        return;

    StagingBuffer staging_buffer(device, image.size);
    staging_buffer.copy(
        ArraySpan<uint8_t>(const_cast<uint8_t*>(image.data), image.size));

    CommandBuffer::run_once(
        device,
        [image = image_,
         extent = VkExtent3D{image.width, image.height, 1},
         staging_buffer =
             VkBuffer{staging_buffer}](VkCommandBuffer command_buffer) {
            VkImageMemoryBarrier barrier{
                /* sType */ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                /* pNext */ nullptr,
                /* srcAccessMask */ 0,
                /* dstAccessMask */ VK_ACCESS_TRANSFER_WRITE_BIT,
                /* oldLayout */ VK_IMAGE_LAYOUT_UNDEFINED,
                /* newLayout */ VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                /* srcQueueFamilyIndex */ VK_QUEUE_FAMILY_IGNORED,
                /* dstQueueFamilyIndex */ VK_QUEUE_FAMILY_IGNORED,
                /* image */ image,
                /* subresourceRange */
                {
                    /* aspectMask */ VK_IMAGE_ASPECT_COLOR_BIT,
                    /* baseMipLevel */ 0,
                    /* levelCount */ 1,
                    /* baseArrayLayer */ 0,
                    /* layerCount */ 1,
                },
            };

            vkCmdPipelineBarrier(  //
                command_buffer,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                0,
                0,
                nullptr,
                0,
                nullptr,
                1,
                &barrier);

            const VkBufferImageCopy region{
                /* bufferOffset */ 0,
                /* bufferRowLength */ 0,
                /* bufferImageHeight */ 0,
                /* imageSubresource */
                {
                    /* aspectMask */ VK_IMAGE_ASPECT_COLOR_BIT,
                    /* mipLevel */ 0,
                    /* baseArrayLayer */ 0,
                    /* layerCount */ 1,
                },
                /* imageOffset */ {0, 0, 0},
                /* imageExtent */ extent,
            };

            vkCmdCopyBufferToImage(  //
                command_buffer,
                staging_buffer,
                image,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &region);

            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            vkCmdPipelineBarrier(  //
                command_buffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                0,
                0,
                nullptr,
                0,
                nullptr,
                1,
                &barrier);
        });
}

void Texture::destroy(const LogicalDevice& device)
{
    vkDestroySampler(device, sampler_, nullptr);
    vkDestroyImageView(device, view_, nullptr);
    vmaDestroyImage(device.allocator(), image_, allocation_);

    sampler_ = VK_NULL_HANDLE;
    view_ = VK_NULL_HANDLE;
    image_ = VK_NULL_HANDLE;
    allocation_ = VK_NULL_HANDLE;
}

void rainbow::vk::bind(const CommandBuffer& command_buffer,
                       const DynamicVertexBuffer& vertex_buffer)
{
    constexpr VkDeviceSize offset = 0;
    const VkBuffer buffer = vertex_buffer;
    vkCmdBindVertexBuffers(command_buffer, 0, 1, &buffer, &offset);
}

void rainbow::vk::bind(const CommandBuffer& command_buffer,
                       const IndexBuffer& index_buffer)
{
    if (index_buffer == command_buffer.bindings().index_buffer)
        return;

    command_buffer.bindings().index_buffer = index_buffer;

    vkCmdBindIndexBuffer(command_buffer, index_buffer, 0, VK_INDEX_TYPE_UINT16);
}

void rainbow::vk::bind(const CommandBuffer& command_buffer,
                       const VertexBuffer& vertex_buffer)
{
    constexpr VkDeviceSize offset = 0;
    const VkBuffer buffer = vertex_buffer;
    vkCmdBindVertexBuffers(command_buffer, 0, 1, &buffer, &offset);
}

void rainbow::vk::draw(const CommandBuffer& command_buffer,
                       const VertexBuffer& vertex_buffer,
                       uint32_t vertex_count)
{
    bind(command_buffer, vertex_buffer);
    vkCmdDraw(command_buffer, vertex_count, 1, 0, 0);
}

void rainbow::vk::draw(const CommandBuffer& command_buffer,
                       const DynamicVertexBuffer& vertex_buffer,
                       uint32_t vertex_count,
                       const IndexBuffer& index_buffer)
{
    bind(command_buffer, vertex_buffer);
    bind(command_buffer, index_buffer);
    vkCmdDrawIndexed(command_buffer, vertex_count, 1, 0, 0, 0);
}

void rainbow::vk::draw(const CommandBuffer& command_buffer,
                       const VertexBuffer& vertex_buffer,
                       uint32_t vertex_count,
                       const IndexBuffer& index_buffer)
{
    bind(command_buffer, vertex_buffer);
    bind(command_buffer, index_buffer);
    vkCmdDrawIndexed(command_buffer, vertex_count, 1, 0, 0, 0);
}

auto rainbow::vk::make_image_view(VkDevice device,
                                  VkImage image,
                                  VkFormat format) -> VkImageView
{
    VkImageView view;
    const VkImageViewCreateInfo info{
        /* sType */ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        /* pNext */ nullptr,
        /* flags */ 0,
        /* image */ image,
        /* viewType */ VK_IMAGE_VIEW_TYPE_2D,
        /* format */ format,
        /* components */
        {
            /* r */ VK_COMPONENT_SWIZZLE_IDENTITY,
            /* g */ VK_COMPONENT_SWIZZLE_IDENTITY,
            /* b */ VK_COMPONENT_SWIZZLE_IDENTITY,
            /* a */ VK_COMPONENT_SWIZZLE_IDENTITY,
        },
        /* subresourceRange */
        {
            /* aspectMask */ VK_IMAGE_ASPECT_COLOR_BIT,
            /* baseMipLevel */ 0,
            /* levelCount */ 1,
            /* baseArrayLayer */ 0,
            /* layerCount */ 1,
        },
    };
    vkCreateImageView(device, &info, nullptr, &view);
    return view;
}

auto rainbow::vk::make_shader_module(const LogicalDevice& device,
                                     ArraySpan<uint8_t> code) -> VkShaderModule
{
    VkShaderModule module;

    const VkShaderModuleCreateInfo module_info{
        /* sType */ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        /* pNext */ nullptr,
        /* flags */ 0,
        /* codeSize */ code.size(),
        /* pCode */ reinterpret_cast<uint32_t*>(code.data()),
    };

    if (VkResult result =
            vkCreateShaderModule(device, &module_info, nullptr, &module))
    {
        R_ABORT("Vulkan: Failed to create shader module (%s)",  //
                to_string(result));
    }

    return module;
}

auto rainbow::vk::make_shader_stage_info(ShaderStageFlags stage,
                                         VkShaderModule module,
                                         const char* name)
    -> VkPipelineShaderStageCreateInfo
{
    return {
        /* sType */ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        /* pNext */ nullptr,
        /* flags */ 0,
        /* stage */ static_cast<VkShaderStageFlagBits>(stage),
        /* module */ module,
        /* pName */ name,
        /* pSpecializationInfo */ nullptr,
    };
}

void rainbow::vk::update_descriptor(const CommandBuffer& command_buffer,
                                    const ProjectionMatrix& matrix)
{
    const VkDescriptorBufferInfo buffer_info{
        /* buffer */ matrix,
        /* offset */ matrix.aligned_size() * command_buffer.current_index(),
        /* range */ matrix.size(),
    };
    const VkWriteDescriptorSet descriptor{
        /* sType */ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        /* pNext */ nullptr,
        /* dstSet */ command_buffer.descriptor_set(),
        /* dstBinding */ 0,
        /* dstArrayElement */ 0,
        /* descriptorCount */ 1,
        /* descriptorType */ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        /* pImageInfo */ nullptr,
        /* pBufferInfo */ &buffer_info,
        /* pTexelBufferView */ nullptr,
    };
    vkUpdateDescriptorSets(
        command_buffer.pipeline().device(), 1, &descriptor, 0, nullptr);
}

void rainbow::vk::update_descriptor(const CommandBuffer& command_buffer,
                                    const Texture& texture,
                                    uint32_t binding)
{
    const auto image_info = texture.descriptor();
    if (image_info.imageView == command_buffer.bindings().texture)
        return;

    command_buffer.bindings().texture = image_info.imageView;

    const VkWriteDescriptorSet descriptor{
        /* sType */ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        /* pNext */ nullptr,
        /* dstSet */ command_buffer.descriptor_set(),
        /* dstBinding */ binding,
        /* dstArrayElement */ 0,
        /* descriptorCount */ 1,
        /* descriptorType */ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        /* pImageInfo */ &image_info,
        /* pBufferInfo */ nullptr,
        /* pTexelBufferView */ nullptr,
    };
    vkUpdateDescriptorSets(
        command_buffer.pipeline().device(), 1, &descriptor, 0, nullptr);
}
