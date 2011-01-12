//
//  TextureAtlas.cpp
//  OnWire
//
//  Created by Tommy Nguyen on 12/7/10.
//  Copyright 2010 Bifrost Games. All rights reserved.
//

#include <Rainbow/Sprite.h>

TextureAtlas::TextureAtlas(const char *filename, const int texture_count, const int sprite_count, const int mipmap) :
	texture(filename), textures(texture_count * 8), sprites(sprite_count)
{ }

TextureAtlas::~TextureAtlas()
{
	for (unsigned int i = 0; i < this->sprites.size(); ++i)
		delete this->sprites[i];
}

Sprite* TextureAtlas::create_sprite(const int x, const int y, const int w, const int h)
{
	Sprite *sprite = new Sprite(this, w, h);
	sprite->set_texture(this->define_texture(x, y, w, h));
	this->sprites.add(sprite);
	return sprite;
}

unsigned int TextureAtlas::define_texture(const int x, const int y, const int w, const int h)
{
	assert(x >= 0 && (x + w) <= this->texture.width && y >= 0 && (y + h) <= this->texture.height);

	unsigned int i = this->textures.size();

	const float x0 = x / static_cast<float>(this->texture.width);
	const float x1 = (x + w) / static_cast<float>(this->texture.width);
	const float y0 = y / static_cast<float>(this->texture.height);
	const float y1 = (y + h) / static_cast<float>(this->texture.height);

	this->textures.add(Vec2f(x1, y0));
	this->textures.add(Vec2f(x0, y0));
	this->textures.add(Vec2f(x1, y1));
	this->textures.add(Vec2f(x0, y1));

	return i;
}

void TextureAtlas::get_texture(unsigned int i, SpriteVertex *vertices)
{
	const Vec2f *texture = this->textures.c_arr() + i;
	vertices->texcoord.x = texture->x;
	vertices->texcoord.y = texture->y;

	++texture; ++vertices;
	vertices->texcoord.x = texture->x;
	vertices->texcoord.y = texture->y;

	++texture; ++vertices;
	vertices->texcoord.x = texture->x;
	vertices->texcoord.y = texture->y;

	++texture; ++vertices;
	vertices->texcoord.x = texture->x;
	vertices->texcoord.y = texture->y;
}
