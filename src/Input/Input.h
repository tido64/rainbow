// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef INPUT_INPUT_H_
#define INPUT_INPUT_H_

#include "Common/NonCopyable.h"
#include "Input/Acceleration.h"
#include "Input/InputListener.h"

/// Handles accelerometer/gyroscope and pointer events independent of platform.
///
/// Accelerometer data changes do not follow the observer pattern because they
/// happen every frame. Polling is preferred in this case.
///
/// \see http://developer.apple.com/library/ios/#documentation/UIKit/Reference/UIAcceleration_Class/Reference/UIAcceleration.html
class Input : private InputListener, NonCopyable<Input>
{
public:
	Input() : last_listener_(this) {}

	/// Clears all input listeners.
	void reset()
	{
		pop();
		last_listener_ = this;
	}

	/// Subscribes an object to input events.
	/// \param i  The object to subscribe.
	void subscribe(InputListener *const i);

	/// Unsubscribes an object from input events.
	/// \params t  The object to unsubscribe.
	void unsubscribe(InputListener *const i);

	/// Acceleration event.
	/// \param x,y,z  Acceleration data.
	/// \param t      The relative time at which the acceleration occurred.
	void accelerated(const double x,
	                 const double y,
	                 const double z,
	                 const double t);

	void on_key_down(const Key &k);
	void on_key_up(const Key &k);

	void on_pointer_began(const unsigned int count, Pointer *pointers);
	void on_pointer_canceled();
	void on_pointer_ended(const unsigned int count, Pointer *pointers);
	void on_pointer_moved(const unsigned int count, Pointer *pointers);

private:
	Acceleration acceleration_;  ///< Accelerometer data
	InputListener *last_listener_;

	void on_end_link_removed(Link *node) override;
};

#endif
