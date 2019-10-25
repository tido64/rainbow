// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/TextureAllocator.gl.h"

#include <tuple>

#include "Common/Logging.h"
#include "Common/TypeCast.h"
#include "Graphics/Image.h"
#include "Graphics/OpenGL.h"
#include "Graphics/Renderer.h"

#ifndef GL_EXT_texture_compression_s3tc
#    define GL_COMPRESSED_RGB_S3TC_DXT1_EXT 0x83F0
#    define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#    define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#    define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
#endif
#ifndef GL_IMG_texture_compression_pvrtc
#    define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG 0x8C00
#    define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG 0x8C01
#    define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG 0x8C02
#    define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG 0x8C03
#endif
#ifndef GL_AMD_compressed_ATC_texture
#    define GL_ATC_RGB_AMD 0x8C92
#    define GL_ATC_RGBA_EXPLICIT_ALPHA_AMD 0x8C93
#endif
#ifndef GL_OES_compressed_ETC1_RGB8_texture
#    define GL_ETC1_RGB8_OES 0x8D64
#endif

#define kInvalidColorDepth "Invalid colour depth"

using rainbow::Image;
using rainbow::graphics::Filter;
using rainbow::graphics::TextureHandle;
using rainbow::graphics::gl::TextureAllocator;

namespace
{
    constexpr auto texture_filter(Filter filter) -> int
    {
        switch (filter)
        {
            case Filter::Nearest:
                return GL_NEAREST;

            case Filter::Linear:
                [[fallthrough]];
            case Filter::Cubic:
                return GL_LINEAR;
        }

        std::abort();
    }

    auto texture_format(const Image& image) -> std::tuple<GLenum, GLenum>
    {
        switch (image.format)
        {
            case Image::Format::Unknown:
                R_ASSERT(false, "Unknown image format");
                return std::make_tuple(GL_RGBA8, GL_RGBA);

            case Image::Format::ATITC:
                return std::make_tuple(image.channels == 3
                                           ? GL_ATC_RGB_AMD
                                           : GL_ATC_RGBA_EXPLICIT_ALPHA_AMD,
                                       GL_NONE);

            case Image::Format::BC1:
                return std::make_tuple(image.channels == 3
                                           ? GL_COMPRESSED_RGB_S3TC_DXT1_EXT
                                           : GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
                                       GL_NONE);

            case Image::Format::BC2:
                return std::make_tuple(
                    GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_NONE);

            case Image::Format::BC3:
                return std::make_tuple(
                    GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_NONE);

            case Image::Format::ETC1:
                return std::make_tuple(GL_ETC1_RGB8_OES, GL_NONE);

            case Image::Format::PVRTC:
                R_ASSERT(image.depth == 2 || image.depth == 4,  //
                         kInvalidColorDepth);
                R_ASSERT(image.channels == 3 || image.channels == 4,
                         "Invalid number of colour channels");
                return std::make_tuple(
                    image.channels == 3
                        ? image.depth == 2  //
                              ? GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG
                              : GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG
                        : image.depth == 2
                              ? GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG
                              : GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG,
                    GL_NONE);

            case Image::Format::RGBA:
                return std::make_tuple(GL_RGBA, GL_RGBA);

            case Image::Format::PNG:
                [[fallthrough]];
            case Image::Format::SVG:
                switch (image.channels)
                {
                    case 1:
                        R_ASSERT(image.depth == 8, kInvalidColorDepth);
                        return std::make_tuple(GL_LUMINANCE, GL_LUMINANCE);

                    case 2:
                        R_ASSERT(image.depth == 16, kInvalidColorDepth);
                        return std::make_tuple(
                            GL_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA);

                    case 3:
                        R_ASSERT(image.depth == 16 || image.depth == 24,  //
                                 kInvalidColorDepth);
                        return std::make_tuple(
                            image.depth == 16 ? GL_RGBA4 : GL_RGBA8, GL_RGB);

                    case 4:
                        R_ASSERT(image.depth == 16 || image.depth == 32,  //
                                 kInvalidColorDepth);
                        return std::make_tuple(
                            image.depth == 16 ? GL_RGBA4 : GL_RGBA8, GL_RGBA);

                    default:
                        R_ASSERT(false, "Unknown image format");
                        return std::make_tuple(GL_RGBA8, GL_RGBA);
                }
        }

        std::abort();
    }

    constexpr auto texture_id(const TextureHandle& handle)
    {
        return rainbow::narrow_cast<GLuint>(handle[0]);
    }

    void bind(const TextureHandle& handle, uint32_t unit)
    {
        static auto bound_unit = 0U;
        static auto bound_textures = std::array<GLuint, 32>{};

        if (unit != bound_unit)
        {
            bound_unit = unit;
            glActiveTexture(GL_TEXTURE0 + unit);
        }

        auto id = texture_id(handle);
        if (id != bound_textures[unit])
        {
            bound_textures[unit] = id;
            glBindTexture(GL_TEXTURE_2D, id);
        }
    }
}  // namespace

void TextureAllocator::construct(TextureHandle& handle,
                                 const Image& image,
                                 Filter mag_filter,
                                 Filter min_filter)
{
    GLuint name;
    glGenTextures(1, &name);
    handle[0] = name;

    update(handle, image, mag_filter, min_filter);
}

void TextureAllocator::destroy(TextureHandle& handle)
{
    const auto texture = texture_id(handle);
    glDeleteTextures(1, &texture);
}

auto TextureAllocator::max_size() const noexcept -> size_t
{
    return sizeof(GLuint);
}

void TextureAllocator::update(const TextureHandle& handle,
                              const Image& image,
                              Filter mag_filter,
                              Filter min_filter)
{
    ::bind(handle, 0);
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture_filter(min_filter));
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture_filter(mag_filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    auto [internal_format, format] = texture_format(image);
    switch (image.format)
    {
        case Image::Format::Unknown:
            break;

        case Image::Format::ATITC:
            [[fallthrough]];
        case Image::Format::BC1:
            [[fallthrough]];
        case Image::Format::BC2:
            [[fallthrough]];
        case Image::Format::BC3:
            [[fallthrough]];
        case Image::Format::ETC1:
            [[fallthrough]];
        case Image::Format::PVRTC:
            glCompressedTexImage2D(  //
                GL_TEXTURE_2D,
                0,
                internal_format,
                narrow_cast<GLsizei>(image.width),
                narrow_cast<GLsizei>(image.height),
                0,
                narrow_cast<GLsizei>(image.size),
                image.data);
            break;

        case Image::Format::PNG:
            [[fallthrough]];
        case Image::Format::RGBA:
            [[fallthrough]];
        case Image::Format::SVG:
            glTexImage2D(  //
                GL_TEXTURE_2D,
                0,
                internal_format,
                narrow_cast<GLsizei>(image.width),
                narrow_cast<GLsizei>(image.height),
                0,
                format,
                GL_UNSIGNED_BYTE,
                image.data);
            break;
    }

    R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to upload texture");
}

void rainbow::graphics::bind(const Context& ctx,
                             const Texture& texture,
                             uint32_t unit)
{
    auto texture_data = ctx.texture_provider.raw_get(texture);
    ::bind(texture_data.data, unit);
}
