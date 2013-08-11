#ifndef COMMON_SHAREDPTR_H_
#define COMMON_SHAREDPTR_H_

#include "Common/Debug.h"
#include "Common/NonCopyable.h"

/// Makes a class reference countable.
///
/// Subclasses of RefCounted can be wrapped with SharedPtr.
///
/// Copyright 2012-13 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class RefCounted : private NonCopyable<RefCounted>
{
	template<class T> friend class SharedPtr;

public:
	RefCounted() : refs(0) { }

private:
	unsigned int refs;
};

/// Shared pointers automatically release the allocated memory when it no longer
/// is referenced.
///
/// Classes that need to be shared this way must subclass RefCounted.
///
/// Copyright 2011-13 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
template<class T>
class SharedPtr
{
public:
	/// Constructs an empty shared pointer.
	SharedPtr();

	/// Copies pointer and increments its reference counter.
	SharedPtr(const SharedPtr<T> &);

	/// Sets pointer and increment its reference counter.
	explicit SharedPtr(T *);

	~SharedPtr();

	/// Returns pointer to the managed object.
	inline T* get() const;

	/// Releases the current pointer and retains the new one.
	SharedPtr<T>& operator=(const SharedPtr<T> &);

	/// Releases the current pointer and assigns the new one.
	SharedPtr<T>& operator=(T *);

	/// Dereferences pointer to the managed object.
	inline T& operator*() const;

	/// Dereferences pointer to the managed object.
	inline T* operator->() const;

private:
	RefCounted *ptr;  ///< Actual pointer managed by this shared pointer.

	/// Decrements reference counter and releases memory if the count hits 0.
	void release();
};

template<class T>
SharedPtr<T>::SharedPtr() : ptr(nullptr) { }

template<class T>
SharedPtr<T>::SharedPtr(const SharedPtr<T> &shared_ptr) : ptr(shared_ptr.ptr)
{
	R_ASSERT(this->ptr, "No reference to pointer");
	++this->ptr->refs;
}

template<class T>
SharedPtr<T>::SharedPtr(T *ptr) : ptr(ptr)
{
	R_ASSERT(this->ptr, "No reference to pointer");
	++this->ptr->refs;
}

template<class T>
SharedPtr<T>::~SharedPtr()
{
	this->release();
}

template<class T>
T* SharedPtr<T>::get() const
{
	return static_cast<T*>(this->ptr);
}

template<class T>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr<T> &shared_ptr)
{
	return *this = static_cast<T*>(shared_ptr.ptr);
}

template<class T>
SharedPtr<T>& SharedPtr<T>::operator=(T *ptr)
{
	this->release();
	this->ptr = ptr;
	++this->ptr->refs;
	return *this;
}

template<class T>
T& SharedPtr<T>::operator*() const
{
	R_ASSERT(this->ptr, "No reference to pointer");
	return *static_cast<T*>(this->ptr);
}

template<class T>
T* SharedPtr<T>::operator->() const
{
	R_ASSERT(this->ptr, "No reference to pointer");
	return static_cast<T*>(this->ptr);
}

template<class T>
void SharedPtr<T>::release()
{
	R_ASSERT(!this->ptr || this->ptr->refs > 0,
	         "This object should've been deleted by now");

	if (this->ptr && !--this->ptr->refs)
	{
		delete static_cast<T*>(this->ptr);
		this->ptr = nullptr;
	}
}

#endif
