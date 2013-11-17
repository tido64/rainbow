// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_TERMINABLE_H_
#define COMMON_TERMINABLE_H_

/// Terminable classes control the lifetime of an application.
class Terminable
{
public:
	inline bool has_terminated() const;
	inline bool is_active() const;

	inline void activate();
	inline void deactivate();
	inline void terminate();

protected:
	inline Terminable();

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
