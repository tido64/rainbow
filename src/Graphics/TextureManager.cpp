// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "Graphics/OpenGL.h"
#include "Graphics/TextureManager.h"

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

	this->bind(tex.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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

	this->bind(tex.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
			this->textures.qerase(i);
			break;
		}
	}
}

#ifndef NDEBUG
void TextureManager::print_usage() const
{
	const double mb = 4e-6;
	const double recycled = this->recycled.size() * 64 * 64 * mb;
	double total = recycled;
	for (size_t i = 0; i < this->textures.size(); ++i)
		total += this->textures[i].sz;

	printf("[Rainbow] Video: %.2f MBs of textures (%.2f MBs unused)\n",
	       total * mb, recycled);
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
