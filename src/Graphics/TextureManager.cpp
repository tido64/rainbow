// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/TextureManager.h"

#include <algorithm>

#include "Graphics/OpenGL.h"

void TextureManager::bind(const unsigned int id)
{
	if (id == active_[0])
		return;

	glBindTexture(GL_TEXTURE_2D, id);
	active_[0] = id;
}

void TextureManager::bind(const unsigned int id, const unsigned int unit)
{
	R_ASSERT(unit < kNumTextureUnits, "Invalid texture unit");

	if (id == active_[unit])
		return;

	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, id);
	glActiveTexture(GL_TEXTURE0);
	active_[unit] = id;
}

unsigned int TextureManager::create(const unsigned int internal_format,
                                    const unsigned int width,
                                    const unsigned int height,
                                    const unsigned int format,
                                    const void *data)
{
	TextureId texture = create_texture(width * height);
	glTexImage2D(
	    GL_TEXTURE_2D, 0, internal_format, width, height, 0, format,
	    GL_UNSIGNED_BYTE, data);
	R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to load texture");

	textures_.push_back(texture);
	print_usage();
	return texture.id;
}

unsigned int TextureManager::create_compressed(const unsigned int format,
                                               const unsigned int width,
                                               const unsigned int height,
                                               const size_t size,
                                               const void *data)
{
	TextureId texture = create_texture(width * height >> 1);
	glCompressedTexImage2D(
	    GL_TEXTURE_2D, 0, format, width, height, 0, size, data);
	R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to load compressed texture");

	textures_.push_back(texture);
	print_usage();
	return texture.id;
}

#if RECORD_VMEM_USAGE
void
TextureManager::memory_usage(double &used, double &unused, double &peak) const
{
	unused = recycled_.size() * 64 * 64 * 1e-6;
	used = mem_used_ * 4e-6 + unused;
	peak = mem_peak_ * 4e-6;
}
#endif

void TextureManager::remove(const unsigned int id)
{
	const int i = textures_.find(TextureId{id, 0});
	if (i < 0)
		return;

	bind(textures_[i].id);
	glTexImage2D(
	    GL_TEXTURE_2D, 0, GL_LUMINANCE, 64, 64, 0, GL_LUMINANCE,
	    GL_UNSIGNED_BYTE, nullptr);
	bind();
	recycled_.push_back(textures_[i]);
#if RECORD_VMEM_USAGE
	mem_used_ -= textures_[i].sz;
#endif
	textures_.qerase(i);
}

void TextureManager::set_filter(const int filter)
{
	R_ASSERT(filter == GL_NEAREST || filter == GL_LINEAR,
	         "Invalid texture filter");

	mag_filter_ = filter;
	min_filter_ = filter;
}

TextureManager::TextureManager()
    : mag_filter_(GL_LINEAR)
    , min_filter_(GL_LINEAR)
#if RECORD_VMEM_USAGE
    , mem_peak_(0.0)
    , mem_used_(0.0)
#endif
{
	std::fill_n(active_, kNumTextureUnits, 0);
	make_global();
}

TextureManager::~TextureManager()
{
	purge(recycled_);
	purge(textures_);
}

TextureManager::TextureId
TextureManager::create_texture(const unsigned int size)
{
	TextureId texture = { 0, size };
	if (!recycled_.size())
		glGenTextures(1, &texture.id);
	else
	{
		texture.id = recycled_[0].id;
		recycled_.qerase(0);
	}
#if RECORD_VMEM_USAGE
	mem_used_ += texture.sz;
	if (mem_used_ > mem_peak_)
		mem_peak_ = mem_used_;
#endif

	bind(texture.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return texture;
}

void TextureManager::print_usage() const
{
#ifndef NDEBUG
	double used, unused, peak;
	memory_usage(used, unused, peak);
	R_DEBUG("[Rainbow] Video: %.2f MBs of textures (%.2f MBs unused)\n",
	        used, unused);
#endif
}

void TextureManager::purge(Vector<TextureId> &t)
{
	if (!t.size())
		return;

	/* Hack to avoid allocating a new array of texture ids.
	 *
	 * Since sizeof(int) < sizeof(TextureId) and we will clear the vector by
	 * the end of this function, we can safely overwrite the array and avoid
	 * allocating more memory.
	 */
	static_assert(sizeof(unsigned int) < sizeof(TextureId),
	              "sizeof(int) needs to be smaller than sizeof(TextureId)");

	unsigned int *textures = reinterpret_cast<unsigned int*>(t.begin());
	for (size_t i = 0; i < t.size(); ++i)
		textures[i] = t[i].id;

	glDeleteTextures(t.size(), textures);
	t.clear();

	print_usage();
}
