// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_SHAREDPTR_H_
#define COMMON_SHAREDPTR_H_

#include "Common/Debug.h"
#include "Common/NonCopyable.h"

/// Makes a class reference countable.
///
/// Subclasses of RefCounted can be wrapped with SharedPtr.
class RefCounted : private NonCopyable<RefCounted>
{
	template<class T> friend class SharedPtr;

protected:
	RefCounted() : refs_(0) { }
	~RefCounted() = default;

private:
	unsigned int refs_;
};

/// Shared pointers automatically release the allocated memory when it no longer
/// is referenced.
///
/// Classes that need to be shared this way must subclass RefCounted.
template<class T>
class SharedPtr
{
public:
	/// Constructs an empty shared pointer.
	SharedPtr();

	/// Copies pointer and increments its reference counter.
	SharedPtr(const SharedPtr<T> &);

	/// Takes over pointer. Does not increase its reference count and will leave
	/// the other pointer empty.
	SharedPtr(SharedPtr<T> &&);

	/// Sets pointer and increment its reference counter.
	explicit SharedPtr(T *);

	~SharedPtr();

	/// Returns pointer to the managed object.
	inline T* get() const;

#ifdef RAINBOW_TEST
	/// Returns the number of references to this object.
	unsigned int ref_count() const { return ptr_->refs_; }
#endif

	/// Releases the current pointer and retains the new one.
	SharedPtr<T>& operator=(const SharedPtr<T> &);

	/// Releases the current pointer and takes over the new one.
	SharedPtr<T>& operator=(SharedPtr<T> &&);

	/// Releases the current pointer and assigns the new one.
	SharedPtr<T>& operator=(T *);

	/// Dereferences pointer to the managed object.
	inline T& operator*() const;

	/// Dereferences pointer to the managed object.
	inline T* operator->() const;

	/// Returns whether there is an associated managed object.
	explicit operator bool() const;

private:
	RefCounted *ptr_;  ///< Actual pointer managed by this shared pointer.

	/// Decrements reference counter and releases memory if the count hits 0.
	void release();
};

template<class T>
SharedPtr<T>::SharedPtr() : ptr_(nullptr) { }

template<class T>
SharedPtr<T>::SharedPtr(const SharedPtr<T> &shared_ptr) : ptr_(shared_ptr.ptr_)
{
	if (!ptr_)
		return;

	++ptr_->refs_;
}

template<class T>
SharedPtr<T>::SharedPtr(SharedPtr<T> &&shared_ptr) : ptr_(shared_ptr.ptr_)
{
	shared_ptr.ptr_ = nullptr;
}

template<class T>
SharedPtr<T>::SharedPtr(T *ptr) : ptr_(ptr)
{
	R_ASSERT(ptr_, "No reference to pointer");
	++ptr_->refs_;
}

template<class T>
SharedPtr<T>::~SharedPtr()
{
	release();
}

template<class T>
T* SharedPtr<T>::get() const
{
	return static_cast<T*>(ptr_);
}

template<class T>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr<T> &shared_ptr)
{
	return *this = static_cast<T*>(shared_ptr.ptr_);
}

template<class T>
SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<T> &&shared_ptr)
{
	release();
	ptr_ = shared_ptr.ptr_;
	shared_ptr.ptr_ = nullptr;
	return *this;
}

template<class T>
SharedPtr<T>& SharedPtr<T>::operator=(T *ptr)
{
	if (ptr == ptr_)
		return *this;
	release();
	ptr_ = ptr;
	if (!ptr_)
		return *this;
	++ptr_->refs_;
	return *this;
}

template<class T>
T& SharedPtr<T>::operator*() const
{
	R_ASSERT(ptr_, "No reference to pointer");
	return *static_cast<T*>(ptr_);
}

template<class T>
T* SharedPtr<T>::operator->() const
{
	R_ASSERT(ptr_, "No reference to pointer");
	return static_cast<T*>(ptr_);
}

template<class T>
SharedPtr<T>::operator bool() const
{
	return ptr_;
}

template<class T>
void SharedPtr<T>::release()
{
	R_ASSERT(!ptr_ || ptr_->refs_ > 0,
	         "This object should've been deleted by now");

	if (ptr_ && !--ptr_->refs_)
	{
		delete static_cast<T*>(ptr_);
		ptr_ = nullptr;
	}
}

#endif
