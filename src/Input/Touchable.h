// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef INPUT_TOUCHABLE_H_
#define INPUT_TOUCHABLE_H_

struct Touch;

/// Interface for touchable objects.
///
/// This interface can be used for both touch and mouse inputs.
class Touchable
{
public:
	virtual ~Touchable() = default;

	/// User puts finger down on the device.
	/// \param touches  The id and position of each touch
	/// \param count    Number of touches
	inline void touch_began(const Touch *const touches, const size_t count);

	/// Canceled touch event. May occur when the user moves finger off-screen
	/// or the application got interrupted by external events.
	inline void touch_canceled();

	/// User lifts finger.
	/// \param touches  The id and position of each touch
	/// \param count    Number of touches
	inline void touch_ended(const Touch *const touches, const size_t count);

	/// User moves finger around while it still is on the device.
	/// \param touches  The id and position of each touch
	/// \param count    Number of touches
	inline void touch_moved(const Touch *const touches, const size_t count);

private:
	virtual void touch_began_impl(const Touch *const, const size_t) { }
	virtual void touch_canceled_impl() { }
	virtual void touch_ended_impl(const Touch *const, const size_t) { }
	virtual void touch_moved_impl(const Touch *const, const size_t) { }
};

void Touchable::touch_began(const Touch *const touches, const size_t count)
{
	touch_began_impl(touches, count);
}

void Touchable::touch_canceled()
{
	touch_canceled_impl();
}

void Touchable::touch_ended(const Touch *const touches, const size_t count)
{
	touch_ended_impl(touches, count);
}

void Touchable::touch_moved(const Touch *const touches, const size_t count)
{
	touch_moved_impl(touches, count);
}

#endif
