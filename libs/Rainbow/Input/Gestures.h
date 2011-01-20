/// Handles touch gestures.

#ifndef GESTURES_H_
#define GESTURES_H_

#include <Rainbow/Hardware/Controls.h>
#include <Rainbow/Input/Touch.h>
#include <Rainbow/Algorithm.h>

namespace Rainbow
{
	namespace Gestures
	{
		static const Touches *touches = Controls::Instance()->get_touches();

		class Pinch
		{
		public:
			float angle;
			float scale;

			Pinch() : angle(0.0f), scale(1.0f), angle_base(0.0f), scale_base(0.0f) { }

			void operator()()
			{
				if (touches->size() == 2)
				{
					Touches::const_iterator i = touches->cbegin();
					const Vec2i &a = i->second.position;
					const Vec2i &b = (++i)->second.position;

					if (equalf(this->scale_base, 0.0f))
					{
						this->angle_base = a.angle(b);
						this->scale_base = a.distance(b);
						if (equalf(this->scale_base, 0.0f))
							this->scale_base = 1.0f;
					}
					else
					{
						this->angle = this->angle_base - a.angle(b);
						this->scale = a.distance(b) / this->scale_base;
					}
				}
				else
				{
					this->angle = 0.0f;
					this->scale = 1.0f;
					this->scale_base = 0.0f;
				}
			}

		private:
			float angle_base;
			float scale_base;
		};
	}
}

#endif
