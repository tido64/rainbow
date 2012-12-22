// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "Graphics/OpenGL.h"
#include "Graphics/TextureManager.h"

TextureManager::TextureManager() :
	active(0), mag_filter(GL_LINEAR), min_filter(GL_LINEAR), mem_peak(0.0),
	mem_used(0.0) { }

TextureManager::~TextureManager()
{
	this->purge(this->recycled);
	this->purge(this->textures);
}

void TextureManager::bind(const unsigned int id)
{
	if (id == this->active)
		return;

	glBindTexture(GL_TEXTURE_2D, id);
	this->active = id;
}

unsigned int TextureManager::create(const unsigned int internal_format,
                                    const unsigned int width,
                                    const unsigned int height,
                                    const unsigned int format,
                                    const void *data)
{
	TextureId tex = { 0, 0 };
	if (!this->recycled.size())
		glGenTextures(1, &tex.id);
	else
	{
		tex = this->recycled[0];
		this->recycled.qerase(0);
	}
	tex.sz = width * height;

	this->mem_used += tex.sz;
	if (this->mem_used > this->mem_peak)
		this->mem_peak = this->mem_used;

	this->bind(tex.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->mag_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to load texture");

	this->textures.push_back(tex);

#ifndef NDEBUG
	this->print_usage();
#endif

	return tex.id;
}

unsigned int TextureManager::create_compressed(const unsigned int format,
                                               const unsigned int width,
                                               const unsigned int height,
                                               const size_t size,
                                               const void *data)
{
	TextureId tex = { 0, 0 };
	if (!this->recycled.size())
		glGenTextures(1, &tex.id);
	else
	{
		tex = this->recycled[0];
		this->recycled.qerase(0);
	}
	tex.sz = width * height >> 1;

	this->mem_used += tex.sz;
	if (this->mem_used > this->mem_peak)
		this->mem_peak = this->mem_used;

	this->bind(tex.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->mag_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glCompressedTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, size, data);
	R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to load compressed texture");

	this->textures.push_back(tex);

#ifndef NDEBUG
	this->print_usage();
#endif

	return tex.id;
}

void TextureManager::get_usage(double &used, double &unused, double &peak) const
{
	unused = this->recycled.size() * 64 * 64 * 1e-6;
	used = this->mem_used * 4e-6 + unused;
	peak = this->mem_peak * 4e-6;
}

void TextureManager::remove(const unsigned int id)
{
	for (size_t i = 0; i < this->textures.size(); ++i)
	{
		if (this->textures[i].id == id)
		{
			this->bind(this->textures[i].id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 64, 64, 0,
			             GL_LUMINANCE, GL_UNSIGNED_BYTE, nullptr);
			this->bind();
			this->recycled.push_back(this->textures[i]);
			this->mem_used -= this->textures[i].sz;
			this->textures.qerase(i);
			break;
		}
	}
}

void TextureManager::set_filter(const int filter)
{
	R_ASSERT(filter == GL_NEAREST || filter == GL_LINEAR,
	         "Invalid texture filter");

	this->mag_filter = filter;
	this->min_filter = filter;
}

#ifndef NDEBUG
void TextureManager::print_usage() const
{
	double used, unused, peak;
	this->get_usage(used, unused, peak);
	printf("[Rainbow] Video: %.2f MBs of textures (%.2f MBs unused)\n", used, unused);
}
#endif

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
	              "int cannot be greater than TextureId");
	unsigned int *textures = reinterpret_cast<unsigned int*>(t.begin());
	for (size_t i = 0; i < t.size(); ++i)
		textures[i] = t[i].id;

	glDeleteTextures(t.size(), textures);
	t.clear();

#ifndef NDEBUG
	this->print_usage();
#endif
}
