// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_GLOBAL_H_
#define COMMON_GLOBAL_H_

#include "Common/Debug.h"
#include "Common/NonCopyable.h"

template<typename T>
class Global : private NonCopyable<Global<T>>
{
public:
	static T* Get();

protected:
	Global() = default;
	~Global();

	bool is_global() const;
	void make_global();

private:
	static T *s_instance;
};

template<typename T>
T* Global<T>::Get()
{
	R_ASSERT(s_instance, "Instance not yet created or already destroyed");
	return s_instance;
}

template<typename T>
Global<T>::~Global()
{
	s_instance = nullptr;
}

template<typename T>
bool Global<T>::is_global() const
{
	return s_instance == static_cast<const T*>(this);
}

template<typename T>
void Global<T>::make_global()
{
	R_ASSERT(!s_instance, "An instance already exists");
	s_instance = static_cast<T*>(this);
}

template<typename T>
T *Global<T>::s_instance = nullptr;

#endif
