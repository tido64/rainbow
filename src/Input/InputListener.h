// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef INPUT_INPUTLISTENER_H_
#define INPUT_INPUTLISTENER_H_

#include "Common/Link.h"
#include "Platform/Macros.h"

class Key;
struct Touch;

/// Interface for input listeners.
class InputListener : private Link
{
	friend class Input;

public:
	virtual ~InputListener() = default;

	/// Returns the next input listener; \c nullptr if none.
	InputListener* next() const
	{
		return static_cast<InputListener*>(Link::next());
	}

	/// Returns the previous input listener; \c nullptr if none.
	InputListener* prev() const
	{
		return static_cast<InputListener*>(Link::prev());
	}

#ifdef RAINBOW_BUTTONS
	bool on_key_down(const Key &k)
	{
		return on_key_down_impl(k);
	}

	bool on_key_up(const Key &k)
	{
		return on_key_up_impl(k);
	}
#endif

	/// User puts finger down on the device; mouse down.
	/// \param count    Number of touches.
	/// \param touches  The id and position of each touch.
	bool on_touch_began(const unsigned int count, const Touch *const touches)
	{
		return on_touch_began_impl(count, touches);
	}

	/// Canceled touch event. May occur when the user moves finger off-screen
	/// or the application got interrupted by external events.
	bool on_touch_canceled()
	{
		return on_touch_canceled_impl();
	}

	/// User lifts finger; mouse up.
	/// \param count    Number of touches.
	/// \param touches  The id and position of each touch.
	bool on_touch_ended(const unsigned int count, const Touch *const touches)
	{
		return on_touch_ended_impl(count, touches);
	}

	/// User moves finger around while it still is on the device; mouse moved.
	/// \param count    Number of touches.
	/// \param touches  The id and position of each touch.
	bool on_touch_moved(const unsigned int count, const Touch *const touches)
	{
		return on_touch_moved_impl(count, touches);
	}

private:
#ifdef RAINBOW_BUTTONS
	virtual bool on_key_down_impl(const Key &)
	{
		return false;
	}

	virtual bool on_key_up_impl(const Key &)
	{
		return false;
	}
#endif

	virtual bool on_touch_began_impl(const unsigned int, const Touch *const)
	{
		return false;
	}

	virtual bool on_touch_canceled_impl()
	{
		return false;
	}

	virtual bool on_touch_ended_impl(const unsigned int, const Touch *const)
	{
		return false;
	}

	virtual bool on_touch_moved_impl(const unsigned int, const Touch *const)
	{
		return false;
	}
};

#endif
