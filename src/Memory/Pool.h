// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef MEMORY_POOL_H_
#define MEMORY_POOL_H_

#include <deque>
#include <utility>

#include "Common/NonCopyable.h"
#include "Memory/NotNull.h"

namespace rainbow
{
    namespace detail
    {
        template <typename T>
        struct PoolItem : private NonCopyable<PoolItem<T>> {
            T element;
            PoolItem* next_free;

            template <typename... Args>
            PoolItem(Args&&... args)
                : element(std::forward<Args>(args)...), next_free(this)
            {
            }
        };
    }  // namespace detail

    /// <summary>
    ///   Memory pool for elements that are created and deleted often.
    /// </summary>
    template <typename T, typename Container = std::deque<detail::PoolItem<T>>>
    class Pool
    {
    public:
        using size_type = typename Container::size_type;
        using value_type = T;

        /// <summary>
        ///   Returns number of elements currently in circulation.
        /// </summary>
        auto size() const { return pool_.size(); }

        /// <summary>Removes all elements from the pool.</summary>
        void clear()
        {
            free_ = nullptr;
            pool_.clear();
        }

        /// <summary>
        ///   Returns an element constructed with specified parameters.
        /// </summary>
        template <typename... Args>
        auto construct(Args&&... args) -> value_type*
        {
            if (!free_) {
                pool_.emplace_back(std::forward<Args>(args)...);
                auto& i = pool_.back();
                R_ASSERT(i.next_free == &i, "Expected sentinel");
                return &i.element;
            }

            value_type& element = pop();
            element.reset(std::forward<Args>(args)...);
            return &element;
        }

        /// <summary>Releases the element to the pool for reuse.</summary>
        /// <remarks>The pointer is invalidated after return.</remarks>
        template <typename... Args>
        void release(NotNull<value_type*> element, Args&&... args)
        {
            element->dispose(std::forward<Args>(args)...);

            auto i = get_iterator(element);
            i->next_free = free_;
            free_ = i;
        }

        template <typename U, typename V, typename F>
        friend void for_each(Pool<U, V>& pool, F&& action)
        {
            const auto size = pool.size();
            for (size_t i = 0; i < size; ++i) {
                auto& item = pool.pool_[i];
                if (item.next_free != &item)
                    continue;

                action(item.element);
            }
        }

    private:
        detail::PoolItem<value_type>* free_ = nullptr;
        Container pool_;

        /// <summary>Pops and returns the next reusable element.</summary>
        auto pop() -> value_type&
        {
            auto i = free_;
            free_ = i->next_free;
            i->next_free = i;  // Set sentinel to indicate item is alive.
            return i->element;
        }

        /// <summary>Returns the element's iterator.<summary>
        static auto get_iterator(NotNull<value_type*> element)
        {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
            auto i = reinterpret_cast<decltype(free_)>(element.get());
            R_ASSERT(&i->element == element.get(), "This shouldn't happen.");
            return i;
        }
    };
}  // namespace rainbow

#endif
