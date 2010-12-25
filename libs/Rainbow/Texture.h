/// A texture loaded from an image file.

/// Note: Textures' dimension must be (2^n) by (2^m) for some arbitrary n and m,
/// where n > 6 and m > 6.
///
/// Note: Textures are loaded upside-down, so the coordinates must be flipped.
///
/// \see http://iphonedevelopment.blogspot.com/2009/05/opengl-es-from-ground-up-part-6_25.html
///
/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <Rainbow/OpenGL.h>

#if defined(ONWIRE_ANDROID)
#elif defined(ONWIRE_IOS)
#	include <UIKit/UIKit.h>
#endif

struct Texture
{
	GLuint name;
	float width, height;

	Texture(const char *filename)
	{
		void *data;

	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

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
