// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/TextureManager.h"

#include "Graphics/Renderer.h"

using rainbow::Texture;
using rainbow::TextureHandle;

namespace
{
#ifndef NDEBUG
	void assert_texture_size(unsigned int width, unsigned int height)
	{
		const unsigned int max_texture_size = Renderer::max_texture_size();
		R_ASSERT(width <= max_texture_size && height <= max_texture_size,
		         "Texture dimension exceeds max texture size supported by "
		         "hardware");
	}
#else
#	define assert_texture_size(...) static_cast<void>(0)
#endif

	template <typename Container, typename T, typename F>
	void perform_if(Container& container, const T& value, F&& action)
	{
		for (auto&& element : container)
		{
			if (element == value)
			{
				action(element);
				break;
			}
		}
	}
}

Texture::Texture(const Texture& texture) : name_(texture.name_)
{
	TextureManager::Get()->retain(*this);
}

Texture::~Texture()
{
	if (name_ == 0)
		return;

	TextureManager::Get()->release(*this);
}

void Texture::bind() const
{
	TextureManager::Get()->bind(name_);
}

void Texture::bind(unsigned int unit) const
{
	TextureManager::Get()->bind(name_, unit);
}

Texture& Texture::operator=(Texture&& texture)
{
	if (name_ > 0)
		TextureManager::Get()->release(*this);
	name_ = texture.name_;
	texture.name_ = 0;
	return *this;
}

void TextureManager::set_filter(int filter)
{
	R_ASSERT(filter == GL_NEAREST || filter == GL_LINEAR,
	         "Invalid texture filter function.");

	mag_filter_ = filter;
	min_filter_ = filter;
}

void TextureManager::bind(unsigned int name)
{
	if (name == active_[0])
		return;

	glBindTexture(GL_TEXTURE_2D, name);
	active_[0] = name;
}

void TextureManager::bind(unsigned int name, unsigned int unit)
{
	R_ASSERT(unit < kNumTextureUnits, "Invalid texture unit");

	if (unit >= kNumTextureUnits || name == active_[unit])
		return;

	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, name);
	glActiveTexture(GL_TEXTURE0);
	active_[unit] = name;
}

void TextureManager::trim()
{
	auto first = std::remove_if(
	    textures_.begin(),
	    textures_.end(),
	    [](const TextureHandle& texture) { return texture.use_count == 0; });
	auto end = textures_.end();
	if (first == end)
		return;

	for (auto i = first; i != end; ++i)
	{
		glDeleteTextures(1, &i->name);
#if RAINBOW_RECORD_VMEM_USAGE
		mem_used_ -= i->size;
#endif
	}

	textures_.erase(first, end);

#if RAINBOW_RECORD_VMEM_USAGE
	update_usage();
#endif
}

TextureManager::TextureManager()
    : mag_filter_(GL_LINEAR), min_filter_(GL_LINEAR)
#if RAINBOW_RECORD_VMEM_USAGE
    , mem_peak_(0.0), mem_used_(0.0)
#endif
{
	std::fill_n(active_, kNumTextureUnits, 0);
	make_global();
}

TextureManager::~TextureManager()
{
	for (const TextureHandle& texture : textures_)
		glDeleteTextures(1, &texture.name);
}

Texture TextureManager::create_texture(const char* id)
{
	GLuint name;
	glGenTextures(1, &name);
	textures_.emplace_back(id, name);

	bind(name);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return textures_.back();
}

void TextureManager::upload(const Texture& texture,
                            unsigned int internal_format,
                            unsigned int width,
                            unsigned int height,
                            unsigned int format,
                            const void* data)
{
	assert_texture_size(width, height);

	bind(texture);
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format,
	             GL_UNSIGNED_BYTE, data);

	R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to upload texture");

#if RAINBOW_RECORD_VMEM_USAGE
	perform_if(textures_,
	           texture,
	           [this, width, height](TextureHandle& texture) {
	               texture.size = width * height * 4;
	               mem_used_ += texture.size;
	           });
	update_usage();
#endif
}

void TextureManager::upload_compressed(const Texture& texture,
                                       unsigned int format,
                                       unsigned int width,
                                       unsigned int height,
                                       unsigned int size,
                                       const void* data)
{
	assert_texture_size(width, height);

	bind(texture);
	glCompressedTexImage2D(
	    GL_TEXTURE_2D, 0, format, width, height, 0, size, data);

	R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to upload texture");

#if RAINBOW_RECORD_VMEM_USAGE
	perform_if(textures_, texture, [this, &size](TextureHandle& texture) {
		texture.size = size;
		mem_used_ += texture.size;
	});
	update_usage();
#endif
}

void TextureManager::release(const Texture& t)
{
	perform_if(textures_, t, [](TextureHandle& texture) {
		--texture.use_count;
	});
}

void TextureManager::retain(const Texture& t)
{
	perform_if(textures_, t, [](TextureHandle& texture) {
		++texture.use_count;
	});
}

#if RAINBOW_RECORD_VMEM_USAGE
TextureManager::MemoryUsage TextureManager::memory_usage() const
{
	const double M = 1e-6;
	return {mem_used_ * M, mem_peak_ * M};
}

void TextureManager::update_usage()
{
	if (mem_used_ > mem_peak_)
		mem_peak_ = mem_used_;

	LOGD("Video: %.2f MBs of textures", memory_usage().used);
}
#endif
