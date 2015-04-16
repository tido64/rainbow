// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef HEIMDALL_OVERLAYACTIVATOR_H_
#define HEIMDALL_OVERLAYACTIVATOR_H_

#ifdef USE_HEIMDALL

#include "Input/InputListener.h"

namespace heimdall
{
	class Overlay;

	class OverlayActivator final : public InputListener
	{
	public:
		inline OverlayActivator(Overlay *);

		inline bool is_activated() const;

		void reset();
		void update(const unsigned long dt);

	private:
		int resistance_;
		int time_till_activation_;
		unsigned int touches_[2];
		Overlay *overlay_;

		/* Implement InputListener */

		bool on_touch_began_impl(const unsigned int,
		                         const Touch *const) override;
		bool on_touch_canceled_impl() override;
		bool on_touch_ended_impl(const unsigned int,
		                         const Touch *const) override;
	};

	OverlayActivator::OverlayActivator(Overlay *overlay) : overlay_(overlay)
	{
		reset();
	}

	bool OverlayActivator::is_activated() const
	{
		return time_till_activation_ == 0 && resistance_ < 2;
	}
}

#endif  // USE_HEIMDALL
#endif  // HEIMDALL_OVERLAYACTIVATOR_H_
