/// Rainbow particle system.

/// Hint: When removing a "dead" particle, swap it with the last active
/// particle, and decrement particle count.
///
/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef PARTICLESYSTEM_H_
#define PARTICLESYSTEM_H_

#include "../OpenGL.h"
#include "../Texture.h"
#include "PointSprite.h"

template<class ParticleInitPolicy, class ParticleActionPolicy, const int max_particles>
class ParticleSystem
{
public:

	ParticleSystem() : active(true), count(0), elapsed(0)
	{
		glGenBuffers(1, &this->vbo_name);
	}

	void emit(unsigned int n = 1)
	{
		if (this->count >= max_particles) return;

		if (this->count + n > max_particles)
			n = max_particles - this->count;

		this->init(&this->particles[this->count], n);
		this->count += n;
	}

	void draw()
	{
		static const Texture texture("mobpig.png");
		static const void *color_offset = reinterpret_cast<float *>(0) + 5;
		static const void *tex_offset = reinterpret_cast<float *>(0) + 9;
		static const void *vertex_offset = reinterpret_cast<float *>(0) + 1;
		if (!this->active) return;

		glEnable(GL_POINT_SPRITE);

	#ifdef GL_VERSION_ES_CM_1_1
		glEnableClientState(GL_POINT_SIZE_ARRAY_OES);
	#else
		//glEnableClientState(GL_POINT_SIZE_ARRAY);
	#endif

		glEnableClientState(GL_COLOR_ARRAY);

		glBindTexture(GL_TEXTURE_2D, texture.name);
		glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo_name);
		glColorPointer(4, GL_FLOAT, sizeof(PointSprite), color_offset);

	#ifdef GL_VERSION_ES_CM_1_1
		glPointSizePointerOES(GL_FLOAT, sizeof(PointSprite), 0);
	#else
		glPointSize(8.0f);
	#endif

		glTexCoordPointer(2, GL_FLOAT, sizeof(PointSprite), tex_offset);
		glVertexPointer(2, GL_FLOAT, sizeof(PointSprite), vertex_offset);
		glDrawArrays(GL_POINTS, 0, this->count);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDisableClientState(GL_COLOR_ARRAY);

	#ifdef GL_VERSION_ES_CM_1_1
		glDisableClientState(GL_POINT_SIZE_ARRAY_OES);
	#else
		//glDisableClientState(GL_POINT_SIZE_ARRAY);
	#endif

		glDisable(GL_POINT_SPRITE);
	}

	void update()
	{
		if (!this->active) return;

		if (this->count == 0)
		{
			this->active = false;
			this->elapsed = 0;
			return;
		}

		this->action(this->particles, this->count, this->elapsed++);
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo_name);
		glBufferData(GL_ARRAY_BUFFER, this->count * sizeof(PointSprite), this->particles, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

private:
	bool active;
	unsigned int count;
	unsigned int elapsed;
	GLuint vbo_name;
	PointSprite particles[max_particles];

	ParticleInitPolicy init;
	ParticleActionPolicy action;
};

#endif
