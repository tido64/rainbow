// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef INPUT_INPUT_H_
#define INPUT_INPUT_H_

#include "Common/NonCopyable.h"
#include "Input/Acceleration.h"
#include "Input/InputListener.h"
#include "Platform/Macros.h"

class  Key;
struct lua_State;
struct Touch;

/// Handles accelerometer/gyroscope and touch events independent of platform.
///
/// Subscribable input events. There are no accelerometer events.
///
/// \see http://developer.apple.com/library/ios/#documentation/UIKit/Reference/UIAcceleration_Class/Reference/UIAcceleration.html
class Input : private InputListener, NonCopyable<Input>
{
public:
	inline explicit Input(lua_State *);

	/// Clears all input listeners.
	inline void reset();

	/// Subscribes an object to input events.
	/// \param i  The object to subscribe.
	void subscribe(InputListener *const i);

	/// Unsubscribes an object from input events.
	/// \params t  The object to unsubscribe.
	void unsubscribe(InputListener *const i);

	/// Acceleration event.
	/// \param x,y,z  Acceleration data.
	/// \param i      The relative time at which the acceleration event occurred.
	void accelerated(const double x, const double y, const double z, const double t);

	/// Clears all events.
	void clear();

#ifdef RAINBOW_BUTTONS
	void on_key_down(const Key &k);
	void on_key_up(const Key &k);
#endif

	void on_touch_began(Touch *const touches, const size_t count);
	void on_touch_canceled();
	void on_touch_ended(Touch *const touches, const size_t count);
	void on_touch_moved(Touch *const touches, const size_t count);

private:
	lua_State *lua_state_;
	Acceleration acceleration_;  ///< Accelerometer data
	InputListener *last_listener_;

	void on_end_link_removed(Link *node) override;
};

Input::Input(lua_State *L) : lua_state_(L), last_listener_(this) { }

void Input::reset()
{
	pop();
	last_listener_ = this;
}

#endif
