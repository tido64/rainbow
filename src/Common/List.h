// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_LIST_H_
#define COMMON_LIST_H_

#include <functional>

#include "Memory/SharedPtr.h"

/// A functional list.
///
/// Based on Bartosz Milewski's <bartosz@relisoft.com> port of functional data
/// structures to C++, whose work is based on Chris Okasaki's "Purely Functional
/// Data Structures". Our implementation uses the intrusive \c SharedPtr instead
/// of \c std::shared_ptr.
///
/// \see https://github.com/BartoszMilewski/Okasaki
/// \see http://www.cambridge.org/us/academic/subjects/computer-science/algorithmics-complexity-computer-algebra-and-computational-g/purely-functional-data-structures
/// \see http://www.cs.cmu.edu/~rwh/theses/okasaki.pdf
template<typename T>
class List : private NonCopyable<List<T>>
{
	struct Item : public RefCounted
	{
		const T value;
		const SharedPtr<Item> next;

		Item(T&& value, SharedPtr<Item> next)
		    : value(std::forward<T>(value)), next(std::move(next)) {}
	};

public:
	List() = default;

	List(List&& list) : head_(std::move(list.head_)) {}

	bool empty() const
	{
		return !head_;
	}

	const T& front() const
	{
		return head_->value;
	}

	List insert(const size_t i, T&& value) const
	{
		if (i == 0)
			return push_front(std::forward<T>(value));
		return List(front(), pop_front().insert(i - 1, std::forward<T>(value)));
	}

	List pop_front() const
	{
		return List(head_->next);
	}

	List push_back(T&& value) const
	{
		if (empty())
			return push_front(std::forward<T>(value));
		return List(front(), pop_front().push_back(std::forward<T>(value)));
	}

	List push_front(T&& value) const
	{
		return List(std::forward<T>(value), *this);
	}

	List& operator=(List&& list)
	{
		head_ = std::move(list.head_);
		return *this;
	}

private:
	SharedPtr<Item> head_;

	explicit List(SharedPtr<Item> items) : head_(std::move(items)) {}

	List(T&& value, const List &tail)
	    : head_(new Item(std::forward<T>(value), tail.head_)) {}
};

template<typename T, typename F>
void for_each(const List<T> &list, F&& f)
{
	static_assert(std::is_convertible<F, std::function<void(T)>>::value,
	              "for_each() requires a function type void(T)");

	if (list.empty())
		return;

	f(list.front());
	for_each(list.pop_front(), f);
}

#endif
