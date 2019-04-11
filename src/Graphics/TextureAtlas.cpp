// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/TextureAtlas.h"

#include "FileSystem/File.h"
#include "Graphics/Image.h"
#include "Graphics/OpenGL.h"
#include "Graphics/TextureManager.h"

#if defined(GL_OES_compressed_ETC1_RGB8_texture) ||                            \
    defined(GL_IMG_texture_compression_pvrtc) ||                               \
    defined(GL_EXT_texture_compression_s3tc)
#    define RAINBOW_HAS_COMPRESSED_TEXTURES
#endif

#define kInvalidColorDepth "Invalid colour depth"

using rainbow::czstring;
using rainbow::Data;
using rainbow::Image;
using rainbow::TextureAtlas;
using rainbow::graphics::Texture;
using rainbow::graphics::TextureManager;

namespace
{
#ifdef RAINBOW_HAS_COMPRESSED_TEXTURES
    constexpr auto compression_format(const Image& image)
    {
        switch (image.format)
        {
#    ifdef GL_OES_compressed_ETC1_RGB8_texture
            case Image::Format::ETC1:
                return GL_ETC1_RGB8_OES;
#    endif  // ETC1
#    ifdef GL_IMG_texture_compression_pvrtc
            case Image::Format::PVRTC:
                R_ASSERT(
                    image.depth == 2 || image.depth == 4, kInvalidColorDepth);
                R_ASSERT(image.channels == 3 || image.channels == 4,
                         "Invalid number of colour channels");
                if (image.channels == 3)
                {
                    return image.depth == 2
                               ? GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG
                               : GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
                }
                return image.depth == 2 ? GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG
                                        : GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
#    endif  // PVRTC
#    ifdef GL_EXT_texture_compression_s3tc
            case Image::Format::BC1:
                return image.channels == 4 ? GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
                                           : GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
            case Image::Format::BC2:
                return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            case Image::Format::BC3:
                return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
#    endif  // DDS
            default:
                R_ASSERT(false, "Unknown compressed texture format");
                return 0;
        }
    }
#endif  // RAINBOW_HAS_COMPRESSED_TEXTURES

    constexpr auto internal_format(const Image& image)
    {
        switch (image.channels)
        {
            case 1:
                R_ASSERT(image.depth == 8, kInvalidColorDepth);
                return std::make_tuple(GL_LUMINANCE, GL_LUMINANCE);
            case 2:
                R_ASSERT(image.depth == 16, kInvalidColorDepth);
                return std::make_tuple(GL_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA);
            case 3:
                R_ASSERT(image.depth == 16 || image.depth == 24,  //
                         kInvalidColorDepth);
                return std::make_tuple(
                    GL_RGB, image.depth == 16 ? GL_RGBA4 : GL_RGBA8);
            case 4:
                R_ASSERT(image.depth == 16 || image.depth == 32,  //
                         kInvalidColorDepth);
                return std::make_tuple(
                    GL_RGBA, image.depth == 16 ? GL_RGBA4 : GL_RGBA8);
            default:
                R_ASSERT(false, "Unknown image format");
                return std::make_tuple(GL_RGBA, GL_RGBA8);
        }
    }
}  // namespace

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
        add_region(  //
            rectangles[i],
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
#ifdef RAINBOW_HAS_COMPRESSED_TEXTURES
#    ifdef GL_EXT_texture_compression_s3tc
        case Image::Format::BC1:
        case Image::Format::BC2:
        case Image::Format::BC3:
#    endif
#    ifdef GL_OES_compressed_ETC1_RGB8_texture
        case Image::Format::ETC1:
#    endif
#    ifdef GL_IMG_texture_compression_pvrtc
        case Image::Format::PVRTC:
#    endif
            texture_manager.upload_compressed(  //
                texture,
                compression_format(image),
                image.width,
                image.height,
                static_cast<uint32_t>(image.size),
                image.data);
            break;
#endif  // RAINBOW_HAS_COMPRESSED_TEXTURES
        default:
        {
            const auto [format, internal] = internal_format(image);
            texture_manager.upload(  //
                texture,
                internal,
                image.width,
                image.height,
                format,
                image.data);
            break;
        }
    }
}
