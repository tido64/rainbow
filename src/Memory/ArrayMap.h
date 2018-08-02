// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef MEMORY_ARRAYMAP_H_
#define MEMORY_ARRAYMAP_H_

#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace rainbow
{
    template <typename Key, typename T>
    class ArrayMap
    {
    public:
        using key_type = Key;
        using mapped_type = T;
        using value_type = std::pair<key_type, mapped_type>;
        using container_type = std::vector<value_type>;
        using iterator = typename container_type::iterator;
        using reference = mapped_type&;

        bool empty() const noexcept { return map_.empty(); }
        auto size() const noexcept { return map_.size(); }

        auto back() -> reference { return map_.back().second; }
        auto begin() noexcept { return std::begin(map_); }
        auto end() noexcept { return std::end(map_); }

        void clear() noexcept { map_.clear(); }

        template <typename K>
        auto find(K&& key) -> iterator
        {
            auto end = this->end();
            for (auto i = begin(); i != end; ++i)
            {
                if (i->first == key)
                    return i;
            }
            return end;
        }

        template <typename U>
        auto find_value(U&& value) -> iterator
        {
            auto end = this->end();
            for (auto i = begin(); i != end; ++i)
            {
                if (i->second == value)
                    return i;
            }
            return end;
        }

        template <typename K, typename U>
        auto emplace(K&& key, U&& value) -> std::pair<iterator, bool>
        {
            auto end = this->end();
            for (auto i = begin(); i != end; ++i)
            {
                if (i->first == key)
                    return std::make_pair(i, false);
            }

            map_.emplace_back(
                std::make_pair(std::forward<K>(key), std::forward<U>(value)));
            return std::make_pair(begin() + map_.size() - 1, true);
        }

        template <typename K>
        void erase(K&& key)
        {
            auto i = find(std::forward<K>(key));
            if (i == end())
                return;

            map_.erase(i);
        }

        void erase(iterator first, iterator last) { map_.erase(first, last); }

        template <typename K>
        auto operator[](K&& key) -> reference
        {
            return find(std::forward<K>(key))->second;
        }

    private:
        container_type map_;
    };
}  // namespace rainbow

#endif
