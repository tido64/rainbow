/*
 *  Rain.cpp
 *  OnWire
 *
 *  Created by Tommy Nguyen on 12/18/10.
 *  Copyright 2010 Bifrost Games. All rights reserved.
 *
 */

#include "Rain.h"

namespace Rainbow
{
	namespace Rain
	{
		using Rainbow::mt_random;

		void ParticleActionPolicy::operator()(PointSprite *p, const unsigned int count, const unsigned int elapsed)
		{
			PointSprite *s = p;
			for (unsigned int i = 0; i < count; ++i)
			{
				p->position += p->vector;
				++p;
			}
			for (unsigned int i = 0; i < count; ++i)
			{
				if (s->position.x >= Screen::Instance().width() | s->position.y <= 0.0f)
					ParticleInitPolicy::init(s);
				++s;
			}
		}

		void ParticleInitPolicy::init(PointSprite *p)
		{
			p->size = mt_random() * 16;
			p->position.x = Screen::Instance().width() * 0.3f + Screen::Instance().width() * 0.6f * (mt_random() - 0.5f) * 2;
			p->position.y = Screen::Instance().height();
			p->vector.x = mt_random() * 2;
			p->vector.y = mt_random() * (p->size / 8) - 16;
			p->color.r = p->color.g = mt_random();
			p->color.b = mt_random() + 0.5f;
		}

		void ParticleInitPolicy::operator()(PointSprite *p, const unsigned int count)
		{
			for (unsigned int i = 0; i < count; ++i)
			{
				init(p);
				p->position.y *= mt_random();
				p->texcoord[0].x = 1.0f;
				p->texcoord[0].y = 0.0f;
				p->texcoord[1].x = 0.0f;
				p->texcoord[1].y = 0.0f;
				p->texcoord[2].x = 1.0f;
				p->texcoord[2].y = 1.0f;
				p->texcoord[3].x = 0.0f;
				p->texcoord[3].y = 1.0f;
				++p;
			}
		}
	}
}
