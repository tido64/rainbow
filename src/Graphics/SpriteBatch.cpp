#include "Graphics/SpriteBatch.h"

const unsigned char SpriteBatch::indices[] = { 0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9, 10, 10, 9, 11, 12, 13, 14, 14, 13, 15, 16, 17, 18, 18, 17, 19, 20, 21, 22, 22, 21, 23, 24, 25, 26, 26, 25, 27, 28, 29, 30, 30, 29, 31, 32, 33, 34, 34, 33, 35, 36, 37, 38, 38, 37, 39, 40, 41, 42, 42, 41, 43, 44, 45, 46, 46, 45, 47, 48, 49, 50, 50, 49, 51, 52, 53, 54, 54, 53, 55, 56, 57, 58, 58, 57, 59, 60, 61, 62, 62, 61, 63, 64, 65, 66, 66, 65, 67, 68, 69, 70, 70, 69, 71, 72, 73, 74, 74, 73, 75, 76, 77, 78, 78, 77, 79, 80, 81, 82, 82, 81, 83, 84, 85, 86, 86, 85, 87, 88, 89, 90, 90, 89, 91, 92, 93, 94, 94, 93, 95, 96, 97, 98, 98, 97, 99, 100, 101, 102, 102, 101, 103, 104, 105, 106, 106, 105, 107, 108, 109, 110, 110, 109, 111, 112, 113, 114, 114, 113, 115, 116, 117, 118, 118, 117, 119, 120, 121, 122, 122, 121, 123, 124, 125, 126, 126, 125, 127, 128, 129, 130, 130, 129, 131, 132, 133, 134, 134, 133, 135, 136, 137, 138, 138, 137, 139, 140, 141, 142, 142, 141, 143, 144, 145, 146, 146, 145, 147, 148, 149, 150, 150, 149, 151, 152, 153, 154, 154, 153, 155, 156, 157, 158, 158, 157, 159, 160, 161, 162, 162, 161, 163, 164, 165, 166, 166, 165, 167, 168, 169, 170, 170, 169, 171, 172, 173, 174, 174, 173, 175, 176, 177, 178, 178, 177, 179, 180, 181, 182, 182, 181, 183, 184, 185, 186, 186, 185, 187, 188, 189, 190, 190, 189, 191, 192, 193, 194, 194, 193, 195, 196, 197, 198, 198, 197, 199, 200, 201, 202, 202, 201, 203, 204, 205, 206, 206, 205, 207, 208, 209, 210, 210, 209, 211, 212, 213, 214, 214, 213, 215, 216, 217, 218, 218, 217, 219, 220, 221, 222, 222, 221, 223, 224, 225, 226, 226, 225, 227, 228, 229, 230, 230, 229, 231, 232, 233, 234, 234, 233, 235, 236, 237, 238, 238, 237, 239, 240, 241, 242, 242, 241, 243, 244, 245, 246, 246, 245, 247, 248, 249, 250, 250, 249, 251, 252, 253, 254, 254, 253, 255 };

SpriteBatch::SpriteBatch(const int hint) :
	batch_vertices(0), reserved(align(hint)), texture(nullptr), sprites(hint)
{
	this->vertex_buffer = new SpriteVertex[this->reserved];
	glGenBuffers(1, &this->buffer);
}

SpriteBatch::~SpriteBatch()
{
	for (unsigned int i = 0; i < this->sprites.size(); ++i)
		delete this->sprites[i];
	delete[] this->vertex_buffer;
	delete this->texture;
}

Sprite* SpriteBatch::add(const int x, const int y, const int w, const int h)
{
	Sprite *s = new Sprite(w, h, this);
	this->push_back(s);
	s->set_texture(this->texture->create(x, y, w, h));
	return s;
}

void SpriteBatch::draw()
{
	glBindTexture(GL_TEXTURE_2D, this->texture->name);
	glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SpriteVertex), 0);
	glTexCoordPointer(2, GL_FLOAT, sizeof(SpriteVertex), SpriteVertex::tx_offset);
	glVertexPointer(2, GL_FLOAT, sizeof(SpriteVertex), SpriteVertex::vx_offset);
	glDrawElements(GL_TRIANGLES, this->batch_vertices, GL_UNSIGNED_BYTE, indices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Texture* SpriteBatch::set_texture(const Data &texture)
{
	delete this->texture;
	this->texture = new Texture(texture);
	return this->texture;
}

Texture* SpriteBatch::set_texture(Texture *t)
{
	delete this->texture;
	this->texture = t;
	return this->texture;
}

void SpriteBatch::push_back(Sprite *s)
{
	assert(this->sprites.size() <= 16 || !"Rainbow::SpriteBatch: Hard-coded limit reached");

	const unsigned int index = this->sprites.size() << 2;
	if (index > this->reserved)
	{
		this->reserved <<= 1;
		SpriteVertex *batch = new SpriteVertex[this->reserved];
		memcpy(batch, this->vertex_buffer, this->sprites.size() * sizeof(SpriteVertex));
		delete[] this->vertex_buffer;
		this->vertex_buffer = batch;

		// Assign new sprite buffers
		for (unsigned int i = 0; i < this->sprites.size(); ++i)
			this->sprites[i]->vertex_array = this->vertex_buffer + (i << 2);
	}

	// Assign the batch's buffer to the sprite
	s->vertex_array = &this->vertex_buffer[index];
	this->sprites.push_back(s);
	this->batch_vertices += 6;
}

void SpriteBatch::update()
{
	// Update all sprite vertices
	for (unsigned int i = 0; i < this->sprites.size(); ++i)
		this->sprites[i]->update();

	// Update vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
	glBufferData(GL_ARRAY_BUFFER, (this->sprites.size() << 2) * sizeof(SpriteVertex), this->vertex_buffer, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
