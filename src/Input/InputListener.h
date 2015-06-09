// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef INPUT_INPUTLISTENER_H_
#define INPUT_INPUTLISTENER_H_

#include "Common/Link.h"

struct Key;
struct Pointer;

/// Interface for input listeners.
class InputListener : private Link
{
	friend class Input;

public:
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

	bool on_key_down(const Key &k)
	{
		return on_key_down_impl(k);
	}

	bool on_key_up(const Key &k)
	{
		return on_key_up_impl(k);
	}

	/// User puts finger down on the device; mouse down.
	/// \param count     Number of pointers.
	/// \param pointers  The id and position of each pointer.
	bool on_pointer_began(const unsigned int count, const Pointer *pointers)
	{
		return on_pointer_began_impl(count, pointers);
	}

	/// Canceled pointer event. May occur when the user moves finger off-screen
	/// or the application got interrupted by external events.
	bool on_pointer_canceled()
	{
		return on_pointer_canceled_impl();
	}

	/// User lifts finger; mouse up.
	/// \param count     Number of pointers.
	/// \param pointers  The id and position of each pointer.
	bool on_pointer_ended(const unsigned int count, const Pointer *pointers)
	{
		return on_pointer_ended_impl(count, pointers);
	}

	/// User moves finger around while it still is on the device; mouse moved.
	/// \param count     Number of pointers.
	/// \param pointers  The id and position of each pointer.
	bool on_pointer_moved(const unsigned int count, const Pointer *pointers)
	{
		return on_pointer_moved_impl(count, pointers);
	}

protected:
	~InputListener() = default;

private:
	virtual bool on_key_down_impl(const Key &) { return false; }
	virtual bool on_key_up_impl(const Key &) { return false; }

	virtual bool on_pointer_began_impl(const unsigned int, const Pointer *)
	{
		return false;
	}

	virtual bool on_pointer_canceled_impl()
	{
		return false;
	}

	virtual bool on_pointer_ended_impl(const unsigned int, const Pointer *)
	{
		return false;
	}

	virtual bool on_pointer_moved_impl(const unsigned int, const Pointer *)
	{
		return false;
	}
};

#endif
