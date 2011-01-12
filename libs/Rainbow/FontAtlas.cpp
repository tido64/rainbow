/*
 *  FontAtlas.cpp
 *  OnWire
 *
 *  Created by Tommy Nguyen on 12/13/10.
 *  Copyright 2010 Bifrost Games. All rights reserved.
 *
 */

#include <Rainbow/FontAtlas.h>

FontAtlas::FontAtlas(const char *f, const unsigned int pt) : pt(pt)
{
	assert(f != 0);

	unsigned char *font_data = 0;
	const unsigned int font_sz = AssetManager::Instance()->load(font_data, f);

	FT_Library lib;
	int ft_error = FT_Init_FreeType(&lib);
	assert(ft_error == 0);

	FT_Face face;
	ft_error = FT_New_Memory_Face(lib, font_data, font_sz, 0, &face);
	assert(ft_error == 0);
	FT_Set_Char_Size(face, this->pt << 6, this->pt << 6, 96, 96);

	glGenTextures(chars, this->textures);
	for (unsigned char i = 0; i < chars; ++i)
	{
		ft_error = FT_Load_Glyph(face, FT_Get_Char_Index(face, i), FT_LOAD_DEFAULT);
		assert(ft_error == 0);
		FT_Glyph glyph;
		ft_error = FT_Get_Glyph(face->glyph, &glyph);
		assert(ft_error == 0);
		ft_error = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1);
		assert(ft_error == 0);
		FT_BitmapGlyph bitmap_glyph = reinterpret_cast<FT_BitmapGlyph>(glyph);
		FT_Bitmap &bitmap = bitmap_glyph->bitmap;

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
			unsigned char *d_ptr = data + (padding * (width + 1)) * 2;

			tmp = (width - w) * 2;
			for (int y = 0; y < bitmap.rows; ++y)
			{
				for (int x = 0; x < w; ++x)
				{
					*d_ptr = *(d_ptr + 1) = *buf;
					d_ptr += 2;
					++buf;
				}
				d_ptr += tmp;
			}

			glBindTexture(GL_TEXTURE_2D, this->textures[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
		}

		// Create a custom font glyph
		FontGlyph *fg = &this->charset[i];
		fg->advance = face->glyph->advance.x / 64 - margin;

		// Vertices
		fg->quad[0].position.x = bitmap.width + bitmap_glyph->left;
		fg->quad[0].position.y = bitmap_glyph->top;

		fg->quad[1].position.x = bitmap_glyph->left;
		fg->quad[1].position.y = fg->quad[0].position.y;

		fg->quad[2].position.x = fg->quad[0].position.x;
		fg->quad[2].position.y = bitmap_glyph->top - bitmap.rows;

		fg->quad[3].position.x = fg->quad[1].position.x;
		fg->quad[3].position.y = fg->quad[2].position.y;

		// Texture coordinates
		fg->quad[0].texcoord.x = (bitmap.width + (padding + margin)) / static_cast<float>(width);
		fg->quad[0].texcoord.y = (padding - margin) / static_cast<float>(height);

		fg->quad[1].texcoord.x = (padding - margin) / static_cast<float>(width);
		fg->quad[1].texcoord.y = fg->quad[0].texcoord.y;

		fg->quad[2].texcoord.x = fg->quad[0].texcoord.x;
		fg->quad[2].texcoord.y = (bitmap.rows + (padding + margin)) / static_cast<float>(height);

		fg->quad[3].texcoord.x = fg->quad[1].texcoord.x;
		fg->quad[3].texcoord.y = fg->quad[2].texcoord.y;

		FT_Done_Glyph(glyph);
	}

	FT_Done_Face(face);
	FT_Done_FreeType(lib);
	free(font_data);
}

void FontAtlas::print(const char *text, const float x, const float y) const
{
	unsigned int i = 0;

	//glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	while (text[i] != '\0')
	{
		const unsigned int c = static_cast<unsigned int>(text[i]);
		glBindTexture(GL_TEXTURE_2D, this->textures[c]);
		glVertexPointer(2, GL_FLOAT, sizeof(SpriteVertex), &this->charset[c].quad[0].position);
		glTexCoordPointer(2, GL_FLOAT, sizeof(SpriteVertex), &this->charset[c].quad[0].texcoord);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glTranslatef(this->charset[c].advance, 0.0f, 0.0f);
		++i;
	}
	glPopMatrix();
}
