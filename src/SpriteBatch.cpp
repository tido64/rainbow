#include "SpriteBatch.h"

SpriteBatch::SpriteBatch(const int hint) :
	texture(0), count(0), reserved(indexof(hint)), sprites(hint)
{
	this->vertex_buffer = new SpriteVertex[this->reserved];
	glGenBuffers(1, &this->buffer);
}

void SpriteBatch::add(Sprite *s)
{
	unsigned int offset = 0;

	if (this->texture == 0)
		this->texture = s->texture;
	else
	{
		// Make sure all sprites use the same texture
		assert(s->texture == this->texture || !"Rainbow::SpriteBatch: Added sprite to wrong batch");

		offset = this->indexof(this->sprites.size());
		if (offset >= this->reserved)
		{
			this->reserved <<= 1;
			SpriteVertex *batch = new SpriteVertex[this->reserved];
			memcpy(batch, this->vertex_buffer, offset * sizeof(SpriteVertex));
			delete[] this->vertex_buffer;
			this->vertex_buffer = batch;

			this->sprites[0]->vertex_array = batch;
			for (unsigned int i = 1; i < this->sprites.size(); ++i)
				this->sprites[i]->vertex_array = this->vertex_buffer + indexof(i);
		}

		// Copy degenerate vertices
		this->vertex_buffer[offset - 2] = this->vertex_buffer[offset - 3];
		this->vertex_buffer[offset - 1] = *s->vertex_array;
	}

	// Delete the current vertex array and assign the batch's buffer
	memcpy(this->vertex_buffer + offset, s->vertex_array, Sprite::vertex_array_sz);
	delete[] s->vertex_array;
	s->vertex_array = &this->vertex_buffer[offset];
	s->buffered = true;
	this->sprites.push_back(s);

	// Update constants
	this->batch_vertices = (this->sprites.size() << 2) + ((this->sprites.size() - 1) << 1);
	this->batch_buffer_sz = this->batch_vertices * sizeof(SpriteVertex);
}

void SpriteBatch::draw()
{
	// Offsets: Refer struct SpriteVertex
	static const void *tx_offset = reinterpret_cast<void *>(sizeof(Colorb));
	static const void *vx_offset = reinterpret_cast<void *>(sizeof(Colorb) + sizeof(Vec2f));

	glBindTexture(GL_TEXTURE_2D, this->texture);

	glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(SpriteVertex), 0);
	glTexCoordPointer(2, GL_FLOAT, sizeof(SpriteVertex), tx_offset);
	glVertexPointer(2, GL_FLOAT, sizeof(SpriteVertex), vx_offset);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, this->batch_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SpriteBatch::update()
{
	// Update all sprite vertices
	this->sprites[0]->update();
	for (unsigned int i = 1; i < this->sprites.size(); ++i)
	{
		this->sprites[i]->update();
		unsigned int offset = this->indexof(i);
		this->vertex_buffer[offset - 2] = this->vertex_buffer[offset - 3];
		this->vertex_buffer[offset - 1] = this->vertex_buffer[offset];
	}

	// Update vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
	glBufferData(GL_ARRAY_BUFFER, this->batch_buffer_sz, this->vertex_buffer, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
