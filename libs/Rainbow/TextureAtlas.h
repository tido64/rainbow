#ifndef TEXTUREATLAS_H_
#define TEXTUREATLAS_H_

#include <cstring>

#include "Texture.h"
#include "Types.h"
#include "Vector.h"

class Sprite;


/// Custom texture atlas implemented in OpenGL ES 1.1.

/// GL drawing order: (1,1) -> (0,1) -> (1,0) -> (0,0)
///
/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class TextureAtlas
{
public:
	TextureAtlas(const char *filename, const int texture_count, const int sprite_count, const int mipmap = 0);
	~TextureAtlas();

	/// Creates a sprite.
	/// \return A sprite object
	Sprite* create_sprite(const int x, const int y, const int width, const int height);

	/// Defines a texture from the map.
	/// \param x       x-component of texture location
	/// \param y       y-component of texture location
	/// \param width   Width of the texture
	/// \param height  Height of the texture
	/// \return An id unique to the texture
	unsigned int define_texture(const int x, const int y, const int width, const int height);

	/// Returns the texture name this atlas holds.
	inline GLuint get_name() const { return this->texture.name; }

	/// Gets named texture.
	/// \param id        The id of the texture to get
	/// \param vertices  Interleaved vertex data array (vertex and texture coordinates)
	void get_texture(unsigned int id, SpriteVertex *vertices);

private:
	Texture texture;           ///< Loaded texture
	Vector<Sprite *> sprites;  ///< Stores all sprites
	Vector<Vec2f> textures;    ///< Stores all textures
};

#endif
