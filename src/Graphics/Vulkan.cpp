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
#include "FileSystem/File.h"
#include "Graphics/Image.h"
#include "Graphics/SpriteVertex.h"
#include "Resources/Fixed2D.frag.spv.h"
#include "Resources/Fixed2D.vert.spv.h"
#include "ThirdParty/VulkanMemoryAllocator.h"

using namespace rainbow;
using namespace std::literals::string_view_literals;

namespace
{
    struct DeviceProperties
    {
        VkPhysicalDevice physical_device = VK_NULL_HANDLE;
        uint32_t score = 0;
        uint32_t memory_type_index = -1;
        VkDeviceSize min_uniform_buffer_offset_alignment = 0;
    };

    struct PipelineCacheHeader
    {
        uint32_t length;
        uint32_t version;
        uint32_t vendor_id;
        uint32_t device_id;
        uint8_t uuid[VK_UUID_SIZE];
    };

    constexpr bool kEnablePipelineCache = false;
    constexpr const char kPipelineCacheFile[] = "rainbow.pipeline";

    auto begin_frame(vk::FrameInfo& info) -> vk::FrameInfo&
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

    void get_extension_properties(VkPhysicalDevice device,
                                  std::vector<VkExtensionProperties>& out)
    {
        uint32_t extension_count;
        vkEnumerateDeviceExtensionProperties(
            device, nullptr, &extension_count, nullptr);
        out.resize(extension_count);

        vkEnumerateDeviceExtensionProperties(
            device, nullptr, &extension_count, out.data());
    }

    auto has_swapchain_extension(VkPhysicalDevice device,
                                 std::vector<VkExtensionProperties>& out)
    {
        get_extension_properties(device, std::ref(out));
        return std::any_of(std::begin(out), std::end(out), [](auto&& props) {
            return props.extensionName == "VK_KHR_swapchain"sv;
        });
    }

    auto load_pipeline_cache(VkPhysicalDevice physical_device, VkDevice device)
    {
        if constexpr (!kEnablePipelineCache)
            return static_cast<VkPipelineCache>(VK_NULL_HANDLE);

        VkPipelineCache pipeline_cache = VK_NULL_HANDLE;

        auto data = File::read(kPipelineCacheFile, FileType::UserFile);
        auto initial_data_size = data.size();
        auto header = data.as<PipelineCacheHeader*>();
        if (header->length == sizeof(*header) &&
            header->version == VK_PIPELINE_CACHE_HEADER_VERSION_ONE)
        {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(physical_device, &properties);
            if (header->vendor_id != properties.vendorID ||
                header->device_id != properties.deviceID ||
                !std::equal(std::begin(header->uuid),
                            std::end(header->uuid),
                            std::begin(properties.pipelineCacheUUID)))
            {
                initial_data_size = 0;
            }
        }
        else
        {
            initial_data_size = 0;
        }

        const VkPipelineCacheCreateInfo info{
            /* sType */ VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
            /* pNext */ nullptr,
            /* flags */ 0,
            /* initialDataSize */ initial_data_size,
            /* pInitialData */ data.bytes(),
        };
        if (auto result =
                vkCreatePipelineCache(device, &info, nullptr, &pipeline_cache))
        {
            LOGE("Vulkan: Failed to create pipeline cache: %s",
                 vk::to_string(result));
        }
        else if (initial_data_size > 0)
        {
            LOGI("Vulkan: Successfully loaded pipeline cache");
        }
        return pipeline_cache;
    }

