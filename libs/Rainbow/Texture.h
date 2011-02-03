#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <Rainbow/AssetManager.h>
#include <Rainbow/OpenGL.h>

#if defined(RAINBOW_IOS)
#	include <UIKit/UIKit.h>
#else
#	include <cstdio>
#	include <png.h>

struct png_read_struct
{
	unsigned int offset;
	unsigned char *data;

	png_read_struct(unsigned char *data = 0, unsigned int offset = 0) : offset(offset), data(data) { }
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
/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class Texture
{
public:
	GLuint name;
	GLsizei width, height;

	Texture(const char *filename)
	{
		GLint format = GL_RGBA;  // Default texture format
		void *data = 0;

		#if defined(RAINBOW_IOS)
		{
			NSString *file = [NSString stringWithUTF8String:(filename)];
			NSString *path = [[NSBundle mainBundle] pathForResource:[file stringByDeletingPathExtension] ofType:[file pathExtension]];
			NSData *texture = [[NSData alloc] initWithContentsOfFile:path];
			UIImage *image = [[UIImage alloc] initWithData:texture];
			[texture release];
			assert(image != nil);

			this->width = CGImageGetWidth(image.CGImage);
			this->height = CGImageGetHeight(image.CGImage);
			CGRect bounds = CGRectMake(0, 0, this->width, this->height);

			CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();
			data = malloc(this->height * this->width * 4);
			CGContextRef context = CGBitmapContextCreate(data, this->width, this->height, 8, 4 * this->width, color_space, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
			CGColorSpaceRelease(color_space);

			CGContextClearRect(context, bounds);
			CGContextTranslateCTM(context, 0, 0);
			CGContextDrawImage(context, bounds, image.CGImage);
			CGContextRelease(context);

			[image release];
		}
		#else
		{
			// Load file into memory
			png_read_struct texture;
			texture.offset = 8;
			AssetManager::Instance().load(texture.data, filename);

			// Check PNG signature
			{
				unsigned char png_sig[texture.offset];
				memcpy(png_sig, texture.data, texture.offset);
				int result = png_sig_cmp(png_sig, 0, texture.offset);
				assert(result == 0);
			}

			png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
			assert(png_ptr != 0);

			png_infop info_ptr = png_create_info_struct(png_ptr);
			assert(info_ptr != 0);

			png_infop end_info = png_create_info_struct(png_ptr);
			assert(end_info != 0);

			int result = setjmp(png_jmpbuf(png_ptr));
			assert(result == 0);

			// Texture can't be greater than what the hardware supports
			png_set_user_limits(png_ptr, GL_MAX_TEXTURE_SIZE, GL_MAX_TEXTURE_SIZE);

			// Initialise PNG reading
			png_set_sig_bytes(png_ptr, texture.offset);
			png_set_read_fn(png_ptr, &texture, mem_fread);

			// Retrieve PNG info
			png_read_info(png_ptr, info_ptr);
			if (png_get_channels(png_ptr, info_ptr) != 4)
			{
				switch(png_get_color_type(png_ptr, info_ptr))
				{
					case PNG_COLOR_TYPE_GRAY:
						if (png_get_bit_depth(png_ptr, info_ptr) < 8)
							png_set_expand_gray_1_2_4_to_8(png_ptr);
						format = GL_LUMINANCE;
						break;
					case PNG_COLOR_TYPE_PALETTE:
						png_set_palette_to_rgb(png_ptr);
					case PNG_COLOR_TYPE_RGB:
						format = GL_RGB;
						break;
					case PNG_COLOR_TYPE_GRAY_ALPHA:
						if (png_get_bit_depth(png_ptr, info_ptr) < 8)
							png_set_expand_gray_1_2_4_to_8(png_ptr);
						format = GL_LUMINANCE_ALPHA;
						break;
					default:
						printf("Unknown PNG color type: %u\n", png_get_color_type(png_ptr, info_ptr));
						break;
				}
			}
			this->width = png_get_image_width(png_ptr, info_ptr);
			this->height = png_get_image_height(png_ptr, info_ptr);
			assert(this->width > 0 && this->height > 0);

			// Allocate memory for bitmap
			png_read_update_info(png_ptr, info_ptr);
			const unsigned int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
			data = malloc(this->height * rowbytes);
			assert(data != 0);

			// Allocate row pointer array
			png_bytep *row_pointers = new png_bytep[this->height];
			assert(row_pointers != 0);

			png_byte *b = static_cast<png_byte *>(data);
			row_pointers[0] = b;
			for (int i = 1; i < this->height; ++i)
				row_pointers[i] = b + i * rowbytes;

			png_read_image(png_ptr, row_pointers);

			delete[] row_pointers;
			png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		}
		#endif

		glGenTextures(1, &this->name);
		glBindTexture(GL_TEXTURE_2D, this->name);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, format, GL_UNSIGNED_BYTE, data);

		free(data);
	}

private:

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
