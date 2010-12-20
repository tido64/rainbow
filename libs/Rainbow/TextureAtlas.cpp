//
//  TextureAtlas.cpp
//  OnWire
//
//  Created by Tommy Nguyen on 12/7/10.
//  Copyright 2010 Ninja Unicorn. All rights reserved.
//

#include <Rainbow/Sprite.h>

TextureAtlas::TextureAtlas(const char *filename, const int texture_count, const int sprite_count, const int mipmap) :
	texture(filename), textures(texture_count * 8), sprites(sprite_count)
{ }

TextureAtlas::~TextureAtlas()
{
	for (unsigned int i = 0; this->sprites.size(); ++i)
		delete this->sprites[i];
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
	assert(x >= 0 && (x + w) <= this->texture.width && y >= 0 && (y + h) <= this->texture.height);

	unsigned int i = this->textures.size();

	const float x0 = x / static_cast<float>(this->texture.width);
	const float x1 = (x + w) / static_cast<float>(this->texture.width);
	const float y0 = y / static_cast<float>(this->texture.height);
	const float y1 = (y + h) / static_cast<float>(this->texture.height);

	this->textures.add(x1); this->textures.add(y0);  // (1,0)
	this->textures.add(x0); this->textures.add(y0);  // (0,0)
	this->textures.add(x1); this->textures.add(y1);  // (1,1)
	this->textures.add(x0); this->textures.add(y1);  // (0,1)

	return i;
}

void TextureAtlas::get_texture(const unsigned int i, SpriteVertex *vertices)
{
	vertices[0].texcoord.x = this->textures[i];
	vertices[0].texcoord.y = this->textures[i + 1];

	vertices[1].texcoord.x = this->textures[i + 2];
	vertices[1].texcoord.y = this->textures[i + 3];

	vertices[2].texcoord.x = this->textures[i + 4];
	vertices[2].texcoord.y = this->textures[i + 5];

	vertices[3].texcoord.x = this->textures[i + 6];
	vertices[3].texcoord.y = this->textures[i + 7];
}
