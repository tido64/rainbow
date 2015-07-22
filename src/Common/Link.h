// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_LINK_H_
#define COMMON_LINK_H_

#include "Common/Constraints.h"

class Link
{
public:
	Link() : next_(nullptr), prev_(nullptr) {}

	Link* next() const { return next_; }
	Link* prev() const { return prev_; }

	void append(Link *node);
	void pop();

	template<typename T, typename F, typename = EnableIfBaseOf<Link, T>>
	friend bool for_each(T *node, F&& f)
	{
		while (node)
		{
			if (f(node))
				return true;
			node = node->next();
		}
		return false;
	}

protected:
	~Link() { pop(); }

private:
	Link *next_;
	Link *prev_;

	virtual void on_end_link_removed(Link *node);
};

#endif
