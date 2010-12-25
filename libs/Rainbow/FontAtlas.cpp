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
	FT_Library lib;
	if (FT_Init_FreeType(&lib))
		throw "FT_Init_FreeType: An error occurred during FreeType initialisation";

	FT_Face face;
	if (FT_New_Face(lib, f, 0, &face))
		throw "FT_New_Face: An error occurred while loading your font";
	FT_Set_Char_Size(face, this->pt << 6, this->pt << 6, 96, 96);

	glGenTextures(chars, this->textures);
	for (unsigned char i = 0; i < chars; ++i)
	{
		if (FT_Load_Glyph(face, FT_Get_Char_Index(face, i), FT_LOAD_DEFAULT))
			throw "FT_Load_Glyph: Failed";

		FT_Glyph glyph;
		if (FT_Get_Glyph(face->glyph, &glyph))
			throw "FT_Get_Glyph: Failed";

		if (FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1))
			throw "FT_Glyph_To_Bitmap: Failed";
		FT_BitmapGlyph bitmap_glyph = reinterpret_cast<FT_BitmapGlyph>(glyph);
		FT_Bitmap &bitmap = bitmap_glyph->bitmap;

		const int width = (this->pt < 17) ? 16 : next_pow2(bitmap.width);
		const int height = (this->pt < 17) ? 16 : next_pow2(bitmap.rows);

		GLubyte *data = new GLubyte[width * height * 4];
		unsigned int j = 0;
		if (!bitmap.buffer)
		{
			for (int b = 0; b < width * height; ++b)
			{
				data[j++] = 0;
				data[j++] = 0;
				data[j++] = 0;
				data[j++] = 0;
			}
		}
		else
		{
			for (int y = 0; y < height; ++y)
			{
				for (int x = 0; x < width; ++x)
				{
					const unsigned char c = (x >= bitmap.width || y >= bitmap.rows) ? 0 : bitmap.buffer[x + y * bitmap.width];
					data[j++] = c;
					data[j++] = c;
					data[j++] = c;
					data[j++] = c;
				}
			}
		}

		glBindTexture(GL_TEXTURE_2D, this->textures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		delete[] data;

		//const unsigned int char_i = i * 4;
		FontGlyph *fg = &this->charset[i];
		fg->advance = face->glyph->advance.x / 64;

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
		const float x = static_cast<float>(bitmap.width) / width;
		const float y = static_cast<float>(bitmap.rows) / height;

		fg->quad[0].texcoord.x = x;
		fg->quad[0].texcoord.y = 0.0f;

		fg->quad[1].texcoord.x = 0.0f;
		fg->quad[1].texcoord.y = 0.0f;

		fg->quad[2].texcoord.x = x;
		fg->quad[2].texcoord.y = y;

		fg->quad[3].texcoord.x = 0.0f;
		fg->quad[3].texcoord.y = y;

		FT_Done_Glyph(glyph);
	}

	FT_Done_Face(face);
	FT_Done_FreeType(lib);
}

void FontAtlas::print(const char *text, const float x, const float y)
{
	unsigned int i = 0;

	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	while (text[i] != '\0')
	{
		const unsigned int c = static_cast<unsigned int>(text[i]);
		glBindTexture(GL_TEXTURE_2D, this->textures[c]);
		glTexCoordPointer(2, GL_FLOAT, sizeof(SpriteVertex), &this->charset[c].quad[0].texcoord);
		glVertexPointer(2, GL_FLOAT, sizeof(SpriteVertex), &this->charset[c].quad[0].position);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glTranslatef(this->charset[c].advance, 0.0f, 0.0f);
		++i;
	}
	glPopMatrix();
}
