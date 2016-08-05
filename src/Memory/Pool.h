// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <deque>
#include <utility>

#include "Memory/NotNull.h"

namespace rainbow
{
    namespace detail
    {
        template <typename T>
        struct PoolItem
        {
            T element;
            PoolItem* next_free;

            template <typename... Args>
            PoolItem(Args&&... args)
                : element(std::forward<Args>(args)...), next_free(nullptr) {}
        };
    }

    /// <summary>Memory pool for larger number of reusable elements.</summary>
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
            if (!free_)
            {
                pool_.emplace_back(std::forward<Args>(args)...);
                return &pool_.back().element;
            }

            value_type& element = pop();
            element = value_type{std::forward<Args>(args)...};
            return &element;
        }

        /// <summary>Releases the element to the pool for reuse.</summary>
        /// <remarks>The pointer is invalidated after return.</remarks>
        void release(NotNull<value_type*> element)
        {
            element->dispose();

            Entry* e = get_entry(element);
            R_ASSERT(&e->element == element.get(), "This shouldn't happen.");

            e->next_free = free_;
            free_ = e;
        }

    private:
        using Entry = detail::PoolItem<value_type>;

        Entry* free_ = nullptr;
        Container pool_;

        /// <summary>Pops and returns the next reusable element.</summary>
        auto pop() -> value_type&
        {
            Entry* e = free_;
            free_ = e->next_free;
            return e->element;
        }

        /// <summary>Returns the element's entry in the pool.<summary>
        static auto get_entry(NotNull<value_type*> element)
        {
            return reinterpret_cast<Entry*>(element.get());
        }
    };
}
