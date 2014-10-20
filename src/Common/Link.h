// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_LINK_H_
#define COMMON_LINK_H_

class Link
{
public:
	inline Link();

	inline Link* next() const;
	inline Link* prev() const;

	void append(Link *node);
	void pop();

	template<typename U, typename F>
	friend bool for_each(U *node, F&& f);

protected:
	inline ~Link();

private:
	Link *next_;
	Link *prev_;

	virtual void on_end_link_removed(Link *node);
};

Link::Link() : next_(nullptr), prev_(nullptr) { }

Link* Link::next() const
{
	return next_;
}

Link* Link::prev() const
{
	return prev_;
}

Link::~Link()
{
	pop();
}

template<typename T, typename F>
bool for_each(T *node, F&& f)
{
	while (node)
	{
		if (f(node))
			return true;
		node = node->next();
	}
	return false;
}

#endif
