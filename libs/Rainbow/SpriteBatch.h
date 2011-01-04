/// A batch of sprites to be drawn.

/// All sprites share a common vertex buffer object (at different offsets) and
/// are drawn with a single glDraw call. The sprites must use the same texture
/// atlas.
///
/// It is also possible to instantiate a sprite batch with only one sprite. The
/// batch will then simply act as a buffer.
///
/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef SPRITEBATCH_H_
#define SPRITEBATCH_H_

#include <Rainbow/Sprite.h>

template<int N>
class SpriteBatch
{
public:
	SpriteBatch() : count(0)
	{
		assert(N > 0);
		glGenBuffers(1, &this->buffer);
	}

	void add(Sprite *s)
	{
		assert(this->count < N);

		unsigned int offset = 0;

		if (this->count == 0)
			this->texture = s->texture;
		else
		{
			// Make sure all the sprites use the same texture atlas
			assert(s->texture == this->texture);

			// Copy degenerate vertices
			offset = this->indexof(this->count);
			this->vertex_buffer[offset - 2] = this->vertex_buffer[offset - 3];
			this->vertex_buffer[offset - 1] = s->vertex_array[0];
		}

		// Delete the current vertex array and assign the batch's buffer
		memcpy(&this->vertex_buffer[offset], s->vertex_array, Sprite::vertex_array_sz);
		delete[] s->vertex_array;
		s->vertex_array = &this->vertex_buffer[offset];
		s->buffered = true;
		this->sprites[this->count++] = s;
	}

	void draw()
	{
		static const void *tex_offset = reinterpret_cast<float *>(0) + 2;

		// Enable all colour channels
		glColor4ub(0xff, 0xff, 0xff, 0xff);
		glBindTexture(GL_TEXTURE_2D, this->texture);

		glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
		glVertexPointer(2, GL_FLOAT, sizeof(SpriteVertex), 0);
		glTexCoordPointer(2, GL_FLOAT, sizeof(SpriteVertex), tex_offset);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, batch_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void update()
	{
		// Make sure we're not wasting memory, and also to make sure the loop
		// below is unrollable if needed
		assert(this->count == N);

		// Update all sprite vertices
		this->sprites[0]->update();
		for (int i = 1; i < N; ++i)
		{
			this->sprites[i]->update();
			unsigned int offset = this->indexof(i);
			this->vertex_buffer[offset - 2] = this->vertex_buffer[offset - 3];
			this->vertex_buffer[offset - 1] = this->vertex_buffer[offset];
		}

		// Update vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
		glBufferData(GL_ARRAY_BUFFER, batch_buffer_sz, this->vertex_buffer, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

protected:
	static const unsigned int batch_vertices = N * 4 + (N - 1) * 2;
	static const unsigned int batch_buffer_sz = batch_vertices * sizeof(SpriteVertex);

	inline unsigned int indexof(const unsigned int i)
	{
		return (i << 2) + (i << 1);
	}

private:
	int count;
	GLuint buffer;
	GLuint texture;
	Sprite *sprites[N];
	SpriteVertex vertex_buffer[batch_vertices];
};

#endif
