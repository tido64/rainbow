// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef MEMORY_SHAREDPTR_H_
#define MEMORY_SHAREDPTR_H_

#include <type_traits>
#include <utility>

#include "Common/Logging.h"
#include "Common/NonCopyable.h"

namespace rainbow { class LinearAllocator; }

/// Makes a class reference countable.
///
/// Subclasses of RefCounted can be wrapped with SharedPtr.
class RefCounted : private NonCopyable<RefCounted>
{
	friend rainbow::LinearAllocator;
	template<typename T> friend class SharedPtr;

protected:
	RefCounted() : refs_(0) {}
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
	using element_type = T;

	/// Constructs an empty shared pointer.
	SharedPtr() : ptr_(nullptr)
	{
		static_assert(std::is_base_of<RefCounted, T>::value,
		              "Managed objects must be a subclass of RefCounted");
	}

	/// Copies pointer and increments its reference counter.
	SharedPtr(const SharedPtr<T> &ptr) : ptr_(ptr.ptr_)
	{
		if (!ptr_)
			return;

		++ptr_->refs_;
	}

	/// Takes over pointer. Does not increase its reference count and will leave
	/// the other pointer empty.
	SharedPtr(SharedPtr<T> &&ptr) : ptr_(ptr.ptr_)
	{
		ptr.ptr_ = nullptr;
	}

	/// Sets pointer and increment its reference counter.
	explicit SharedPtr(T *ptr) : ptr_(ptr)
	{
		R_ASSERT(ptr_, "No reference to pointer");
		++ptr_->refs_;
	}

	~SharedPtr() { reset(); }

	/// Returns a pointer to the managed object.
	T* get() const { return ptr_; }

	/// Returns the number of references to the managed object.
	unsigned int use_count() const { return ptr_->refs_; }

	/// Releases the managed object.
	void reset()
	{
		R_ASSERT(!ptr_ || ptr_->refs_ > 0,
		         "This object should've been deleted by now");

		if (ptr_ && !--ptr_->refs_)
			delete ptr_;
		ptr_ = nullptr;
	}

	/// Replaces the managed object.
	void reset(T *ptr)
	{
		if (ptr == ptr_)
			return;
		reset();
		ptr_ = ptr;
		if (!ptr_)
			return;
		++ptr_->refs_;
	}

	/// Dereferences pointer to the managed object.
	T* operator->() const
	{
		R_ASSERT(ptr_, "No reference to pointer");
		return ptr_;
	}

	/// Dereferences pointer to the managed object.
	T& operator*() const
	{
		R_ASSERT(ptr_, "No reference to pointer");
		return *ptr_;
	}

	/// Releases the current pointer and retains the new one.
	SharedPtr<T>& operator=(const SharedPtr<T> &ptr)
	{
		reset(ptr.ptr_);
		return *this;
	}

	/// Releases the current pointer and takes over the new one.
	SharedPtr<T>& operator=(SharedPtr<T> &&ptr)
	{
		reset();
		ptr_ = ptr.ptr_;
		ptr.ptr_ = nullptr;
		return *this;
	}

	/// Returns whether there is an associated managed object.
	explicit operator bool() const { return ptr_; }

private:
	T *ptr_;  ///< Actual pointer managed by this shared pointer.
};

template<typename T, typename... Args>
SharedPtr<T> make_shared(Args&&... args)
{
	return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

#endif
