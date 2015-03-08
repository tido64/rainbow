// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef MEMORY_ARENA_H_
#define MEMORY_ARENA_H_

#include <iterator>
#include <memory>
#include <new>

#include "Common/NonCopyable.h"

template<typename T>
class Arena : private NonCopyable<Arena<T>>
{
public:
	Arena();
	Arena(Arena &&);
	~Arena();

	/// Returns the pointer to the arena.
	T* get() const;

	/// Releases \p count elements.
	void release(size_t count) const;

	/// Resizes arena to hold \p new_count elements and moves \p old_count
	/// elements over.
	/// \param old_count  Number of elements to keep.
	/// \param new_count  Number of elements to allocate for.
	void resize(const size_t old_count, const size_t new_count);

	/// Returns whether this arena is valid.
	explicit operator bool() const;

	T& operator[](const size_t i) const;
	T* operator->() const;
	T* operator+(const size_t offset) const;
	T& operator*() const;

private:
	T *arena_;
};

template<typename T>
Arena<T>::Arena() : arena_(nullptr) {}

template<typename T>
Arena<T>::Arena(Arena &&a) : arena_(a.arena_)
{
	a.arena_ = nullptr;
}

template<typename T>
Arena<T>::~Arena()
{
	operator delete(arena_);
}

template<typename T>
T* Arena<T>::get() const
{
	return arena_;
}

template<typename T>
void Arena<T>::release(size_t count) const
{
	if (!arena_)
		return;
	while (count > 0)
		arena_[--count].~T();
}

template<typename T>
void Arena<T>::resize(const size_t old_count, const size_t new_count)
{
	T *new_arena = static_cast<T*>(operator new(new_count * sizeof(T)));
	if (old_count > 0)
	{
		std::uninitialized_copy_n(
		    std::make_move_iterator(arena_), old_count, new_arena);
	}
	operator delete(arena_);
	arena_ = new_arena;
}

template<typename T>
Arena<T>::operator bool() const
{
	return arena_;
}

template<typename T>
T& Arena<T>::operator[](const size_t i) const
{
	return arena_[i];
}

template<typename T>
T* Arena<T>::operator->() const
{
	return arena_;
}

template<typename T>
T* Arena<T>::operator+(const size_t offset) const
{
	return arena_ + offset;
}

template<typename T>
T& Arena<T>::operator*() const
{
	return *arena_;
}

#endif
