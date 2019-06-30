// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/TextureAllocator.vk.h"

#include <tuple>

// clang-format off
#include "ThirdParty/DisableWarnings.h"
#include <vk_mem_alloc.h>
#include "ThirdParty/ReenableWarnings.h"
// clang-format on

#include "Common/Logging.h"
#include "Common/TypeCast.h"
#include "Graphics/Image.h"
#include "Graphics/Renderer.h"
#include "Graphics/Vulkan.h"

using rainbow::Image;
using rainbow::graphics::Filter;
using rainbow::graphics::TextureHandle;
using rainbow::graphics::vk::TextureAllocator;
using rainbow::vk::CommandBuffer;
using rainbow::vk::StagingBuffer;

namespace
{
    struct TextureData
    {
        VkSampler sampler = VK_NULL_HANDLE;
        VkImageView view = VK_NULL_HANDLE;
        VkImage image = VK_NULL_HANDLE;
        VmaAllocation allocation = VK_NULL_HANDLE;
    };

    constexpr auto image_filter(Filter filter) -> VkFilter
    {
        switch (filter)
        {
            case Filter::Nearest:
                return VK_FILTER_NEAREST;
            case Filter::Linear:
                return VK_FILTER_LINEAR;
            case Filter::Cubic:
                return VK_FILTER_CUBIC_IMG;
        }

        std::abort();
    }

    constexpr auto image_format(const Image& image)
    {
        switch (image.format)
        {
            case Image::Format::Unknown:  // NOLINT(bugprone-branch-clone)
                return VK_FORMAT_UNDEFINED;

            case Image::Format::ATITC:  // Adreno
                return VK_FORMAT_UNDEFINED;

            case Image::Format::BC1:  // DXT1
                switch (image.channels)
                {
                    case 3:
                        return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
                    case 4:
                        return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
                    default:
                        return VK_FORMAT_UNDEFINED;
                }

            case Image::Format::BC2:  // DXT3
                return VK_FORMAT_BC2_UNORM_BLOCK;

            case Image::Format::BC3:  // DXT5
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

            case Image::Format::RGBA:
                return VK_FORMAT_B8G8R8A8_UNORM;

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

    auto texture_data(const TextureHandle& handle)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        return reinterpret_cast<const TextureData&>(handle);
    }
}  // namespace

TextureAllocator::TextureAllocator(rainbow::vk::Context& context)
    : context_(context)
{
    constexpr VkSamplerCreateInfo sampler_info{
        /* sType */ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        /* pNext */ nullptr,
        /* flags */ 0,
        /* magFilter */ image_filter(Filter::Cubic),
        /* minFilter */ image_filter(Filter::Linear),
        /* mipmapMode */ VK_SAMPLER_MIPMAP_MODE_LINEAR,
        /* addressModeU */ VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
        /* addressModeV */ VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
        /* addressModeW */ VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
        /* mipLodBias */ 0.0F,
        /* anisotropyEnable */ VK_FALSE,
        /* maxAnisotropy */ 1.0F,
        /* compareEnable */ VK_FALSE,
        /* compareOp */ VK_COMPARE_OP_NEVER,
        /* minLod */ 0.0F,
        /* maxLod */ 0.0F,
        /* borderColor */ VK_BORDER_COLOR_INT_TRANSPARENT_BLACK,
        /* unnormalizedCoordinates */ VK_FALSE,
    };

    vkCreateSampler(
        context.device(), &sampler_info, nullptr, &default_sampler_);
}

TextureAllocator::~TextureAllocator()
{
    vkDestroySampler(context_.device(), default_sampler_, nullptr);
}

void TextureAllocator::construct(TextureHandle& handle,
                                 const Image& image,
                                 Filter mag_filter,
                                 Filter min_filter)
{
    auto texture = texture_data(handle);

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
    if (auto result = vmaCreateImage(context_.allocator(),
                                     &info,
                                     &allocation_info,
                                     &texture.image,
                                     &texture.allocation,
                                     nullptr))
    {
        R_ABORT("Vulkan: Failed to create image (%s)",
                rainbow::vk::to_string(result));
    }

    texture.view = rainbow::vk::make_image_view(
        context_.device(), texture.image, info.format);

    update(handle, image, mag_filter, min_filter);
}

void TextureAllocator::destroy(TextureHandle& handle)
{
    auto texture = texture_data(handle);

    if (texture.sampler != default_sampler_)
        vkDestroySampler(context_.device(), texture.sampler, nullptr);

    vkDestroyImageView(context_.device(), texture.view, nullptr);
    vmaDestroyImage(context_.allocator(), texture.image, texture.allocation);

    texture.sampler = VK_NULL_HANDLE;
    texture.view = VK_NULL_HANDLE;
    texture.image = VK_NULL_HANDLE;
    texture.allocation = VK_NULL_HANDLE;
}

auto TextureAllocator::max_size() const noexcept -> size_t
{
    return sizeof(TextureData);
}

void TextureAllocator::update(const TextureHandle& handle,
                              const Image& image,
                              Filter mag_filter,
                              Filter min_filter)
{
    auto texture = texture_data(handle);

    if (mag_filter == Filter::Cubic && min_filter == Filter::Linear)
    {
        texture.sampler = default_sampler_;
    }
    else
    {
        if (texture.sampler != default_sampler_)
            vkDestroySampler(context_.device(), texture.sampler, nullptr);

        const VkSamplerCreateInfo sampler_info{
            /* sType */ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            /* pNext */ nullptr,
            /* flags */ 0,
            /* magFilter */ image_filter(mag_filter),
            /* minFilter */ image_filter(min_filter),
            /* mipmapMode */ VK_SAMPLER_MIPMAP_MODE_LINEAR,
            /* addressModeU */ VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
            /* addressModeV */ VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
            /* addressModeW */ VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
            /* mipLodBias */ 0.0F,
            /* anisotropyEnable */ VK_FALSE,
            /* maxAnisotropy */ 1.0F,
            /* compareEnable */ VK_FALSE,
            /* compareOp */ VK_COMPARE_OP_NEVER,
            /* minLod */ 0.0F,
            /* maxLod */ 0.0F,
            /* borderColor */ VK_BORDER_COLOR_INT_TRANSPARENT_BLACK,
            /* unnormalizedCoordinates */ VK_FALSE,
        };

        vkCreateSampler(
            context_.device(), &sampler_info, nullptr, &texture.sampler);
    }

    StagingBuffer staging_buffer(context_, image.size);
    staging_buffer.copy(
        ArraySpan<uint8_t>(const_cast<uint8_t*>(image.data), image.size));

    CommandBuffer::run_once(
        context_,
        [image = texture.image,
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

void rainbow::graphics::bind(const Context& ctx,
                             const Texture& texture,
                             uint32_t unit)
{
    auto raw_data = ctx.texture_provider.raw_get(texture);
    auto info = texture_data(raw_data.data);
    const VkDescriptorImageInfo image_info{
        /* sampler */ info.sampler,
        /* imageView */ info.view,
        /* imageLayout */ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };
    rainbow::vk::update_descriptor(ctx.command_buffer(), image_info, unit + 1);
}
