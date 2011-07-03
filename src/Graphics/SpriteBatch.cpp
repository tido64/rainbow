#include "Graphics/SpriteBatch.h"

const void *SpriteBatch::tx_offset = reinterpret_cast<void *>(sizeof(Colorb));
const void *SpriteBatch::vx_offset = reinterpret_cast<void *>(sizeof(Colorb) + sizeof(Vec2f));

SpriteBatch::SpriteBatch(const int hint) :
	reserved(align(hint)), texture(0), sprites(hint)
{
	this->vertex_buffer = new SpriteVertex[this->reserved];
	glGenBuffers(1, &this->buffer);
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
	glTexCoordPointer(2, GL_FLOAT, sizeof(SpriteVertex), tx_offset);
	glVertexPointer(2, GL_FLOAT, sizeof(SpriteVertex), vx_offset);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, this->batch_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Texture* SpriteBatch::set_texture(const void *const texture)
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
	unsigned int offset = 0;

	if (this->sprites.size() > 0)
	{
		offset = this->align(this->sprites.size());
		if (offset >= this->reserved)
		{
			this->reserved <<= 1;
			SpriteVertex *batch = new SpriteVertex[this->reserved];
			memcpy(batch, this->vertex_buffer, offset * sizeof(SpriteVertex));
			delete[] this->vertex_buffer;
			this->vertex_buffer = batch;

			this->sprites[0]->vertex_array = batch;
			for (unsigned int i = 1; i < this->sprites.size(); ++i)
				this->sprites[i]->vertex_array = this->vertex_buffer + this->align(i);
		}
	}

	// Assign the batch's buffer to the sprite
	s->vertex_array = &this->vertex_buffer[offset];
	this->sprites.push_back(s);

	// Update constants
	this->batch_vertices = (this->sprites.size() << 2) + ((this->sprites.size() - 1) << 1);
	this->batch_buffer_sz = this->batch_vertices * sizeof(SpriteVertex);
}

void SpriteBatch::update()
{
	// Update all sprite vertices
	this->sprites[0]->update();
	for (unsigned int i = 1; i < this->sprites.size(); ++i)
	{
		this->sprites[i]->update();
		unsigned int offset = this->align(i);
		this->vertex_buffer[offset - 2] = this->vertex_buffer[offset - 3];
		this->vertex_buffer[offset - 1] = this->vertex_buffer[offset];
	}

	// Update vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
	glBufferData(GL_ARRAY_BUFFER, this->batch_buffer_sz, this->vertex_buffer, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
