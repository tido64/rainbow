// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_FUNCTIONAL_H_
#define COMMON_FUNCTIONAL_H_

#include <functional>
#include <iterator>
#include <utility>

#include "Common/Algorithm.h"

namespace rainbow
{
    template <typename T>
    using out = std::reference_wrapper<T>;

    template <typename F, typename T>
    auto filter(F&& pred, const T& list) -> T
    {
        T result;
        std::copy_if(
            std::begin(list), std::end(list), std::back_inserter(result), pred);
        return result;
    }

    template <typename F, typename T>
    auto filter_by_mut(F&& pred, T& list) -> T&
    {
        using U = typename T::value_type;
        remove_if(list, [&pred](const U& i) { return !pred(i); });
        return list;
    }

    template <typename F, typename T, typename U, typename V>
    auto foldl(F&& op, const T& z, const U& list, V i, V end) -> T
    {
        return i == end
                   ? z
                   : foldl(std::forward<F>(op), op(z, *i), list, i + 1, end);
    }

    template <typename F, typename T, typename U>
    auto foldl(F&& op, const T& z, const U& list) -> T
    {
        return foldl(
            std::forward<F>(op), z, list, std::begin(list), std::end(list));
    }

    template <typename F, typename T>
    auto foldl1(F&& op, const T& list)
        -> decltype(op(*std::begin(list), *std::begin(list)))
    {
        auto i = std::begin(list);
        return foldl(std::forward<F>(op), *i, list, i + 1, std::end(list));
    }

    template <typename F, typename T, typename U, typename V>
    auto foldr(F&& op, const T& z, const U& list, V i, V rend) -> T
    {
        return i == rend
                   ? z
                   : foldr(std::forward<F>(op), op(*i, z), list, i + 1, rend);
    }

    template <typename F, typename T, typename U>
    auto foldr(F&& op, const T& z, const U& list) -> T
    {
        return foldr(
            std::forward<F>(op), z, list, std::rbegin(list), std::rend(list));
    }

    template <typename F, typename T>
    auto foldr1(F&& op, const T& list)
        -> decltype(op(*std::rbegin(list), *std::rbegin(list)))
    {
        auto i = std::rbegin(list);
        return foldr(std::forward<F>(op), *i, list, i + 1, std::rend(list));
    }

    template <typename F, typename T>
    auto map(F&& op, const T& list) -> T
    {
        T result;
        std::transform(
            std::begin(list), std::end(list), std::back_inserter(result), op);
        return result;
    }

    template <typename F, typename T>
    auto map_by_mut(F&& op, T& list) -> T&
    {
        std::transform(std::begin(list), std::end(list), std::begin(list), op);
        return list;
    }

    template <typename T>
    auto accumulate(const T& list)
        -> decltype(*std::begin(list) + *std::begin(list))
    {
        using U = typename T::value_type;
        return foldl1([](U a, U b) { return a + b; }, list);
    }
}

#endif
