// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifdef USE_HEIMDALL

#include "Heimdall/OverlayActivator.h"

#include <algorithm>

#include "Heimdall/Overlay.h"
#include "Input/Touch.h"

namespace
{
	const int kTimeTillActivation = 2000;
}

namespace heimdall
{
	void OverlayActivator::reset()
	{
		resistance_ = 2;
		time_till_activation_ = kTimeTillActivation;
		touches_[0] = -1;
		touches_[1] = -1;
	}

	void OverlayActivator::update(const unsigned long dt)
	{
		if (resistance_ == 0)
		{
			time_till_activation_ =
			    std::max(time_till_activation_ - static_cast<int>(dt), 0);
			if (time_till_activation_ == 0)
				overlay_->show();
		}
	}

	bool OverlayActivator::on_touch_began_impl(const unsigned int count,
	                                           const Touch *const touches)
	{
		R_ASSERT(!overlay_->is_visible(), "Overlay is leaking touch events");

		int i = 0;
		switch (resistance_)
		{
			case 2:
				touches_[0] = touches[0].hash;
				--resistance_;
				if (count < 2)
					break;
				i = 1;
				// fall through
			case 1:
				touches_[1] = touches[i].hash;
				--resistance_;
				time_till_activation_ = kTimeTillActivation;
				break;
			default:
				break;
		}
		return false;
	}

	bool OverlayActivator::on_touch_canceled_impl()
	{
		const bool prevent_propagation = is_activated();
		reset();
		return prevent_propagation;
	}

	bool OverlayActivator::on_touch_ended_impl(const unsigned int count,
	                                           const Touch *const touches)
	{
		const bool prevent_propagation = is_activated();
		for (unsigned int i = 0; i < count; ++i)
		{
			if (touches[i].hash == touches_[0])
			{
				touches_[0] = touches_[1];
				touches_[1] = -1;
				++resistance_;
			}
			else if (touches[i].hash == touches_[1])
			{
				touches_[1] = -1;
				++resistance_;
			}
		}
		return prevent_propagation;
	}
}

#endif
