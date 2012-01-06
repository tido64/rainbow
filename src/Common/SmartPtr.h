#ifndef SMARTPTR_H_
#define SMARTPTR_H_

#include <cassert>

#include "Platform.h"

/// Make a class SmartPtr-friendly.
///
/// Subclasses of SmartPtrFriendly can be wrapped with SmartPtr.
///
/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class SmartPtrFriendly
{
	template<class T> friend class SmartPtr;

public:
	SmartPtrFriendly() : refs(0) { }

private:
	unsigned int refs;

	SmartPtrFriendly(const SmartPtrFriendly &);
	SmartPtrFriendly& operator=(const SmartPtrFriendly &);
};

/// Smart pointer automatically releases the allocated memory when it no longer
/// is referenced.
///
/// Classes that need to use this class must subclass SmartPtrFriendly.
///
/// The rationale for this design is to keep things simple and stupid, and
/// prevent frivolous use of this pointer type. Whenever possible, use \c new
/// and \c delete.
///
/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
template<class T>
class SmartPtr
{
public:
	/// Construct an empty smart pointer.
	SmartPtr();

	/// Copy pointer and increment its reference counter.
	SmartPtr(const SmartPtr<T> &);

	/// Set pointer and increment its reference counter.
	explicit SmartPtr(T *);

	~SmartPtr();

	/// Return the actual pointer.
	inline T* raw_ptr() const;

	/// Release the current pointer and copy the new one.
	SmartPtr<T>& operator=(const SmartPtr<T> &);

	/// Release the current pointer and assign the new one.
	SmartPtr<T>& operator=(T *);

	/// Dereferencing this pointer returns the actual pointer.
	inline T& operator*() const;

	/// Dereferencing this pointer returns the actual pointer.
	inline T* operator->() const;

private:
	SmartPtrFriendly *ptr;  ///< Actual pointer managed by this smart pointer.

	/// Decrement reference counter and release memory if the count hits 0.
	void release();
};

template<class T>
SmartPtr<T>::SmartPtr() : ptr(nullptr) { }

template<class T>
SmartPtr<T>::SmartPtr(const SmartPtr<T> &smart_ptr) : ptr(smart_ptr.ptr)
{
	assert(this->ptr || !"Rainbow::SmartPtr: No reference to pointer");
	++this->ptr->refs;
}

template<class T>
SmartPtr<T>::SmartPtr(T *ptr) : ptr(ptr)
{
	assert(this->ptr || !"Rainbow::SmartPtr: No reference to pointer");
	++this->ptr->refs;
}

template<class T>
SmartPtr<T>::~SmartPtr()
{
	this->release();
}

template<class T>
T* SmartPtr<T>::raw_ptr() const
{
	return static_cast<T*>(this->ptr);
}

template<class T>
SmartPtr<T>& SmartPtr<T>::operator=(const SmartPtr<T> &smart_ptr)
{
	return *this = static_cast<T*>(smart_ptr.ptr);
}

template<class T>
SmartPtr<T>& SmartPtr<T>::operator=(T *ptr)
{
	this->release();
	this->ptr = ptr;
	++this->ptr->refs;
	return *this;
}

template<class T>
T& SmartPtr<T>::operator*() const
{
	assert(this->ptr || !"Rainbow::SmartPtr: No reference to pointer");
	return *static_cast<T*>(this->ptr);
}

template<class T>
T* SmartPtr<T>::operator->() const
{
	assert(this->ptr || !"Rainbow::SmartPtr: No reference to pointer");
	return static_cast<T*>(this->ptr);
}

template<class T>
void SmartPtr<T>::release()
{
	assert(!this->ptr || this->ptr->refs > 0
	       || !"Rainbow::SmartPtr::release: This object should've been deleted by now");

	if (this->ptr && !--this->ptr->refs)
	{
		delete this->ptr;
		//this->ptr = nullptr;
	}
}

#endif
