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
	/// \param touches  The id and position of each touch.
	/// \param count    Number of touches.
	bool on_touch_began(const Touch *const touches, const size_t count)
	{
		return on_touch_began_impl(touches, count);
	}

	/// Canceled touch event. May occur when the user moves finger off-screen
	/// or the application got interrupted by external events.
	bool on_touch_canceled()
	{
		return on_touch_canceled_impl();
	}

	/// User lifts finger; mouse up.
	/// \param touches  The id and position of each touch.
	/// \param count    Number of touches.
	bool on_touch_ended(const Touch *const touches, const size_t count)
	{
		return on_touch_ended_impl(touches, count);
	}

	/// User moves finger around while it still is on the device; mouse moved.
	/// \param touches  The id and position of each touch.
	/// \param count    Number of touches.
	bool on_touch_moved(const Touch *const touches, const size_t count)
	{
		return on_touch_moved_impl(touches, count);
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

	virtual bool on_touch_began_impl(const Touch *const, const size_t)
	{
		return false;
	}

	virtual bool on_touch_canceled_impl()
	{
		return false;
	}

	virtual bool on_touch_ended_impl(const Touch *const, const size_t)
	{
		return false;
	}

	virtual bool on_touch_moved_impl(const Touch *const, const size_t)
	{
		return false;
	}
};

#endif