    auto make_buffer(VmaAllocator allocator,
                     uint32_t memory_type_index,
                     size_t size,
                     vk::BufferUsageFlags usage)
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
            /* memoryTypeBits */ (1U << memory_type_index),
            /* pool */ VK_NULL_HANDLE,
            /* pUserData */ nullptr,
        };

        VkBuffer buffer = VK_NULL_HANDLE;
        VmaAllocation allocation = VK_NULL_HANDLE;
        if (auto result = vmaCreateBuffer(allocator,
                                          &info,
                                          &allocation_info,
                                          &buffer,
                                          &allocation,
                                          nullptr))
        {
            R_ABORT("Vulkan: Failed to create buffer: %s",  //
                    vk::to_string(result));
        }

        return std::make_tuple(buffer, allocation);
    }

    auto make_default_descriptor_pool(VkDevice device, uint32_t max_sets)
    {
        VkDescriptorPool pool;

        const auto pool_sizes =
            make_array(VkDescriptorPoolSize{
                           /* type */ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                           /* descriptorCount */ max_sets,
                       },
                       VkDescriptorPoolSize{
                           /* type */ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                           /* descriptorCount */ max_sets,
                       });

        const VkDescriptorPoolCreateInfo pool_info{
            /* sType */ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            /* pNext */ nullptr,
            /* flags */ 0,
            /* maxSets */ max_sets,
            /* poolSizeCount */
            narrow_cast<uint32_t>(pool_sizes.size()),
            /* pPoolSizes */ pool_sizes.data(),
        };

        if (auto result =
                vkCreateDescriptorPool(device, &pool_info, nullptr, &pool))
        {
            R_ABORT("Vulkan: Failed to create descriptor pool (%s)",
                    vk::to_string(result));
        }

        return pool;
    }

    auto make_default_descriptor_set_layout(VkDevice device)
    {
        VkDescriptorSetLayout layout;

        constexpr auto bindings = make_array(
            VkDescriptorSetLayoutBinding{
                /* binding */ 0,
                /* descriptorType */ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                /* descriptorCount */ 1,
                /* stageFlags */ VK_SHADER_STAGE_VERTEX_BIT,
                /* pImmutableSamplers */ nullptr,
            },
            VkDescriptorSetLayoutBinding{
                /* binding */ 1,
                /* descriptorType */ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                /* descriptorCount */ 1,
                /* stageFlags */ VK_SHADER_STAGE_FRAGMENT_BIT,
                /* pImmutableSamplers */ nullptr,
            });

        const VkDescriptorSetLayoutCreateInfo layout_info{
            /* sType */ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            /* pNext */ nullptr,
            /* flags */ 0,
            /* bindingCount */
            narrow_cast<uint32_t>(bindings.size()),
            /* pBindings */ bindings.data(),
        };

        if (auto result = vkCreateDescriptorSetLayout(
                device, &layout_info, nullptr, &layout))
        {
            R_ABORT("Vulkan: Failed to create descriptor set layout (%s)",
                    vk::to_string(result));
        }

        return layout;
    }

    template <typename T, size_t N>
    auto make_shader_module(VkDevice device, const std::array<T, N>& code)
    {
        return vk::make_shader_module(
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
        for (auto i = 0U; i < mem_props.memoryTypeCount; ++i)
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
            [device, surface, i = 0U](auto&& family) mutable {
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
        constexpr auto kSurfaceExtensions = make_array(
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
            "VK_KHR_xlib_surface"sv);

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

    void store_pipeline_cache(VkDevice device, VkPipelineCache pipeline_cache)
    {
        if constexpr (!kEnablePipelineCache)
            return;

        if (pipeline_cache == VK_NULL_HANDLE)
            return;

        size_t size = 0;
        if (auto result =
                vkGetPipelineCacheData(device, pipeline_cache, &size, nullptr))
        {
            LOGE("Vulkan: Failed to get pipeline cache data size: %s",
                 vk::to_string(result));
            return;
        }

        auto data = std::make_unique<uint8_t[]>(size);
        if (auto result = vkGetPipelineCacheData(
                device, pipeline_cache, &size, data.get()))
        {
            LOGE("Vulkan: Failed to get pipeline cache data: %s",
                 vk::to_string(result));
            return;
        }

        if (WriteableFile::write(
                kPipelineCacheFile,
                Data{data.get(), size, Data::Ownership::Reference}) != size)
        {
            LOGE("Vulkan: Failed to write pipeline cache data");
            return;
        }

        LOGI("Vulkan: Successfully stored pipeline cache");
    }

    auto uniform_buffer_aligned_size(const vk::Context& context, size_t size)
    {
        const auto alignment = context.min_uniform_buffer_offset_alignment();
        return truncate<VkDeviceSize>(
            std::ceil(narrow_cast<float>(size) / alignment) * alignment);
    }

    void wait_and_reset(VkDevice device, VkFence fence)
    {
        vkWaitForFences(
            device, 1, &fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
        vkResetFences(device, 1, &fence);
    }

#ifndef NDEBUG
    constexpr auto kValidationLayers =
        make_array("VK_LAYER_LUNARG_standard_validation"sv);

    void list_devices(ArraySpan<VkPhysicalDevice> devices,
                      VkPhysicalDevice pick)
    {
        LOGI("Detected GPUs:");
        std::for_each_n(devices.data(), devices.size(), [pick](auto&& device) {
            VkPhysicalDeviceProperties props{};
            vkGetPhysicalDeviceProperties(device, &props);
            LOGI("  %s %s", device == pick ? "►" : " ", props.deviceName);
            LOGI("      Type: %s", [](VkPhysicalDeviceType device_type) {
                switch (device_type)
                {
                    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                        return "Other";
                    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                        return "Integrated";
                    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                        return "Discrete";
                    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                        return "Virtual";
                    case VK_PHYSICAL_DEVICE_TYPE_CPU:
                        return "CPU";
                    default:
                        return "Unknown";
                }
            }(props.deviceType));

            VkPhysicalDeviceMemoryProperties mem_props{};
            vkGetPhysicalDeviceMemoryProperties(device, &mem_props);
            const auto type = mem_props.memoryTypes[0];
            const auto heap = mem_props.memoryHeaps[type.heapIndex];
            LOGI("      Memory: %llu MiB", heap.size / (1024 * 1024));
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

vk::Context::Context(const VkApplicationInfo& app_info, PlatformWindow window)
{
    // VkInstance
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

        if (auto result = vkCreateInstance(&create_info, nullptr, &instance_))
        {
            LOGF("Failed to initialise Vulkan (%s)", to_string(result));
            return;
        }
    }

    // VkPhysicalDevice
    {
        auto [devices, device_count] =
            get_array<VkPhysicalDevice>(vkEnumeratePhysicalDevices, instance_);
        if (device_count == 0)
        {
            LOGF("Vulkan: No GPUs found");
            return;
        }

        auto best_device = std::reduce(  //
            devices.get(),
            devices.get() + device_count,
            DeviceProperties{},
            [available_extensions = std::vector<VkExtensionProperties>{}](
                auto best_device, auto device) mutable {
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
                if (!has_swapchain_extension(
                        device, std::ref(available_extensions)))
                {
                    return best_device;
                }

                // More memory is better
                auto [i, memory_score] = preferred_memory_type(device);
                score += memory_score;

                if (score > best_device.score)
                {
                    return DeviceProperties{
                        device,
                        score,
                        i,
                        props.limits.minUniformBufferOffsetAlignment,
                    };
                }

                return best_device;
            });

        min_uniform_buffer_offset_alignment_ =
            best_device.min_uniform_buffer_offset_alignment;
        memory_type_index_ = best_device.memory_type_index;
        physical_device_ = best_device.physical_device;

        IF_DEBUG(list_devices({devices.get(), device_count}, physical_device_));

        if (physical_device_ == VK_NULL_HANDLE)
        {
            LOGF("Vulkan: No eligible GPUs found");
            return;
        }
    }

    // VkSurfaceKHR
    if (auto result = create_surface(window))
    {
        LOGF("Vulkan: Failed to create surface (%s)", to_string(result));
        return;
    }

    // VkDevice
    {
        const auto index = preferred_queue_family(physical_device_, surface_);
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

        float priority = 1.0F;
        queue_create_info.pQueuePriorities = &priority;

        create_info.queueCreateInfoCount = 1;
        create_info.pQueueCreateInfos = &queue_create_info;

        constexpr auto extensions = make_array<const char* const>(
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
            VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
            VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,
            VK_EXT_MEMORY_BUDGET_EXTENSION_NAME);
        create_info.enabledExtensionCount = extensions.size();
        create_info.ppEnabledExtensionNames = extensions.data();

        // VkPhysicalDeviceFeatures features{};
        // create_info.pEnabledFeatures = &features;

        if (auto result = vkCreateDevice(
                physical_device_, &create_info, nullptr, &device_))
        {
            LOGF("Vulkan: Failed to create logical device (%s)",
                 to_string(result));
            return;
        }

        const VmaAllocatorCreateInfo allocator_info{
            /* flags */ VMA_ALLOCATOR_CREATE_EXTERNALLY_SYNCHRONIZED_BIT,
            /* physicalDevice */ physical_device_,
            /* device */ device_,
            /* preferredLargeHeapBlockSize */ 0,
            /* pAllocationCallbacks */ nullptr,
            /* pDeviceMemoryCallbacks */ nullptr,
            /* frameInUseCount */ 1,
            /* pHeapSizeLimit */ nullptr,
            /* pVulkanFunctions */ nullptr,
            /* pRecordSettings */ nullptr,
            /* instance */ instance_,
            /* vulkanApiVersion */ app_info.apiVersion,
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
    }

    reset_swapchain();
}

vk::Context::~Context()
{
    clear_swapchain();

    vkDestroyCommandPool(device_, command_pool_, nullptr);
    vmaDestroyAllocator(allocator_);
    vkDestroyDevice(device_, nullptr);

    if (surface_ != VK_NULL_HANDLE)
        vkDestroySurfaceKHR(instance_, surface_, nullptr);

    vkDestroyInstance(instance_, nullptr);
}

void vk::Context::clear_swapchain()
{
    if (swapchain_ == VK_NULL_HANDLE)
        return;

    for (auto&& view : views_)
        vkDestroyImageView(device_, view, nullptr);

    vkDestroySwapchainKHR(device_, swapchain_, nullptr);
}

void vk::Context::reset_swapchain()
{
    clear_swapchain();

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physical_device_, surface_, &capabilities);

    surface_format_ = preferred_surface_format(physical_device_, surface_);

    const VkSwapchainCreateInfoKHR create_info{
        /* sType */ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        /* pNext */ nullptr,
        /* flags */ 0,
        /* surface */ surface_,
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

    if (auto result =
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
        [device = device_, format = surface_format()](auto&& image) {
            return make_image_view(device, image, format);
        });

    LOGI("Vulkan: Resolution: %ix%i",  //
         image_extent_.width,
         image_extent_.height);
}

vk::Pipeline::Pipeline(const Context& context, VkPipeline base_pipeline)
    : device_(context.device())
{
    if (device_ == VK_NULL_HANDLE)
        return;

    descriptor_set_layout_ = make_default_descriptor_set_layout(device_);
    descriptor_pool_ = make_default_descriptor_pool(
        device_, narrow_cast<uint32_t>(context.image_views().size()));

    pipeline_cache_ = load_pipeline_cache(context.physical_device(), device_);

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

    const auto shader_stages = make_array(
        make_shader_stage_info(ShaderStageFlags::Vertex, vertex_shader_),
        make_shader_stage_info(ShaderStageFlags::Fragment, fragment_shader_));

    pipeline_info.stageCount = shader_stages.size();
    pipeline_info.pStages = shader_stages.data();

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

    const auto vertex_input_attributes =
        make_array(VkVertexInputAttributeDescription{
                       /* location */ 0,
                       /* binding */ vertex_input_binding.binding,
                       /* format */ VK_FORMAT_R8G8B8A8_UINT,
                       /* offset */ offsetof(SpriteVertex, color),
                   },
                   VkVertexInputAttributeDescription{
                       /* location */ 1,
                       /* binding */ vertex_input_binding.binding,
                       /* format */ VK_FORMAT_R32G32_SFLOAT,
                       /* offset */ offsetof(SpriteVertex, texcoord),
                   },
                   VkVertexInputAttributeDescription{
                       /* location */ 2,
                       /* binding */ vertex_input_binding.binding,
                       /* format */ VK_FORMAT_R32G32_SFLOAT,
                       /* offset */ offsetof(SpriteVertex, position),
                   });

    vertex_input_info.vertexAttributeDescriptionCount =
        vertex_input_attributes.size();
    vertex_input_info.pVertexAttributeDescriptions =
        vertex_input_attributes.data();

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
        /* x */ 0.0F,
        /* y */ 0.0F,
        /* width */ narrow_cast<float>(context.image_extent().width),
        /* height */ narrow_cast<float>(context.image_extent().height),
        /* minDepth */ 0.0F,
        /* maxDepth */ 1.0F,
    };

    viewport_info.viewportCount = 1;
    viewport_info.pViewports = &viewport;

    const VkRect2D scissor{
        /* offset */ {0, 0},
        /* extent */ context.image_extent(),
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
        /* depthBiasConstantFactor */ 0.0F,
        /* depthBiasClamp */ 0.0F,
        /* depthBiasSlopeFactor */ 0.0F,
        /* lineWidth */ 1.0F,
    };

    pipeline_info.pRasterizationState = &rasterizer_info;

    // Multisample

    constexpr VkPipelineMultisampleStateCreateInfo multisample_info{
        /* sType */ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        /* pNext */ nullptr,
        /* flags */ 0,
        /* rasterizationSamples */ VK_SAMPLE_COUNT_1_BIT,
        /* sampleShadingEnable */ VK_FALSE,
        /* minSampleShading */ 0.0F,
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
        /* blendConstants */ {1.0F, 1.0F, 1.0F, 1.0F},
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
        /* format */ context.surface_format(),
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

    if (auto result = vkCreateGraphicsPipelines(
            device_, pipeline_cache_, 1, &pipeline_info, nullptr, &pipeline_))
    {
        LOGF("Vulkan: Failed to create graphics pipeline (%s)",
             to_string(result));
    }
}

vk::Pipeline::~Pipeline()
{
    if (device_ == VK_NULL_HANDLE)
        return;

    store_pipeline_cache(device_, pipeline_cache_);

    vkDestroyPipelineCache(device_, pipeline_cache_, nullptr);
    vkDestroyPipeline(device_, pipeline_, nullptr);
    vkDestroyRenderPass(device_, render_pass_, nullptr);
    vkDestroyPipelineLayout(device_, layout_, nullptr);
    vkDestroyShaderModule(device_, fragment_shader_, nullptr);
    vkDestroyShaderModule(device_, vertex_shader_, nullptr);
    vkDestroyDescriptorPool(device_, descriptor_pool_, nullptr);
    vkDestroyDescriptorSetLayout(device_, descriptor_set_layout_, nullptr);
}

vk::Buffer::Buffer(Buffer&& buffer) noexcept
    : buffer_(buffer.buffer_), allocation_(buffer.allocation_),
      allocator_(buffer.allocator_)
{
    buffer.buffer_ = VK_NULL_HANDLE;
    buffer.allocation_ = VK_NULL_HANDLE;
}

vk::Buffer::Buffer(const Context& context, size_t size, BufferUsageFlags usage)
    : allocator_(context.allocator())
{
    std::tie(buffer_, allocation_) = make_buffer(
        context.allocator(), context.memory_type_index(), size, usage);
}

vk::Buffer::~Buffer()
{
    vmaDestroyBuffer(allocator_, buffer_, allocation_);
}

void vk::Buffer::copy(const void* src, size_t size, size_t offset)
{
    uint8_t* dst;
    vmaMapMemory(allocator_, allocation_, reinterpret_cast<void**>(&dst));
    memcpy(dst + offset, src, size);
    vmaUnmapMemory(allocator_, allocation_);
}

void vk::CommandBuffer::run_once(const Context& context,
                                 Callback<void(VkCommandBuffer)> callback)
{
    const VkCommandBufferAllocateInfo info{
        /* sType */ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        /* pNext */ nullptr,
        /* commandPool */ context.command_pool(),
        /* level */ VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        /* commandBufferCount */ 1,
    };

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers(context.device(), &info, &command_buffer);

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

    auto queue = context.queue();
    vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(
        context.device(), context.command_pool(), 1, &command_buffer);
}

vk::CommandBuffer::CommandBuffer(const Context& context,
                                 const Pipeline& pipeline)
    : context_(context), pipeline_(pipeline)
{
    if (!context)
        return;

    auto buffer_count = narrow_cast<uint32_t>(context.image_views().size());
    frame_info_ = std::make_unique<FrameInfo[]>(buffer_count);

    auto device = VkDevice{context.device()};
    std::transform(  //
        std::begin(context.image_views()),
        std::end(context.image_views()),
        frame_info_.get(),
        [device,
         descriptor_pool = pipeline.descriptor_pool(),
         descriptor_set_layout = pipeline.descriptor_set_layout(),
         render_pass = pipeline.render_pass(),
         extent = context.image_extent()](auto&& image_view) {
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

    R_ASSERT(buffer_count <= 5, "Unexpected high number of image buffers");

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
    std::array<VkCommandBuffer, 5> command_buffers;

    const VkCommandBufferAllocateInfo buffer_alloc_info{
        /* sType */ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        /* pNext */ nullptr,
        /* commandPool */ context.command_pool(),
        /* level */ VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        /* commandBufferCount */ buffer_count,
    };

    vkAllocateCommandBuffers(
        device, &buffer_alloc_info, command_buffers.data());

    for (auto i = 0U; i < buffer_count; ++i)
        frame_info_[i].command_buffer = command_buffers[i];

    constexpr VkSemaphoreCreateInfo semaphore_info{
        VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        nullptr,
        0,
    };

    vkCreateSemaphore(device, &semaphore_info, nullptr, &image_available_);
    vkCreateSemaphore(device, &semaphore_info, nullptr, &render_finished_);
}

vk::CommandBuffer::~CommandBuffer()
{
    auto device = VkDevice{context_.device()};
    vkDeviceWaitIdle(device);

    vkDestroySemaphore(device, render_finished_, nullptr);
    vkDestroySemaphore(device, image_available_, nullptr);

    std::for_each_n(  //
        frame_info_.get(),
        context_.image_views().size(),
        [device,
         descriptor_pool = pipeline_.descriptor_pool(),
         command_pool = context_.command_pool()](auto&& info) {
            vkFreeDescriptorSets(
                device, descriptor_pool, 1, &info.descriptor_set);
            vkFreeCommandBuffers(device, command_pool, 1, &info.command_buffer);
            vkDestroyFramebuffer(device, info.framebuffer, nullptr);
            vkDestroyFence(device, info.fence, nullptr);
        });
}

void vk::CommandBuffer::begin()
{
    vkAcquireNextImageKHR(  //
        context_.device(),
        context_.swapchain(),
        std::numeric_limits<uint64_t>::max(),
        image_available_,
        VK_NULL_HANDLE,
        &current_);

    const auto& info = begin_frame(current());

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
        {/* offset */ {0, 0}, /* extent */ context_.image_extent()},
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

void vk::CommandBuffer::end()
{
    const auto& info = current();

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

    auto queue = context_.queue();
    vkQueueSubmit(queue, 1, &submit_info, info.fence);

    wait_and_reset(context_.device(), info.fence);
    vkResetCommandBuffer(info.command_buffer, 0);

    auto swapchain = context_.swapchain();
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

vk::DynamicBuffer::~DynamicBuffer()
{
    vmaDestroyBuffer(allocator_, buffer_, allocation_);
}

void vk::DynamicBuffer::copy(const void* src, size_t size)
{
    if (size > capacity_)
    {
        capacity_ = size;
        vmaDestroyBuffer(allocator_, buffer_, allocation_);
        std::tie(buffer_, allocation_) =
            make_buffer(allocator_, memory_type_index_, size, usage_);
    }

    void* dst;
    vmaMapMemory(allocator_, allocation_, &dst);
    memcpy(dst, src, size);
    vmaUnmapMemory(allocator_, allocation_);
}

vk::IndexBuffer::IndexBuffer(const Context& context)
    : Buffer(context,
             IndexBuffer::kIndexBufferSize * sizeof(uint16_t),
             BufferUsageFlags::TransferDst | BufferUsageFlags::IndexBuffer)
{
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

vk::ProjectionMatrix::ProjectionMatrix(const Context& context)
    : Buffer(context,
             uniform_buffer_aligned_size(context, size()) *
                 context.image_views().size(),
             BufferUsageFlags::TransferDst | BufferUsageFlags::UniformBuffer),
      aligned_size_(uniform_buffer_aligned_size(context, size()))
{
    const auto extent = context.image_extent();
    const VkViewport viewport{
        /* x */ 0.0F,
        /* y */ 0.0F,
        /* width */ static_cast<float>(extent.width),
        /* height */ static_cast<float>(extent.height),
        /* minDepth */ 0.0F,
        /* maxDepth */ 0.0F,
    };
    for (size_t i = 0; i < context.image_views().size(); ++i)
        set(viewport, i);
}

void vk::ProjectionMatrix::set(const VkViewport& viewport, uint32_t index)
{
    const float norm_width = 2.0F / (viewport.width - viewport.x);

#ifdef RAINBOW_OS_MACOS
    // The coordinate system is flipped in shaders:
    // https://github.com/KhronosGroup/MoltenVK/blob/v1.0.36/MoltenVK/MoltenVK/Utility/MVKEnvironment.h#L54
    constexpr float y_axis = 1.0F;
#else
    constexpr float y_axis = -1.0F;
#endif
    const float norm_height = y_axis * -2.0F / (viewport.height - viewport.y);

    // The orthographic projection matrix is defined as:
    //
    //   | 2 / (r - l)       0             0       -(r + l) / (r - l) |
    //   |      0       2 / (t - b)        0       -(t + b) / (t - b) |
    //   |      0            0       -2 / (f - n)  -(f + n) / (f - n) |
    //   |      0            0             0                1         |
    //
    // Where _b_ = bottom, _f_ = far, _l_ = left, _n_ = near, _r_ = right,
    // _t_ = top, and near = -1.0 and far = 1.0.
    // The matrix is stored in column-major order.
    // clang-format off
    std::array<float, 16> projection{
        norm_width,         0.0F,  0.0F,  0.0F,
              0.0F,  norm_height,  0.0F,  0.0F,
              0.0F,         0.0F, -1.0F,  0.0F,
             -1.0F,       y_axis,  0.0F,  1.0F,
    };
    // clang-format on

    Buffer::copy(projection.data(),
                 projection.size() * sizeof(projection[0]),
                 aligned_size_ * index);
}

void vk::bind(const CommandBuffer& command_buffer,
              const DynamicVertexBuffer& vertex_buffer)
{
    constexpr VkDeviceSize offset = 0;
    VkBuffer buffer = vertex_buffer;
    vkCmdBindVertexBuffers(command_buffer, 0, 1, &buffer, &offset);
}

void vk::bind(const CommandBuffer& command_buffer,
              const IndexBuffer& index_buffer)
{
    if (index_buffer == command_buffer.bindings().index_buffer)
        return;

    command_buffer.bindings().index_buffer = index_buffer;

    vkCmdBindIndexBuffer(command_buffer, index_buffer, 0, VK_INDEX_TYPE_UINT16);
}

void vk::bind(const CommandBuffer& command_buffer,
              const VertexBuffer& vertex_buffer)
{
    constexpr VkDeviceSize offset = 0;
    VkBuffer buffer = vertex_buffer;
    vkCmdBindVertexBuffers(command_buffer, 0, 1, &buffer, &offset);
}

void vk::draw(const CommandBuffer& command_buffer,
              const VertexBuffer& vertex_buffer,
              uint32_t vertex_count)
{
    bind(command_buffer, vertex_buffer);
    vkCmdDraw(command_buffer, vertex_count, 1, 0, 0);
}

void vk::draw(const CommandBuffer& command_buffer,
              const DynamicVertexBuffer& vertex_buffer,
              uint32_t vertex_count,
              const IndexBuffer& index_buffer)
{
    bind(command_buffer, vertex_buffer);
    bind(command_buffer, index_buffer);
    vkCmdDrawIndexed(command_buffer, vertex_count, 1, 0, 0, 0);
}

void vk::draw(const CommandBuffer& command_buffer,
              const VertexBuffer& vertex_buffer,
              uint32_t vertex_count,
              const IndexBuffer& index_buffer)
{
    bind(command_buffer, vertex_buffer);
    bind(command_buffer, index_buffer);
    vkCmdDrawIndexed(command_buffer, vertex_count, 1, 0, 0, 0);
}

auto vk::make_image_view(VkDevice device, VkImage image, VkFormat format)
    -> VkImageView
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

auto vk::make_shader_module(VkDevice device, ArraySpan<uint8_t> code)
    -> VkShaderModule
{
    VkShaderModule module;

    const VkShaderModuleCreateInfo module_info{
        /* sType */ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        /* pNext */ nullptr,
        /* flags */ 0,
        /* codeSize */ code.size(),
        /* pCode */ reinterpret_cast<uint32_t*>(code.data()),
    };

    if (auto result =
            vkCreateShaderModule(device, &module_info, nullptr, &module))
    {
        R_ABORT("Vulkan: Failed to create shader module (%s)",  //
                to_string(result));
    }

    return module;
}

auto vk::make_shader_stage_info(ShaderStageFlags stage,
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

auto vk::to_string(VkResult result) -> czstring
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

void vk::update_descriptor(const CommandBuffer& command_buffer,
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

void vk::update_descriptor(const CommandBuffer& command_buffer,
                           const VkDescriptorImageInfo& image_info,
                           uint32_t binding)
{
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
