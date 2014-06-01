// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef INPUT_INPUT_H_
#define INPUT_INPUT_H_

#include "Common/Vector.h"
#include "Input/Acceleration.h"

class  Key;
struct lua_State;
struct Touch;
class  Touchable;

/// Handles accelerometer/gyroscope and touch events independent of platform.
///
/// Subscribable input events. There are no accelerometer events.
///
/// \see http://developer.apple.com/library/ios/#documentation/UIKit/Reference/UIAcceleration_Class/Reference/UIAcceleration.html
class Input : private NonCopyable<Input>
{
public:
	struct Events
	{
		static const unsigned int Key    = 1u << 0;
		static const unsigned int Touch  = 1u << 1;
		static const unsigned int All    = Key | Touch;
	};

	inline explicit Input(lua_State *);

	/// Resets input subscription list.
	inline void reset();

	/// Subscribes to input events.
	/// \param t       The object that wants to subscribe.
	/// \param events  Events to subscribe to.
	void subscribe(Touchable *const t, const unsigned int events);

	/// Unsubscribes the object from input events.
	/// \params t  The object to unsubscribe.
	inline void unsubscribe(Touchable *const t);

	/// Acceleration event.
	/// \param x,y,z  Acceleration data.
	/// \param t      The relative time at which the acceleration event occurred.
	void accelerated(const double x, const double y, const double z, const double t);

	/// Clears all events.
	void clear();

#ifdef RAINBOW_BUTTONS
	void key_down(const Key &k);
	void key_up(const Key &k);
#endif

	void touch_began(Touch *const touches, const size_t count);
	void touch_canceled();
	void touch_ended(Touch *const touches, const size_t count);
	void touch_moved(Touch *const touches, const size_t count);

private:
	lua_State *lua_state_;
	Acceleration acceleration_;  ///< Accelerometer data
	Vector<Touchable*> touch_subscribers_;
};

Input::Input(lua_State *L) : lua_state_(L) { }

void Input::reset()
{
	touch_subscribers_.clear();
}

void Input::unsubscribe(Touchable *const t)
{
	touch_subscribers_.remove(t);
}

#endif
