#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "AssetManager.h"
#include "OpenGL.h"

#if defined(RAINBOW_IOS)
#	include <UIKit/UIKit.h>
#else
#	include <png.h>

struct png_read_struct
{
	unsigned int offset;
	unsigned char *data;

	png_read_struct(unsigned char *data = 0, unsigned int offset = 0) :
		offset(offset), data(data) { }
};

#endif


/// A texture loaded from an image file.

/// Note: Textures' dimension must be (2^n) by (2^m) for some arbitrary n and m,
/// where n > 6 and m > 6.
///
/// Note: Textures are loaded upside-down, so the coordinates must be flipped.
///
/// \see http://iphonedevelopment.blogspot.com/2009/05/opengl-es-from-ground-up-part-6_25.html
/// \see http://developer.android.com/guide/topics/resources/providing-resources.html
/// \see http://en.wikibooks.org/wiki/OpenGL_Programming/Intermediate/Textures
///
/// Copyright 2010-11 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class Texture
{
public:
	GLuint name;
	GLsizei width, height;

	Texture(const char *filename);

private:
	/// Read image data into buffer.
	/// \param[out] data  Destination buffer
	/// \param[in]  file  Path to image
	/// \return Image colour format
	GLint load(void *&data, const char *file);

#ifdef PNG_LIBPNG_VER_STRING

	static void mem_fread(png_structp png_ptr, png_bytep data, png_size_t length)
	{
		png_read_struct *read_struct = (png_read_struct *)png_get_io_ptr(png_ptr);
		memcpy(data, read_struct->data + read_struct->offset, length);
		read_struct->offset += length;
	}

#endif
};

#endif
