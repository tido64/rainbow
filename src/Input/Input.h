#ifndef INPUT_H_
#define INPUT_H_

#include "Common/Vector.h"
#include "Input/Acceleration.h"

// Rainbow input events
#define RAINBOW_ALL_EVENTS    -1
#define RAINBOW_KEY_EVENTS    (1 << 0)
#define RAINBOW_TOUCH_EVENTS  (1 << 1)

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
class Input
{
public:
	static inline Input& Instance();

	/// Reset input subscription list.
	inline void reset();

	/// Set active Lua state.
	inline void set_state(lua_State *L);

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

#ifdef RAINBOW_BUTTONS
	void key_down(const Key &k);
	void key_up(const Key &k);
#endif

	void touch_began(Touch *const touches, const unsigned int count);
	void touch_canceled();
	void touch_ended(Touch *const touches, const unsigned int count);
	void touch_moved(Touch *const touches, const unsigned int count);

private:
	lua_State *lua_state;
	Acceleration acceleration;  ///< Accelerometer data
	Vector<Touchable*> touch_subscribers;

	inline Input();

	/// Intentionally left undefined.
	Input(const Input &);

	/// Intentionally left undefined.
	Input& operator=(const Input &);
};

Input& Input::Instance()
{
	static Input i;
	return i;
}

void Input::reset()
{
	this->touch_subscribers.clear();
}

void Input::set_state(lua_State *L)
{
	this->lua_state = L;
}

void Input::unsubscribe(Touchable *const t)
{
	this->touch_subscribers.quick_remove(t);
}

Input::Input() : lua_state(nullptr) { }

#endif
