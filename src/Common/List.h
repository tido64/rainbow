#ifndef LIST_H_
#define LIST_H_

#include "Common/Debug.h"
#include "Common/NonCopyable.h"

/// Simple double-linked list container, mimicking STL::list.
///
/// This class is mainly for the lack of full STL implementation on some
/// platforms, such as Android.
///
/// Copyright 2011-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
template<class T>
class List : public NonCopyable<List<T> >
{
public:
	class Iterator;

	inline List();
	~List();

	/// Return iterator to the beginning.
	inline Iterator begin();

	/// Return iterator to the end.
	inline Iterator end();

	/// Insert an item at the back of the list.
	void push_back(const T &value);

	/// Remove elements with specific value.
	/// \param value  The value to remove from the list.
	/// \param count  Number of elements to remove from the list.
	/// \return Number of elements removed.
	size_t remove(const T &value, size_t count = 1);

	/// Return size of list.
	inline size_t size();

private:
	class Element;

	size_t count;    ///< Element count
	Element *first;  ///< Pointer to the first element
	Element *last;   ///< Pointer to the last element
};

template<class T>
class List<T>::Element
{
	friend class List;
	friend class List::Iterator;

public:
	void swap(Element *e);

private:
	Element *next;  ///< Pointer to the next element
	Element *prev;  ///< Pointer to the previous element
	T value;        ///< Stored value

	inline Element(const T &v, Element *const p = 0, Element *const n = 0);
};

template<class T>
class List<T>::Iterator
{
	friend class List;

public:
	void swap(Iterator &i);

	inline operator bool() const;

	inline Iterator& operator++();
	inline Iterator& operator++(int);
	inline Iterator& operator--();
	inline Iterator& operator--(int);
	inline T& operator->() const;
	inline T& operator*() const;

private:
	Element *ptr;
	List *list;

	inline Iterator(List *list);
	inline Iterator(List *list, Element *const p);
};

template<class T>
List<T>::List() : count(0), first(nullptr), last(nullptr) { }

template<class T>
List<T>::~List()
{
	Element *e = this->first;
	Element *next = nullptr;
	while (e)
	{
		next = e->next;
		delete(e);
		e = next;
	}
}

template<class T>
typename List<T>::Iterator List<T>::begin()
{
	return Iterator(this, this->first);
}

template<class T>
typename List<T>::Iterator List<T>::end()
{
	return Iterator(this);
}

template<class T>
void List<T>::push_back(const T &value)
{
	++count;
	if (!this->first)
	{
		this->first = new Element(value);
		this->last = this->first;
	}
	else
	{
		this->last->next = new Element(value, this->last);
		this->last = this->last->next;
	}
}

template<class T>
size_t List<T>::remove(const T &value, size_t count)
{
	size_t removed = 0;
	Element *e = this->first;
	while (e)
	{
		if (e->value == value)
		{
			if (e == this->first)
			{
				this->first = e->next;
				if (this->first)
					this->first->prev = nullptr;
				else
					this->last = nullptr;
			}
			else if (e == this->last)
			{
				this->last = e->prev;
				this->last->next = nullptr;
			}
			else
			{
				Element *prev = e->prev;
				Element *next = e->next;
				prev->next = next;
				next->prev = prev;
			}
			Element *next = e->next;
			delete e;
			e = next;
			++removed;
			if (!--count)
				break;
		}
		else
			e = e->next;
	}
	this->count -= removed;
	return removed;
}

template<class T>
size_t List<T>::size()
{
	return this->count;
}

template<class T>
void List<T>::Element::swap(Element *b)
{
	Element *next = b->next;
	Element *prev = b->prev;

	if (next == this)  // [.|.|b|a|.|.] -> [.|.|a|b|.|.]
	{
		b->next = this->next;
		if (b->next)
			b->next->prev = b;
		b->prev = this;

		this->next = b;
		this->prev = prev;
		if (this->prev)
			this->prev->next = this;
	}
	else if(prev == this)  // [.|.|a|b|.|.] -> [.|.|b|a|.|.]
	{
		b->next = this;
		b->prev = this->prev;
		if (b->prev)
			b->prev->next = b;

		this->next = next;
		if (this->next)
			this->next->prev = this;
		this->prev = b;
	}
	else  // [.|.|a|.|b|.|.] -> [.|.|b|.|a|.|.]
	{
		b->next = this->next;
		b->prev = this->prev;
		if (b->next)
			b->next->prev = b;
		if (b->prev)
			b->prev->next = b;

		this->next = next;
		this->prev = prev;
		if (next)
			next->prev = this;
		if (prev)
			prev->next = this;
	}
}

template<class T>
List<T>::Element::Element(const T &v, Element *const p, Element *const n) :
	next(n), prev(p), value(v) { }

template<class T>
void List<T>::Iterator::swap(Iterator &i)
{
	R_ASSERT(this->list && i.list, "Swapping between different lists is not supported");
	R_ASSERT(this->ptr && i.ptr, "Iterator is a nullptr");

	this->ptr->swap(i.ptr);
	if (this->ptr == this->list->first)
		this->list->first = i.ptr;
	else if (this->ptr == this->list->last)
		this->list->last = i.ptr;
	else if (i.ptr == this->list->first)
		this->list->first = this->ptr;
	else if (i.ptr == this->list->last)
		this->list->last = this->ptr;
}

template<class T>
List<T>::Iterator::operator bool() const
{
	return this->ptr;
}

template<class T>
typename List<T>::Iterator& List<T>::Iterator::operator++()
{
	this->ptr = this->ptr->next;
	return *this;
}

template<class T>
typename List<T>::Iterator& List<T>::Iterator::operator++(int)
{
	Iterator &i = *this;
	this->ptr = this->ptr->next;
	return i;
}

template<class T>
typename List<T>::Iterator& List<T>::Iterator::operator--()
{
	this->ptr = this->ptr->prev;
	return *this;
}

template<class T>
typename List<T>::Iterator& List<T>::Iterator::operator--(int)
{
	Iterator &i = *this;
	this->ptr = this->ptr->prev;
	return i;
}

template<class T>
T& List<T>::Iterator::operator->() const
{
	return this->ptr->value;
}

template<class T>
T& List<T>::Iterator::operator*() const
{
	return this->ptr->value;
}

template<class T>
List<T>::Iterator::Iterator(List *l) : ptr(nullptr), list(l) { }

template<class T>
List<T>::Iterator::Iterator(List *l, Element *const p) : ptr(p), list(l) { }

#endif
