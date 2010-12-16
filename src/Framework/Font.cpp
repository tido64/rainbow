/*
 *  Font.cpp
 *  OnWire
 *
 *  Created by Tommy Nguyen on 12/13/10.
 *  Copyright 2010 Ninja Unicorn. All rights reserved.
 *
 */

#include "Font.h"

Font::Font(const char *f, unsigned int pt) : pt(pt)
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

		FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1);
		FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
		FT_Bitmap &bitmap = bitmap_glyph->bitmap;

		const int width = next_pow2(bitmap.width);
		const int height = next_pow2(bitmap.rows);

		GLubyte *data = new GLubyte[width * height * 2];
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				const int j = (x + y * width) * 2;
				data[j] = 255;
				data[j + 1] = (x >= bitmap.width || y >= bitmap.rows) ? 0 : bitmap.buffer[x + y * bitmap.width];
			}
		}
		glBindTexture(GL_TEXTURE_2D, this->textures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
		delete[] data;

		const unsigned int char_i = i * char_sz;
		const float x = static_cast<float>(bitmap.width) / width;
		const float y = static_cast<float>(bitmap.rows) / height;

		// Vertices
		this->charset[char_i     ] = bitmap.width; this->charset[char_i +  1] = bitmap.rows;
		this->charset[char_i +  4] = 0;            this->charset[char_i +  5] = bitmap.rows;
		this->charset[char_i +  8] = bitmap.width; this->charset[char_i +  9] = 0;
		this->charset[char_i + 12] = 0;            this->charset[char_i + 13] = 0;

		// Texture coordinates
		this->charset[char_i +  2] = x; this->charset[char_i +  3] = y;
		this->charset[char_i +  6] = 0; this->charset[char_i +  7] = y;
		this->charset[char_i + 10] = x; this->charset[char_i + 11] = 0;
		this->charset[char_i + 14] = 0; this->charset[char_i + 15] = 0;

		/* If it's up-side down
		this->charset[char_i +  2] = x; this->charset[char_i +  3] = 0;
		this->charset[char_i +  6] = 0; this->charset[char_i +  7] = 0;
		this->charset[char_i + 10] = x; this->charset[char_i + 11] = y;
		this->charset[char_i + 14] = 0; this->charset[char_i + 15] = y;
		*/

		FT_Done_Glyph(glyph);
	}

	FT_Done_Face(face);
	FT_Done_FreeType(lib);
}

Font::~Font()
{

}

void Font::test()
{
	// Let's print a red letter A
	const unsigned char c = 38;
	const unsigned int c_offset = c * char_sz;

	glColor4ub(0xff, 0x00, 0x00, 0xff);

	glPushMatrix();
	glTranslatef(100.0f, 100.0f, 0.0f);
	glBindTexture(GL_TEXTURE_2D, this->textures[c]);
	glVertexPointer(2, GL_FLOAT, 4 * sizeof(float), &this->charset[c_offset]);
	glTexCoordPointer(2, GL_FLOAT, 4 * sizeof(float), &this->charset[c_offset + 2]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glPopMatrix();
}

/*
void Font::load_font(const char *f, const unsigned int h)
{
	FT_Face face;
	if (FT_New_Face(this->lib, f, 0, &face))
		throw "FT_New_Face: Could not load specified font";
	FT_Set_Char_Size(face, h << 6, h << 6, 96, 96);
	//FT_Set_Pixel_Sizes(face, 0, 16);

	// Create OGL shit here
	GLuint font_tex[122 - 32];
	for (unsigned char i = 65; i < 66; ++i)
	{
		if (FT_Load_Glyph(face, FT_Get_Char_Index(face, i), FT_LOAD_DEFAULT))
			throw "FT_Load_Glyph: Failed";

		FT_Glyph g;
		if (FT_Get_Glyph(face->glyph, &g))
			throw "FT_Get_Glyph: Failed";

		if (face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
			FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

		//FT_Glyph_To_Bitmap(&g, FT_RENDER_MODE_NORMAL, 0, 1);
		//FT_BitmapGlyph bitmap_glyph = reinterpret_cast<FT_BitmapGlyph>(g);
		FT_Bitmap &bitmap = face->glyph->bitmap;

		const int width = this->next_pow2(bitmap.width);
		const int height = this->next_pow2(bitmap.rows);
		GLubyte *data = new GLubyte[2 * width * height];
		for (int j = 0; j < height; ++j)
		{
			for (int k = 0; k < width; ++k)
			{
				data[2 * (k + j * width)] = 255;
				data[2 * (k + j * width) + 1] = (k >= bitmap.width || j >= bitmap.rows) ? 0 : bitmap.buffer[k + j * bitmap.width];
			}
		}

		glGenTextures(1, &font_tex[i]);
		glBindTexture(GL_TEXTURE_2D, font_tex[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
		delete[] data;

		// create vertex array (vbo?)
		//glBindTexture(GL_TEXTURE_2D, font_tex[i]);
		glPushMatrix();
		glTranslatef(120.0f, 100.0f, 0.0f);
		glTranslatef(face->glyph->bitmap_top, face->glyph->bitmap_top - bitmap.rows, 0.0f);
		float x = static_cast<float>(bitmap.width) / width;
		float y = static_cast<float>(bitmap.rows) / height;

		float vertex_array[16];
		vertex_array[0] = bitmap.width;
		vertex_array[1] = bitmap.rows;
		vertex_array[4] = 0;
		vertex_array[5] = bitmap.rows;
		vertex_array[8] = bitmap.width;
		vertex_array[9] = 0;
		vertex_array[12] = 0;
		vertex_array[13] = 0;

		vertex_array[2] = x;
		vertex_array[3] = 0;
		vertex_array[6] = 0;
		vertex_array[7] = 0;
		vertex_array[10] = x;
		vertex_array[11] = y;
		vertex_array[14] = 0;
		vertex_array[15] = y;

		glVertexPointer(2, GL_FLOAT, 4 * sizeof(float), vertex_array);
		glTexCoordPointer(2, GL_FLOAT, 4 * sizeof(float), &vertex_array[2]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glPopMatrix();

		FT_Done_Glyph(g);
	}

	FT_Done_Face(face);
}
*/
