// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef MEMORY_SHAREDPTR_H_
#define MEMORY_SHAREDPTR_H_

#include <memory>
#include <type_traits>
#include <utility>

#include "Common/Logging.h"
#include "Common/NonCopyable.h"

namespace rainbow
{
    /// <summary>Makes a class reference countable.</summary>
    /// <remarks>
    ///   Subclasses of <see cref="RefCounted"/> can be wrapped with
    ///   <see cref="SharedPtr"/>.
    /// </remarks>
    class RefCounted : private NonCopyable<RefCounted>
    {
    public:
        [[nodiscard]] auto use_count() const { return refs_; }

        auto release() { return --refs_; }
        void retain() { ++refs_; }

    protected:
        RefCounted() = default;
        ~RefCounted() = default;

    private:
        uint32_t refs_ = 0;
    };

    /// <summary>
    ///   An intrusive implementation of <see cref="std::shared_ptr"/>.
    /// </summary>
    /// <remarks>
    ///   Only objects that are a subclass of <see cref="RefCounted"/> can be
    ///   managed by this pointer type.
    /// </remarks>
    template <typename T>
    class SharedPtr
    {
    public:
        using element_type = T;

        /// <summary>Constructs an empty shared pointer.</summary>
        SharedPtr() : ptr_(nullptr)
        {
            static_assert(std::is_base_of_v<RefCounted, T>,
                          "Managed objects must be a subclass of RefCounted");
        }

        /// <summary>
        ///   Copies pointer and increments its reference counter.
        /// </summary>
        SharedPtr(const SharedPtr<T>& ptr) : SharedPtr(ptr.ptr_) {}

        /// <summary>
        ///   Takes over pointer. Does not increase its reference count and will
        ///   leave the other pointer empty.
        /// </summary>
        SharedPtr(SharedPtr<T>&& ptr) noexcept : ptr_(ptr.ptr_)
        {
            ptr.ptr_ = nullptr;
        }

        /// <summary>
        ///   Sets pointer and increments its reference counter.
        /// </summary>
        SharedPtr(std::unique_ptr<T> ptr) : SharedPtr(ptr.release()) {}

        /// <summary>
        ///   Sets pointer and increments its reference counter.
        /// </summary>
        explicit SharedPtr(T* ptr) : ptr_(ptr)
        {
            if (ptr == nullptr)
                return;

            ptr->retain();
        }

        ~SharedPtr() { reset(); }

        /// <summary>Returns a pointer to the managed object.</summary>
        auto get() const { return ptr_; }

        /// <summary>
        ///   Returns the number of references to the managed object.
        /// </summary>
        auto use_count() const { return ptr_->use_count(); }

        /// <summary>Releases the managed object.</summary>
        void reset() { reset_internal(nullptr); }

        /// <summary>Replaces the managed object.</summary>
        void reset(T* ptr)
        {
            if (ptr == ptr_)
                return;

            reset_internal(ptr);
            if (!ptr)
                return;

            ptr->retain();
        }

        /// <summary>Dereferences pointer to the managed object.</summary>
        auto operator->() const
        {
            R_ASSERT(ptr_ != nullptr, "No reference to pointer");
            return ptr_;
        }

        /// <summary>Dereferences pointer to the managed object.</summary>
        auto operator*() const -> T&
        {
            R_ASSERT(ptr_ != nullptr, "No reference to pointer");
            return *ptr_;
        }

        /// <summary>
        ///   Releases the current pointer and retains the new one.
        /// </summary>
        auto operator=(const SharedPtr<T>& ptr) -> SharedPtr<T>&
        {
            if (&ptr == this)
                return *this;

            reset(ptr.ptr_);
            return *this;
        }

        /// <summary>
        ///   Releases the current pointer and takes over the new one.
        /// </summary>
        auto operator=(SharedPtr<T>&& ptr) noexcept -> SharedPtr<T>&
        {
            reset_internal(ptr.ptr_);
            ptr.ptr_ = nullptr;
            return *this;
        }

        /// <summary>
        ///   Returns whether there is an associated managed object.
        /// </summary>
        explicit operator bool() const { return ptr_; }

    private:
        T* ptr_;  ///< Actual pointer managed by this shared pointer.

        void reset_internal(T* ptr)
        {
            R_ASSERT(ptr_ == nullptr || ptr_->use_count() > 0,
                     "This object should've been deleted by now");

            if (ptr_ != nullptr && ptr_->release() == 0)
                delete ptr_;
            ptr_ = ptr;
        }
    };

    template <typename T>
    struct IsSharedPtr : std::false_type
    {
    };

    template <typename T>
    struct IsSharedPtr<SharedPtr<T>> : std::true_type
    {
    };

    template <typename T>
    inline constexpr bool is_shared_ptr = IsSharedPtr<T>::value;

    template <typename T, typename... Args>
    auto make_shared(Args&&... args)
    {
        return SharedPtr<T>{std::make_unique<T>(std::forward<Args>(args)...)};
    }
}  // namespace rainbow

#endif
