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
/// Copyright 2010-11 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class Input
{
public:
	static Input& Instance();

	bool accelerated;           ///< Whether we should update accelerometer data
	Acceleration acceleration;  ///< Accelerometer data

	/// Reset input subscription list.
	void reset();

	/// Set screen height. Needed to properly flip the image vertically.
	void set_height(const int height);

	/// Set active Lua state.
	void set_state(lua_State *L);

	/// Subscribe to input events.
	/// \param t      The object that wants to subscribe
	/// \param flags  Events to subscribe to
	void subscribe(Touchable *const t, unsigned int flags);

#ifdef RAINBOW_BUTTONS
	void key_down(const Key &k);
	void key_up(const Key &k);
#endif

	void touch_began(Touch *const touches, const unsigned int count);
	void touch_canceled();
	void touch_ended(Touch *const touches, const unsigned int count);
	void touch_moved(Touch *const touches, const unsigned int count);

private:
	int height;
	lua_State *lua_state;
	Vector<Touchable *> touch_subscribers;

	Input();

	/// Intentionally left undefined.
	Input(const Input &);

	/// The coordinates system on most devices are "flipped". This function
	/// will "correct" the values.
	void touch_flip(Touch *const touches, const unsigned int count);

	/// Intentionally left undefined.
	Input& operator=(const Input &);
};

inline Input& Input::Instance()
{
	static Input i;
	return i;
}

inline Input::Input() : height(0), lua_state(nullptr) { }

inline void Input::reset()
{
	this->touch_subscribers.clear();
}

inline void Input::set_height(const int h)
{
	this->height = h;
}

inline void Input::set_state(lua_State *L)
{
	this->lua_state = L;
}

#endif
