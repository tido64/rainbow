// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/TextureAtlas.h"

#include "FileSystem/File.h"
#include "Graphics/Image.h"
#include "Graphics/TextureManager.h"

#define kInvalidColorDepth "Invalid colour depth"

using rainbow::czstring;
using rainbow::Data;
using rainbow::Image;
using rainbow::TextureAtlas;
using rainbow::graphics::Texture;
using rainbow::graphics::TextureManager;

TextureAtlas::TextureAtlas(czstring path, float scale)
{
    texture_ = TextureManager::Get()->create(
        path,
        [this, path, scale](TextureManager& txr_manager, const Texture& txr) {
            load(txr_manager, txr, File::read(path, FileType::Asset), scale);
        });
}

TextureAtlas::TextureAtlas(czstring id, const Data& data, float scale)
{
    texture_ = TextureManager::Get()->create(
        id,
        [this, &data, scale](TextureManager& txr_manager, const Texture& txr) {
            load(txr_manager, txr, data, scale);
        });
}

auto TextureAtlas::add_region(int x, int y, int width, int height) -> uint32_t
{
    const auto txr_width = static_cast<float>(texture_.width());
    const auto txr_height = static_cast<float>(texture_.height());

    R_ASSERT(x >= 0 && (x + width) <= txr_width && y >= 0 &&
                 (y + height) <= txr_height,
             "Invalid dimensions");

    const Vec2f v0(x / txr_width, y / txr_height);
    const Vec2f v1((x + width) / txr_width, (y + height) / txr_height);
    const auto i = static_cast<uint32_t>(regions_.size());
    regions_.emplace_back(v0, v1);
    regions_[i].atlas = texture_;
    return i;
}

void TextureAtlas::set_regions(const ArrayView<int>& rectangles)
{
    R_ASSERT(rectangles.size() % 4 == 0,
             "Rectangle data size must be a multiple of 4");

    regions_.clear();
    regions_.reserve(rectangles.size() / 4);
    for (size_t i = 0; i < rectangles.size(); i += 4)
    {
        add_region(rectangles[i],
                   rectangles[i + 1],
                   rectangles[i + 2],
                   rectangles[i + 3]);
    }
}

void TextureAtlas::load(TextureManager& texture_manager,
                        const Texture& texture,
                        const Data& data,
                        float scale)
{
    R_ASSERT(data, "Failed to load texture");

    const Image& image = Image::decode(data, scale);
    if (image.data == nullptr)
        return;

    switch (image.format)
    {
#ifdef GL_OES_compressed_ETC1_RGB8_texture
        case Image::Format::ETC1:
            texture_manager.upload_compressed(
                texture, GL_ETC1_RGB8_OES, image.width, image.height,
                image.size, image.data);
            break;
#endif  // ETC1
#ifdef GL_IMG_texture_compression_pvrtc
        case Image::Format::PVRTC: {
            R_ASSERT(image.depth == 2 || image.depth == 4, kInvalidColorDepth);
            R_ASSERT(image.channels == 3 || image.channels == 4,
                     "Invalid number of colour channels");
            GLint internal = 0;
            if (image.channels == 3)
            {
                internal = (image.depth == 2
                    ? GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG
                    : GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG);
            }
            else
            {
                internal = (image.depth == 2
                    ? GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG
                    : GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG);
            }
            texture_manager.upload_compressed(
                texture, internal, image.width, image.height, image.size,
                image.data);
            break;
        }
#endif  // PVRTC
#ifdef GL_EXT_texture_compression_s3tc
        case Image::Format::BC1:
        case Image::Format::BC2:
        case Image::Format::BC3:
            texture_manager.upload_compressed(
                texture,
                image.channels,
                image.width,
                image.height,
                static_cast<uint32_t>(image.size),
                image.data);
            break;
#endif  // DDS
        default: {
            GLint format = 0;
            GLint internal = 0;
            switch (image.channels)
            {
                case 1:
                    R_ASSERT(image.depth == 8, kInvalidColorDepth);
                    format = GL_LUMINANCE;
                    internal = GL_LUMINANCE;
                    break;
                case 2:
                    R_ASSERT(image.depth == 16, kInvalidColorDepth);
                    format = GL_LUMINANCE_ALPHA;
                    internal = GL_LUMINANCE_ALPHA;
                    break;
                case 3:
                    R_ASSERT(image.depth == 16 || image.depth == 24,
                             kInvalidColorDepth);
                    format = GL_RGB;
                    internal = (image.depth == 16 ? GL_RGBA4 : GL_RGBA8);
                    break;
                case 4:
                    R_ASSERT(image.depth == 16 || image.depth == 32,
                             kInvalidColorDepth);
                    format = GL_RGBA;
                    internal = (image.depth == 16 ? GL_RGBA4 : GL_RGBA8);
                    break;
            }
            texture_manager.upload(
                texture, internal, image.width, image.height, format,
                image.data);
            break;
        }
    }
}

#include "Graphics/TextureManager.h"
#include "Graphics/Vulkan.h"

using rainbow::vk::CommandBuffer;

auto rainbow::v2::TextureAtlas::add(Rect region) -> uint32_t
{
    const auto index = regions_.size();
    regions_.push_back(region);
    return index;
}

void rainbow::v2::TextureAtlas::set(ArrayView<Rect> rects)
{
    regions_.clear();
    regions_.reserve(rects.size());
    std::copy(rects.begin(), rects.end(), std::back_inserter(regions_));
}

void rainbow::vk::update_descriptor(const CommandBuffer& command_buffer,
                                    const rainbow::v2::TextureAtlas& atlas,
                                    uint32_t binding)
{
    auto texture = atlas.texture_manager().try_get(atlas.path());
    if (!texture)
    {
        R_ABORT("'%s' has not been initialized or was already released",
                atlas.path().data());
        return;
    }

    update_descriptor(command_buffer, *texture, binding);
}
