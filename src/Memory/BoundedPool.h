// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef MEMORY_BOUNDEDPOOL_H_
#define MEMORY_BOUNDEDPOOL_H_

#include <algorithm>
#include <vector>

#include "Common/Logging.h"
#include "Common/TypeCast.h"

namespace rainbow
{
    template <typename T>
    class BoundedPool
    {
        struct Item {
            T item;
            int next_free;
        };

    public:
        BoundedPool() : next_free_{-1} {}

        template <typename F>
        BoundedPool(F&& generate, int size)
        {
            items_.reserve(size);
            for (int i = 0; i < size; ++i)
                items_.push_back({generate(), i + 1});
            items_.back().next_free = -1;
        }

        auto empty() const { return next_free_ < 0; }

        auto next() -> T*
        {
            if (empty())
                return nullptr;

            auto& i = items_[next_free_];
            next_free_ = i.next_free;
            return &i.item;
        }

        void release(T& obj)
        {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
            auto& item = reinterpret_cast<Item&>(obj);
#ifndef NDEBUG
            auto i = std::find_if(
                std::begin(items_),
                std::end(items_),
                [target = &obj](auto&& i) { return &i.item == target; });
            R_ASSERT(i != std::end(items_), "This shouldn't happen.");
            R_ASSERT(&(*i) == &item, "This shouldn't happen.");
#endif
            item.next_free = next_free_;
            next_free_ = narrow_cast<int>(std::distance(items_.data(), &item));

            R_ASSERT(&items_[next_free_] == &item, "This shouldn't happen.");
        }

        template <typename U, typename F>
        friend void for_each(BoundedPool<U>& pool, F&& func)
        {
            for (auto&& i : pool.items_)
                func(i.item);
        }

    private:
        int next_free_ = 0;
        std::vector<Item> items_;
    };
}  // namespace rainbow

#endif
