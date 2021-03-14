// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef MEMORY_ARRAYMAP_H_
#define MEMORY_ARRAYMAP_H_

#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "Common/Algorithm.h"

namespace rainbow
{
    template <typename Key, typename T>
    class ArrayMap
    {
        template <typename Iter, typename K>
        [[nodiscard]] static auto find(Iter i, Iter last, K&& key) -> Iter
        {
            for (; i != last; ++i) {
                if (i->first == key)
                    return i;
            }
            return last;
        }

    public:
        using key_type = Key;
        using mapped_type = T;
        using value_type = std::pair<key_type, mapped_type>;
        using container_type = std::vector<value_type>;
        using const_iterator = typename container_type::const_iterator;
        using iterator = typename container_type::iterator;
        using reference = mapped_type&;

        [[nodiscard]] auto empty() const noexcept { return map_.empty(); }
        [[nodiscard]] auto size() const noexcept { return map_.size(); }

        [[nodiscard]] auto back() -> reference { return map_.back().second; }
        [[nodiscard]] auto begin() noexcept { return std::begin(map_); }
        [[nodiscard]] auto begin() const noexcept { return std::begin(map_); }
        [[nodiscard]] auto end() noexcept { return std::end(map_); }
        [[nodiscard]] auto end() const noexcept { return std::end(map_); }

        void clear() noexcept { map_.clear(); }

        template <typename K>
        [[nodiscard]] auto find(K&& key) -> iterator
        {
            return find(begin(), end(), std::forward<K>(key));
        }

        template <typename K>
        [[nodiscard]] auto find(K&& key) const -> const_iterator
        {
            return find(begin(), end(), std::forward<K>(key));
        }

        template <typename U>
        [[nodiscard]] auto find_value(U&& value) -> iterator
        {
            auto end = this->end();
            for (auto i = begin(); i != end; ++i) {
                if (i->second == value)
                    return i;
            }
            return end;
        }

        template <typename K, typename U>
        auto emplace(K&& key, U&& value) -> std::pair<iterator, bool>
        {
            auto end = this->end();
            for (auto i = begin(); i != end; ++i) {
                if (i->first == key)
                    return std::make_pair(i, false);
            }

            map_.emplace_back(
                std::make_pair(std::forward<K>(key), std::forward<U>(value)));
            return std::make_pair(begin() + map_.size() - 1, true);
        }

        void erase(iterator i) { quick_erase(map_, i); }

        template <typename K>
        void erase(K&& key)
        {
            auto i = find(std::forward<K>(key));
            if (i == end())
                return;

            quick_erase(map_, i);
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
