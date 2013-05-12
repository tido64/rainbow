#ifndef TERMINABLE_H_
#define TERMINABLE_H_

/// Terminable classes control the lifetime of an application.
///
/// Copyright 2013 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Terminable
{
public:
	inline Terminable();

	inline bool has_terminated() const;
	inline bool is_active() const;

	inline void activate();
	inline void deactivate();
	inline void terminate();

private:
	bool active;
	bool terminated;
};

Terminable::Terminable() : active(true), terminated(false) { }

bool Terminable::has_terminated() const
{
	return this->terminated;
}

bool Terminable::is_active() const
{
	return this->active && !this->terminated;
}

void Terminable::activate()
{
	this->active = true;
}

void Terminable::deactivate()
{
	this->active = false;
}

void Terminable::terminate()
{
	this->deactivate();
	this->terminated = true;
}

#endif
