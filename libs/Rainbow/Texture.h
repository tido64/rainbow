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

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <Rainbow/OpenGL.h>

#if defined(ONWIRE_IOS)
#	include <UIKit/UIKit.h>
#else
#	include <cstdio>
#	include <png.h>
#endif

struct Texture
{
	GLuint name;
	float width, height;

	Texture(const char *filename)
	{
		void *data = 0;

		#if defined(ONWIRE_IOS)
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
			// Load file into memory and make sure it is a PNG
			FILE *fp = fopen(filename, "rb");
			assert(fp != 0);
			png_byte header[8];
			int result = fread(header, 1, 8, fp);
			assert(result == 8);
			result = png_sig_cmp(header, 0, 8);
			assert(result == 0);

			png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
			assert(png_ptr != 0);

			png_infop info_ptr = png_create_info_struct(png_ptr);
			assert(info_ptr != 0);

			png_infop end_info = png_create_info_struct(png_ptr);
			assert(end_info != 0);

			if (setjmp(png_jmpbuf(png_ptr)))
			{
				png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
				fclose(fp);
				throw "Could not set longjmp";
			}

			// Texture can't be greater than what the hardware supports
			png_set_user_limits(png_ptr, GL_MAX_TEXTURE_SIZE, GL_MAX_TEXTURE_SIZE);

			// Initialise PNG reading
			png_init_io(png_ptr, fp);
			png_set_sig_bytes(png_ptr, 8);

			// Retrieve PNG info
			png_read_info(png_ptr, info_ptr);
			assert(png_get_channels(png_ptr, info_ptr) == 4);  // Ensure we're dealing with a 4-channel PNG
			const unsigned int width = png_get_image_width(png_ptr, info_ptr);
			const unsigned int height = png_get_image_height(png_ptr, info_ptr);
			assert(width > 0 && height > 0);

			png_read_update_info(png_ptr, info_ptr);
			const unsigned int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
			data = malloc(height * rowbytes);
			assert(data != 0);

			png_bytep *row_pointers = new png_bytep[height];
			assert(row_pointers != 0);

			png_byte *b = reinterpret_cast<png_byte *>(data);
			row_pointers[0] = b;
			for (unsigned int i = 1; i < height; ++i)
				row_pointers[i] = b + i * rowbytes;

			png_read_image(png_ptr, row_pointers);

			delete[] row_pointers;
			png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
			fclose(fp);

			this->width = width;
			this->height = height;
		}
		#endif

		glGenTextures(1, &this->name);
		glBindTexture(GL_TEXTURE_2D, this->name);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		free(data);
	}
};

#endif
