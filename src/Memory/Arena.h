// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
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
	Arena() : arena_(nullptr) {}
	Arena(Arena&& a) : arena_(a.arena_) { a.arena_ = nullptr; }
	~Arena() { operator delete(arena_); }

	/// <summary>Returns the pointer to the arena.</summary>
	T* get() const { return arena_; }

	/// <summary>Releases <paramref name="count"/> elements.</summary>
	void release(size_t count) const
	{
		if (!arena_)
			return;
		while (count > 0)
			arena_[--count].~T();
	}

	/// <summary>
	///   Resizes arena to hold <paramref name="new_count"/> elements and moves
	///   <paramref name="old_count"/> elements over.
	/// </summary>
	/// <param name="old_count">Number of elements to keep.</param>
	/// <param name="new_count">Number of elements to allocate for.</param>
	void resize(const size_t old_count, const size_t new_count)
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

	/// <summary>Returns whether this arena is valid.</summary>
	explicit operator bool() const { return arena_; }

	T& operator[](const size_t i) const { return arena_[i]; }
	T* operator->() const { return arena_; }
	T* operator+(const size_t offset) const { return arena_ + offset; }
	T& operator*() const { return *arena_; }

private:
	T *arena_;
};

#endif
