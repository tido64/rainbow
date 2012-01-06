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

	List();
	~List();

	/// Return iterator to the beginning.
	Iterator begin() const;

	/// Return iterator to the end.
	Iterator end() const;

	/// Insert an item at the back of the list.
	void push_back(const T &value);

	/// Remove elements with specific value.
	void remove(const T &value);

	/// Return size of list.
	unsigned int size();

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
inline List<T>::List() :
	count(0), first(nullptr), last(nullptr) { }

template<class T>
List<T>::~List()
{
	delete this->first;
}

template<class T>
inline typename List<T>::Iterator List<T>::begin() const
{
	return Iterator(this->first);
}

template<class T>
inline typename List<T>::Iterator List<T>::end() const
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
		return;
	}
	this->last->next = new Element(value, this->last);
	this->last = this->last->next;
}

template<class T>
void List<T>::remove(const T &value)
{
	Element *e = this->first;
	while (e)
	{
		if (e->value == value)
		{
			Element *prev = e->prev;
			Element *next = e->next;
			if (prev)
				prev->next = next;
			else
				this->first = next;
			if (next)
			{
				next->prev = prev;
				e->next = nullptr;
			}
			else
				this->last = prev;
			delete e;
			e = next;
			--this->count;
		}
		else
			e = e->next;
	}
}

template<class T>
inline unsigned int List<T>::size()
{
	return this->count;
}

template<class T>
class List<T>::Element
{
	friend class List;
	friend class List::Iterator;

	Element *next;  ///< Pointer to the next element
	Element *prev;  ///< Pointer to the previous element
	T value;        ///< Stored value

	Element(const T &v, Element *const p = 0, Element *const n = 0);
	~Element();
};

template<class T>
inline List<T>::Element::Element(const T &v, Element *const p, Element *const n) :
	next(n), prev(p), value(v) { }

template<class T>
List<T>::Element::~Element()
{
	delete this->next;
}

template<class T>
class List<T>::Iterator
{
	friend class List;

public:
	Iterator();
	Iterator(const Iterator &i);

	bool operator==(const Iterator &i) const;
	bool operator!=(const Iterator &i) const;
	Iterator& operator++();
	Iterator operator++(int);
	Iterator& operator--();
	Iterator operator--(int);
	T& operator*() const;
	T& operator->() const;

private:
	Element *ptr;

	Iterator(Element *const p);
};

template<class T>
inline List<T>::Iterator::Iterator() :
	ptr(nullptr) { }

template<class T>
inline List<T>::Iterator::Iterator(const Iterator &i) :
	ptr(i.ptr) { }

template<class T>
inline bool List<T>::Iterator::operator==(const Iterator &i) const
{
	return this->ptr == i.ptr;
}

template<class T>
inline bool List<T>::Iterator::operator!=(const Iterator &i) const
{
	return this->ptr != i.ptr;
}

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
inline T& List<T>::Iterator::operator*() const
{
	return this->ptr->value;
}

template<class T>
inline T& List<T>::Iterator::operator->() const
{
	return this->ptr->value;
}

template<class T>
inline List<T>::Iterator::Iterator(Element *const p) :
	ptr(p) { }

#endif
