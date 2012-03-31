#ifndef LIST_H_
#define LIST_H_

#include "Platform.h"

/// Simple double-linked list container, mimicking STL::list.
///
/// This class is mainly for the lack of full STL implementation on some
/// platforms, such as Android.
///
/// Copyright 2011-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
template<class T>
class List
{
public:
	class Iterator;

	inline List();
	~List();

	/// Return iterator to the beginning.
	inline Iterator begin() const;

	/// Return iterator to the end.
	inline Iterator end() const;

	/// Insert an item at the back of the list.
	void push_back(const T &value);

	/// Remove elements with specific value.
	/// \param value  The value to remove from the list.
	/// \param count  Number of elements to remove from the list.
	/// \return Number of elements removed.
	unsigned int remove(const T &value, unsigned int count = 1);

	/// Return size of list.
	inline unsigned int size();

private:
	class Element;
	unsigned int count;  ///< Element count
	Element *first;      ///< Pointer to the first element
	Element *last;       ///< Pointer to the last element

	/// Intentionally left undefined.
	List(const List &);

	/// Intentionally left undefined.
	List& operator=(const List &);
};

template<class T>
class List<T>::Element
{
	friend class List;
	friend class List::Iterator;

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
	inline Iterator();
	inline Iterator(const Iterator &i);

	Iterator& operator++();
	Iterator operator++(int);
	Iterator& operator--();
	Iterator operator--(int);
	inline T& operator->() const;
	inline T& operator*() const;
	inline bool operator==(const Iterator &i) const;
	inline bool operator!=(const Iterator &i) const;

private:
	Element *ptr;

	inline Iterator(Element *const p);
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
typename List<T>::Iterator List<T>::begin() const
{
	return Iterator(this->first);
}

template<class T>
typename List<T>::Iterator List<T>::end() const
{
	return Iterator();
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
unsigned int List<T>::remove(const T &value, unsigned int count)
{
	unsigned int removed = 0;
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
unsigned int List<T>::size()
{
	return this->count;
}

template<class T>
List<T>::Element::Element(const T &v, Element *const p, Element *const n) :
	next(n), prev(p), value(v) { }

template<class T>
List<T>::Iterator::Iterator() : ptr(nullptr) { }

template<class T>
List<T>::Iterator::Iterator(const Iterator &i) : ptr(i.ptr) { }

template<class T>
typename List<T>::Iterator& List<T>::Iterator::operator++()
{
	this->ptr = this->ptr->next;
	return *this;
}

template<class T>
typename List<T>::Iterator List<T>::Iterator::operator++(int)
{
	Iterator i(*this);
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
typename List<T>::Iterator List<T>::Iterator::operator--(int)
{
	Iterator i(*this);
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
bool List<T>::Iterator::operator==(const Iterator &i) const
{
	return this->ptr == i.ptr;
}

template<class T>
bool List<T>::Iterator::operator!=(const Iterator &i) const
{
	return this->ptr != i.ptr;
}

template<class T>
List<T>::Iterator::Iterator(Element *const p) : ptr(p) { }

#endif
