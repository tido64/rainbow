//
//  TextureAtlas.cpp
//  OnWire
//
//  Created by Tommy Nguyen on 12/7/10.
//  Copyright 2010 Ninja Unicorn. All rights reserved.
//

#include "TextureAtlas.h"

TextureAtlas::TextureAtlas(const char *filename, const int mipmap) : textures(64)
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

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_SRC_COLOR);

	glGenTextures(1, &this->texture[0]);
	glBindTexture(GL_TEXTURE_2D, this->texture[0]);

	// Enable mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, mipmap, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	free(data);
}

Sprite* TextureAtlas::create_sprite(const unsigned int x, const unsigned int y, const unsigned int w, const unsigned int h)
{
	const unsigned int i = this->define_texture(x, y, w, h);
	Sprite *sprite = new Sprite(this, w, h);
	sprite->set_texture(i);
	this->sprites.add(sprite);
	return sprite;
}

unsigned int TextureAtlas::define_texture(const unsigned int x, const unsigned int y, const unsigned int w, const unsigned int h)
{
	assert(x >= 0 && (x + w) <= this->width);
	assert(y >= 0 && (y + h) <= this->height);

	unsigned int i = this->textures.size();

	const float x0 = x / static_cast<float>(this->width);
	const float x1 = (x + w) / static_cast<float>(this->width);
	const float y0 = y / static_cast<float>(this->height);
	const float y1 = (y + h) / static_cast<float>(this->height);

	this->textures.add(x1); this->textures.add(y0);  // (1,0)
	this->textures.add(x0); this->textures.add(y0);  // (0,0)
	this->textures.add(x1); this->textures.add(y1);  // (1,1)
	this->textures.add(x0); this->textures.add(y1);  // (0,1)

	return i;
}

/*/ Default GL states: GL_TEXTURE_2D, GL_VERTEX_ARRAY, GL_COLOR_ARRAY, GL_TEXTURE_COORD_ARRAY
void TextureAtlas::draw()
{
	static const unsigned int stride = 4 * sizeof(float);
	static const void *tex_offset = reinterpret_cast<float *>(0) + 2;

	// Enables all colour channels
	glColor4ub(0xff, 0xff, 0xff, 0xff);

	// Loads texture into GPU memory.
	// Note: Need to check whether this is necessary every time.
	glBindTexture(GL_TEXTURE_2D, this->texture[0]);

	Sprite *s = 0;
	for (unsigned int i = 0; i < this->sprites.size(); ++i)
	{
		s = this->sprites[i];
		glBindBuffer(GL_ARRAY_BUFFER, s->buffer);
		glVertexPointer(2, GL_FLOAT, stride, 0);
		glTexCoordPointer(2, GL_FLOAT, stride, tex_offset);

		if (s->rotation != 0.0f)
		{
			glPushMatrix();
			glTranslatef(s->pivot_abs[0], s->pivot_abs[1], 0.0f);
			glRotatef(-s->rotation, 0.0f, 0.0f, 1.0f);
			glTranslatef(-s->pivot_abs[0], -s->pivot_abs[1], 0.0f);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glPopMatrix();
		}
		else
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}*/

void TextureAtlas::get_texture(const unsigned int i, float *vertices)
{
	vertices[ 2] = this->textures[i];
	vertices[ 3] = this->textures[i + 1];

	vertices[ 6] = this->textures[i + 2];
	vertices[ 7] = this->textures[i + 3];

	vertices[10] = this->textures[i + 4];
	vertices[11] = this->textures[i + 5];

	vertices[14] = this->textures[i + 6];
	vertices[15] = this->textures[i + 7];
}
