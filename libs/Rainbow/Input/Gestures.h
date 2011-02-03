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

			/// Detects two finger gesture and updates angle and scale.
			void operator()();

		private:
			float angle_base;
			float scale_base;
		};
	}
}

#endif
