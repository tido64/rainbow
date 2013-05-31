#ifndef INPUT_H_
#define INPUT_H_

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
///
/// Copyright 2010-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Input : private NonCopyable<Input>
{
public:
	enum
	{
		KEY_EVENTS    = 1u << 0,
		TOUCH_EVENTS  = 1u << 1,
		ALL_EVENTS    = KEY_EVENTS | TOUCH_EVENTS
	};

	static Input *Instance;

	inline Input(lua_State *);

	/// Reset input subscription list.
	inline void reset();

	/// Subscribe to input events.
	/// \param t      The object that wants to subscribe.
	/// \param flags  Events to subscribe to.
	void subscribe(Touchable *const t, unsigned int flags);

	/// Unsubscribe the object from input events.
	/// \param t  The object to unsubscribe.
	inline void unsubscribe(Touchable *const t);

	/// Acceleration event.
	/// \param x,y,z  Acceleration data.
	/// \param t      The relative time at which the acceleration event occurred.
	void accelerated(const double x, const double y, const double z, const double t);

	/// Clear all events.
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
	lua_State *lua_state;
	Acceleration acceleration;  ///< Accelerometer data
	Vector<Touchable*> touch_subscribers;
};

Input::Input(lua_State *L) : lua_state(L) { }

void Input::reset()
{
	this->touch_subscribers.clear();
}

void Input::unsubscribe(Touchable *const t)
{
	this->touch_subscribers.qremove(t);
}

#endif
