/*
 *  FontAtlas.cpp
 *  OnWire
 *
 *  Created by Tommy Nguyen on 12/13/10.
 *  Copyright 2010 Bifrost Games. All rights reserved.
 *
 */

#include "FontAtlas.h"

FontAtlas::FontAtlas(const char *f, const float pt) : pt(pt)
{
	assert(f != 0);

	// Read font into memory
	unsigned char *font_data = 0;
	const unsigned int font_sz = AssetManager::Instance().load(font_data, f);

	// Instantiate FreeType
	FT_Library lib;
	int ft_error = FT_Init_FreeType(&lib);
	assert(ft_error == 0);

	// Load font face
	FT_Face face;
	ft_error = FT_New_Memory_Face(lib, font_data, font_sz, 0, &face);
	assert(ft_error == 0);

	FT_Set_Char_Size(face, 0, this->pt * 64, 96, 96);

	glGenTextures(chars, this->textures);
	for (unsigned int i = 0; i < chars; ++i)
	{
		ft_error = FT_Load_Char(face, i + ascii_offset, FT_LOAD_RENDER);
		//assert(ft_error == 0);

		const FT_GlyphSlot &slot = face->glyph;
		const FT_Bitmap &bitmap = slot->bitmap;

		const int width = next_pow2(bitmap.width + (padding << 1));
		const int height = next_pow2(bitmap.rows + (padding << 1));

		// Create texture
		if (!bitmap.buffer)
			this->textures[i] = 0;
		else
		{
			unsigned int tmp = width * height * 2;
			GLubyte data[tmp];
			memset(data, 0, tmp);

			const int w = bitmap.width;
			const unsigned char *buf = bitmap.buffer;
			unsigned char *d_ptr = data + ((padding << 1) * (width + 1));

			tmp = (width - w) * 2;
			for (int y = 0; y < bitmap.rows; ++y)
			{
				for (int x = 0; x < w; ++x)
				{
				#ifdef RAINBOW_IOS

					*d_ptr = *(d_ptr + 1) = *buf;

				#else

					if (*buf > 0)
					{
						*d_ptr = 255;
						*(d_ptr + 1) = *buf;
					}

				#endif

					d_ptr += 2;
					++buf;
				}
				d_ptr += tmp;
			}

			glBindTexture(GL_TEXTURE_2D, this->textures[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
		}

		// Save font glyph
		FontGlyph &fg = this->charset[i];
		fg.advance = slot->advance.x >> 6;
		fg.left = slot->bitmap_left;

		if (FT_HAS_KERNING(face))
		{
			unsigned int glyph = i + ascii_offset;
			FT_Vector kerning;
			for (unsigned int j = 0; j < chars; ++j)
			{
				FT_Get_Kerning(face, j + ascii_offset, glyph, FT_KERNING_DEFAULT, &kerning);
				fg.kern[j] = kerning.x >> 6;
			}
		}

		// Vertices
		fg.quad[0].position.x = bitmap.width + margin;
		fg.quad[0].position.y = slot->bitmap_top + margin;

		fg.quad[1].position.x = -margin;
		fg.quad[1].position.y = fg.quad[0].position.y;

		fg.quad[2].position.x = fg.quad[0].position.x;
		fg.quad[2].position.y = slot->bitmap_top - bitmap.rows - margin;

		fg.quad[3].position.x = fg.quad[1].position.x;
		fg.quad[3].position.y = fg.quad[2].position.y;

		// Texture coordinates
		fg.quad[0].texcoord.x = (padding + bitmap.width + margin) / static_cast<float>(width);
		fg.quad[0].texcoord.y = (padding - margin) / static_cast<float>(height);

		fg.quad[1].texcoord.x = (padding - margin) / static_cast<float>(width);
		fg.quad[1].texcoord.y = fg.quad[0].texcoord.y;

		fg.quad[2].texcoord.x = fg.quad[0].texcoord.x;
		fg.quad[2].texcoord.y = (padding + bitmap.rows + margin) / static_cast<float>(height);

		fg.quad[3].texcoord.x = fg.quad[1].texcoord.x;
		fg.quad[3].texcoord.y = fg.quad[2].texcoord.y;
	}

	FT_Done_Face(face);
	FT_Done_FreeType(lib);
	free(font_data);
}

void FontAtlas::print(const char *text, const float x, const float y) const
{
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glColor4ub(this->color.r, this->color.g, this->color.b, this->color.a);
	unsigned int prev = 0;  // 'space' has no kerning
	for (; *text != '\0'; ++text)
	{
		const unsigned int c = static_cast<unsigned int>(*text - ascii_offset);
		const FontGlyph &glyph = this->charset[c];

		glTranslatef(glyph.left + glyph.kern[prev], 0.0f, 0.0f);
		glBindTexture(GL_TEXTURE_2D, this->textures[c]);
		glVertexPointer(2, GL_FLOAT, sizeof(SpriteVertex), &glyph.quad[0].position);
		glTexCoordPointer(2, GL_FLOAT, sizeof(SpriteVertex), &glyph.quad[0].texcoord);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glTranslatef(glyph.advance - glyph.left, 0.0f, 0.0f);

		prev = c;
	}
	glPopMatrix();
}

void FontAtlas::set_color(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
{
	this->color.r = r;
	this->color.g = g;
	this->color.b = b;
	this->color.a = a;
}
