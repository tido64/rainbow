// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef MEMORY_SCOPESTACK_H_
#define MEMORY_SCOPESTACK_H_

#include <functional>
#include <memory>

#include "Memory/SharedPtr.h"

namespace rainbow
{
    /// <summary>Linear allocator for use with scope stack allocation.</summary>
    /// <remarks>
    ///   <para>
    ///     Largely copied from Andreas Fredriksson's presentation on the
    ///     subject.
    ///   </para>
    ///   References
    ///   <list type="bullet">
    ///     <item>http://dice.se/publications/scope-stack-allocation/</item>
    ///   </list>
    /// </remarks>
    class LinearAllocator : private NonCopyable<LinearAllocator>
    {
    public:
        static size_t aligned_size(size_t size)
        {
            const size_t kMaxAlignment = alignof(std::max_align_t);
            return (size + (kMaxAlignment - 1)) & ~(kMaxAlignment - 1);
        }

        LinearAllocator(size_t size)
            : block_(std::make_unique<char[]>(size)), end_(block_.get())
#ifndef NDEBUG
            , size_(size)
#endif
        {
        }

        void* end() { return end_; }

        void* allocate(size_t size)
        {
            auto block = end_;
            end_ += aligned_size(size);
            R_ASSERT(end_ <= block_.get() + size_, "Out of memory");
            return block;
        }

        void retain(RefCounted* ref) const
        {
            R_ASSERT(std::less_equal<void*>()(block_.get(), ref) &&
                         std::less<void*>()(ref, end_),
                     "Cannot retain a pointer allocated outside of block");
            ++ref->refs_;
        }

        void rewind(void* ptr) { end_ = static_cast<char*>(ptr); }

    private:
        std::unique_ptr<char[]> block_;
        char* end_;
#ifndef NDEBUG
        const size_t size_;
#endif
    };

    /// <summary>Scope stack allocator.</summary>
    /// <remarks>
    ///   <para>
    ///     Largely copied from Andreas Fredriksson's presentation on the
    ///     subject.
    ///   </para>
    ///   References
    ///   <list type="bullet">
    ///     <item>http://dice.se/publications/scope-stack-allocation/</item>
    ///   </list>
    /// </remarks>
    class ScopeStack : private NonCopyable<ScopeStack>
    {
    public:
        template <typename T>
        static size_t size_of()
        {
            return LinearAllocator::aligned_size(
                LinearAllocator::aligned_size(sizeof(BlockHeader)) + sizeof(T));
        }

        ScopeStack(LinearAllocator& allocator)
            : allocator_(allocator), blocks_(nullptr),
              rewind_point_(static_cast<BlockHeader*>(allocator.end())) {}

        ~ScopeStack() { reset(); }

        template <typename T, typename... Args>
        T* allocate(Args&&... args)
        {
            return new (address_of(new_block<T>()))
                T(std::forward<Args>(args)...);
        }

        void reset()
        {
            while (blocks_)
            {
                (*blocks_->destructor)(address_of(blocks_));
                blocks_ = blocks_->next;
            }
            allocator_.rewind(rewind_point_);
        }

        void retain(RefCounted* ref) const { allocator_.retain(ref); }

    private:
        template <typename T>
        static void destruct(void* ptr)
        {
            static_cast<T*>(ptr)->~T();
        }

        struct BlockHeader
        {
            void (*destructor)(void*);
            BlockHeader* next;
        };

        LinearAllocator& allocator_;
        BlockHeader* blocks_;
        void* const rewind_point_;

        void* address_of(BlockHeader* b)
        {
            return reinterpret_cast<char*>(b) +
                   LinearAllocator::aligned_size(sizeof(*b));
        }

        template <typename T>
        BlockHeader* new_block()
        {
            BlockHeader* block = static_cast<BlockHeader*>(allocator_.allocate(
                LinearAllocator::aligned_size(sizeof(BlockHeader)) +
                sizeof(T)));
            block->destructor = &destruct<T>;
            block->next = blocks_;
            blocks_ = block;
            return block;
        }
    };
}

#endif
