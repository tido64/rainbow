#include <Rainbow/Input/Gestures.h>

namespace Rainbow
{
	namespace Gestures
	{
		void Pinch::operator()()
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
	}
}
