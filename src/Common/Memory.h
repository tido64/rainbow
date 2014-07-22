// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_MEMORY_H_
#define COMMON_MEMORY_H_

#include "Common/Debug.h"
#include "Common/NonCopyable.h"

/// Makes a class reference countable.
///
/// Subclasses of RefCounted can be wrapped with SharedPtr.
class RefCounted : private NonCopyable<RefCounted>
{
	template<typename T> friend class SharedPtr;

protected:
	RefCounted() : refs_(0) { }
	~RefCounted() = default;

private:
	unsigned int refs_;
};

/// An intrusive implementation of std::shared_ptr.
///
/// Only objects that are a subclass of RefCounted can be managed by this
/// pointer type.
template<typename T>
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

	/// Returns a pointer to the managed object.
	T* get() const;

	/// Releases the managed object.
	void reset();

	/// Replaces the managed object.
	void reset(T *ptr);

	/// Returns the number of references to the managed object.
	unsigned int use_count() const;

	/// Dereferences pointer to the managed object.
	T* operator->() const;

	/// Dereferences pointer to the managed object.
	T& operator*() const;

	/// Releases the current pointer and retains the new one.
	SharedPtr<T>& operator=(const SharedPtr<T> &);

	/// Releases the current pointer and takes over the new one.
	SharedPtr<T>& operator=(SharedPtr<T> &&);

	/// Returns whether there is an associated managed object.
	explicit operator bool() const;

private:
	RefCounted *ptr_;  ///< Actual pointer managed by this shared pointer.
};

template<typename T>
SharedPtr<T>::SharedPtr() : ptr_(nullptr) { }

template<typename T>
SharedPtr<T>::SharedPtr(const SharedPtr<T> &shared_ptr) : ptr_(shared_ptr.ptr_)
{
	if (!ptr_)
		return;

	++ptr_->refs_;
}

template<typename T>
SharedPtr<T>::SharedPtr(SharedPtr<T> &&shared_ptr) : ptr_(shared_ptr.ptr_)
{
	shared_ptr.ptr_ = nullptr;
}

template<typename T>
SharedPtr<T>::SharedPtr(T *ptr) : ptr_(ptr)
{
	R_ASSERT(ptr_, "No reference to pointer");
	++ptr_->refs_;
}

template<typename T>
SharedPtr<T>::~SharedPtr()
{
	reset();
}

template<typename T>
T* SharedPtr<T>::get() const
{
	return static_cast<T*>(ptr_);
}

template<typename T>
void SharedPtr<T>::reset()
{
	R_ASSERT(!ptr_ || ptr_->refs_ > 0,
	         "This object should've been deleted by now");

	if (ptr_ && !--ptr_->refs_)
		delete static_cast<T*>(ptr_);
	ptr_ = nullptr;
}

template<typename T>
void SharedPtr<T>::reset(T *ptr)
{
	if (ptr == ptr_)
		return;
	reset();
	ptr_ = ptr;
	if (!ptr_)
		return;
	++ptr_->refs_;
}

template<typename T>
unsigned int SharedPtr<T>::use_count() const
{
	return ptr_->refs_;
}

template<typename T>
T* SharedPtr<T>::operator->() const
{
	R_ASSERT(ptr_, "No reference to pointer");
	return static_cast<T*>(ptr_);
}

template<typename T>
T& SharedPtr<T>::operator*() const
{
	R_ASSERT(ptr_, "No reference to pointer");
	return *static_cast<T*>(ptr_);
}

template<typename T>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr<T> &shared_ptr)
{
	reset(static_cast<T*>(shared_ptr.ptr_));
	return *this;
}

template<typename T>
SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<T> &&shared_ptr)
{
	reset();
	ptr_ = shared_ptr.ptr_;
	shared_ptr.ptr_ = nullptr;
	return *this;
}

template<typename T>
SharedPtr<T>::operator bool() const
{
	return ptr_;
}

#endif
