#ifndef INPUT_TOUCHABLE_H_
#define INPUT_TOUCHABLE_H_

struct Touch;

/// Interface for touchable objects.
///
/// This interface can be used for both touch and mouse inputs.
///
/// Copyright 2010-13 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
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
	virtual void touch_began_impl(const Touch *const touches, const size_t count) = 0;
	virtual void touch_canceled_impl() = 0;
	virtual void touch_ended_impl(const Touch *const touches, const size_t count) = 0;
	virtual void touch_moved_impl(const Touch *const touches, const size_t count) = 0;
};

void Touchable::touch_began(const Touch *const touches, const size_t count)
{
	this->touch_began_impl(touches, count);
}

void Touchable::touch_canceled()
{
	this->touch_canceled_impl();
}

void Touchable::touch_ended(const Touch *const touches, const size_t count)
{
	this->touch_ended_impl(touches, count);
}

void Touchable::touch_moved(const Touch *const touches, const size_t count)
{
	this->touch_moved_impl(touches, count);
}

#endif
