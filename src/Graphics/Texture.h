#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "Common/Data.h"
#include "Common/Vec2.h"
#include "Common/Vector.h"
#include "OpenGL.h"

#if defined(RAINBOW_IOS)
#	include <UIKit/UIKit.h>
#else
#	include <png.h>
#endif

/// A texture loaded from an image file.
///
/// iOS Note: Textures' dimension must be (2^n) by (2^m) for some arbitrary n
/// and m, where n, m > 6.
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

	explicit Texture(const Data &data);

	/// Create a texture from file.
	/// \param x       Starting point of the texture (x-coordinate)
	/// \param y       Starting point of the texture (y-coordinate)
	/// \param width   Width of the texture
	/// \param height  Height of the texture
	/// \return The name of the texture
	unsigned int create(const int x, const int y, const int width, const int height);

	/// Trim the internal texture storage.
	void trim();

	const Vec2f* operator[](const unsigned int i) const;

private:
	Vector<Vec2f> textures;  ///< Texture coordinates

	/// Return true if the integer provided is a power of 2.
	bool is_pow2(const unsigned int);

	/// Read image data into graphics buffer.
	void load(const Data &img_data);

#ifdef PNG_LIBPNG_VER_STRING

	/// Structure for reading PNG bitmaps.
	///
	/// Copyright 2010-11 Bifrost Games. All rights reserved.
	/// \author Tommy Nguyen
	struct png_read_struct
	{
		unsigned int offset;
		const unsigned char *data;

		png_read_struct(unsigned char *data = 0, unsigned int offset = 8) :
			offset(offset), data(data) { }
	};

	static void mem_fread(png_structp png_ptr, png_bytep data, png_size_t length)
	{
		png_read_struct *read_struct = static_cast<png_read_struct *>(png_get_io_ptr(png_ptr));
		memcpy(data, read_struct->data + read_struct->offset, length);
		read_struct->offset += length;
	}

#endif
};

inline void Texture::trim()
{
	this->textures.reserve(0);
}

#endif
