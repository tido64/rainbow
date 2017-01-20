// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/TextureAtlas.h"

#include "FileSystem/FileSystem.h"
#include "Graphics/Image.h"
#include "Graphics/TextureManager.h"

#define kInvalidColorDepth "Invalid colour depth"

using rainbow::DataMap;
using rainbow::Image;
using rainbow::TextureAtlas;
using rainbow::filesystem::Path;
using rainbow::graphics::Texture;
using rainbow::graphics::TextureManager;

TextureAtlas::TextureAtlas(const Path& path, float scale)
{
    texture_ = TextureManager::Get()->create(
        path.u8string(),
        [this, &path, scale](
            TextureManager& texture_manager, const Texture& texture)
        {
            load(texture_manager, texture, DataMap{path}, scale);
        });
}

TextureAtlas::TextureAtlas(const char* id, const DataMap& data, float scale)
{
    texture_ = TextureManager::Get()->create(
        id,
        [this, &data, scale](
            TextureManager& texture_manager, const Texture& texture)
        {
            load(texture_manager, texture, data, scale);
        });
}

auto TextureAtlas::add_region(int x, int y, int w, int h) -> uint32_t
{
    const float width = static_cast<float>(texture_.width());
    const float height = static_cast<float>(texture_.height());

    R_ASSERT(x >= 0 && (x + w) <= width && y >= 0 && (y + h) <= height,
             "Invalid dimensions");

    const Vec2f v0(x / width, y / height);
    const Vec2f v1((x + w) / width, (y + h) / height);
    const auto i = static_cast<uint32_t>(regions_.size());
    regions_.emplace_back(v0, v1);
    regions_[i].atlas = texture_;
    return i;
}

void TextureAtlas::set_regions(const ArrayView<int>& rects)
{
    R_ASSERT(rects.size() % 4 == 0,
             "Rectangle data size must be a multiple of 4");

    regions_.clear();
    regions_.reserve(rects.size() / 4);
    for (size_t i = 0; i < rects.size(); i += 4)
        add_region(rects[i], rects[i + 1], rects[i + 2], rects[i + 3]);
}

void TextureAtlas::load(TextureManager& texture_manager,
                        const Texture& texture,
                        const DataMap& data,
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
        case Image::Format::S3TC:
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
