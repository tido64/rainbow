#ifndef LIST_H_
#define LIST_H_

#include <cassert>
#include <cstring>

/// Simple double-linked list container, mimicking STL::list.
///
/// This class is mainly for the lack of full STL implementation on some
/// platforms, such as Android.
///
/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
template<class T>
class List
{
public:
	class Iterator;

	class Element
	{
	private:
		Element *next;  ///< Pointer to the next element
		Element *prev;  ///< Pointer to the previous element
		T value;        ///< Stored value

		Element(const T &v, Element *const p = 0, Element *const n = 0) : next(n), prev(p), value(v) { }
		~Element() { delete this->next; }

		friend class List;
		friend class List::Iterator;
	};

	class Iterator
	{
	public:
		Iterator() : ptr(0) { }
		Iterator(const Iterator &i) : ptr(i.ptr) { }

		bool operator==(const Iterator &i) const { return this->ptr == i.ptr; }

		bool operator!=(const Iterator &i) const { return this->ptr != i.ptr; }

		Iterator& operator++()
		{
			this->ptr = this->ptr->next;
			return *this;
		}

		Iterator operator++(int)
		{
			Iterator i(*this);
			this->ptr = this->ptr->next;
			return i;
		}

		Iterator& operator--()
		{
			this->ptr = this->ptr->prev;
			return *this;
		}

		Iterator operator--(int)
		{
			Iterator i(*this);
			this->ptr = this->ptr->prev;
			return i;
		}

		T& operator*() const { return this->ptr->value; }

		Element* operator->() const { return this->ptr; }

	private:
		Element *ptr;

		Iterator(Element *const p) : ptr(p) { }

		friend class List;
	};

	List() : count(0), first(0), last(0)  { }
	~List() { delete this->first; }

	/// Return iterator to the beginning.
	Iterator begin() const { return Iterator(this->first); }

	/// Return iterator to the end.
	Iterator end() const { return Iterator(); }

	/// Insert an item at the back of the list.
	void push_back(const T &value)
	{
		++count;
		if (!this->first)
		{
			this->first = new Element(value);
			this->last = this->first;
			return;
		}
		this->last->next = new Element(value, this->last);
		this->last = this->last->next;
	}

	/// Remove elements with specific value.
	void remove(const T &value)
	{
		Element *e = this->first;
		while (e)
		{
			if (e->value == value)
			{
				Element *next = e->next;
				if (e->prev)
					e->prev->next = e->next;
				if (e->next)
				{
					e->next->prev = e->prev;
					e->next = 0;
				}
				else
					this->last = e->prev;
				delete e;
				e = next;
				--count;
			}
			else
				e = e->next;
		}
	}

	/// Return size of list.
	inline unsigned int size() { return this->count; }

private:
	unsigned int count;  ///< Element count
	Element *first;      ///< Pointer to the first element
	Element *last;       ///< Pointer to the last element
};

#endif
