// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/TextureManager.h"

#include "Common/Algorithm.h"
#include "Graphics/OpenGL.h"

using rainbow::quick_erase;

#if RECORD_VMEM_USAGE
TextureManager::MemoryUsage TextureManager::memory_usage() const
{
	const double M = 1e-6;
	return MemoryUsage{mem_used_ * M, mem_peak_ * M};
}
#endif

void TextureManager::set_filter(const int filter)
{
	R_ASSERT(filter == GL_NEAREST || filter == GL_LINEAR,
	         "Invalid texture filter");

	mag_filter_ = filter;
	min_filter_ = filter;
}

void TextureManager::bind(const unsigned int name)
{
	if (name == active_[0])
		return;

	glBindTexture(GL_TEXTURE_2D, name);
	active_[0] = name;
}

void TextureManager::bind(const unsigned int name, const unsigned int unit)
{
	R_ASSERT(unit < kNumTextureUnits, "Invalid texture unit");

	if (name == active_[unit])
		return;

	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, name);
	glActiveTexture(GL_TEXTURE0);
	active_[unit] = name;
}

unsigned int TextureManager::create()
{
	unsigned int name;
	if (recycled_.size() == 0)
		glGenTextures(1, &name);
	else
	{
		const auto i = std::begin(recycled_);
		name = *i;
		quick_erase(recycled_, i);
	}
	textures_.emplace_back(name);

	bind(name);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return name;
}

unsigned int TextureManager::create(const unsigned int internal_format,
                                    const unsigned int width,
                                    const unsigned int height,
                                    const unsigned int format,
                                    const void *data)
{
	const auto name = create();
	upload(name, internal_format, width, height, format, data);
	return name;
}

unsigned int TextureManager::create_compressed(const unsigned int format,
                                               const unsigned int width,
                                               const unsigned int height,
                                               const size_t size,
                                               const void *data)
{
	const auto name = create();
	glCompressedTexImage2D(
	    GL_TEXTURE_2D, 0, format, width, height, 0, size, data);

	R_ASSERT(glGetError() == GL_NO_ERROR,
	         "Failed to upload compressed texture");

#if RECORD_VMEM_USAGE
	record_usage(textures_.back(), size);
#endif
	return name;
}

void TextureManager::purge()
{
	if (recycled_.size() == 0)
		return;

	glDeleteTextures(recycled_.size(), recycled_.data());
	recycled_.clear();

#if RECORD_VMEM_USAGE
	print_usage();
#endif
}

void TextureManager::remove(const unsigned int name)
{
	const auto i = std::find(std::begin(textures_), std::end(textures_), name);
	if (i == std::end(textures_))
		return;

	bind(*i);
	glTexImage2D(
	    GL_TEXTURE_2D, 0, GL_LUMINANCE, 1, 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
	    nullptr);
	bind();

	recycled_.push_back(*i);
#if RECORD_VMEM_USAGE
	mem_used_ -= i->size;
#endif
	quick_erase(textures_, i);
}

void TextureManager::upload(const unsigned int name,
                            const unsigned int internal_format,
                            const unsigned int width,
                            const unsigned int height,
                            const unsigned int format,
                            const void *data)
{
	bind(name);
	glTexImage2D(
	    GL_TEXTURE_2D, 0, internal_format, width, height, 0, format,
	    GL_UNSIGNED_BYTE, data);

	R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to upload texture");

#if RECORD_VMEM_USAGE
	record_usage(*std::find(std::begin(textures_), std::end(textures_), name),
	             width * height * 4);
#endif
}

TextureManager::TextureManager()
    : mag_filter_(GL_LINEAR), min_filter_(GL_LINEAR)
#if RECORD_VMEM_USAGE
    , mem_peak_(0.0), mem_used_(0.0)
#endif
{
	std::fill_n(active_, kNumTextureUnits, 0);
	make_global();
}

#if RECORD_VMEM_USAGE
void TextureManager::print_usage() const
{
	LOGD("Video: %.2f MBs of textures", memory_usage().used);
}

void TextureManager::record_usage(TextureName &name, const unsigned int size)
{
	name.size = size;
	mem_used_ += name.size;
	if (mem_used_ > mem_peak_)
		mem_peak_ = mem_used_;
	print_usage();
}
#endif
