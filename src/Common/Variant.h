// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_VARIANT_H_
#define COMMON_VARIANT_H_

#if (__cplusplus <= 201402L && !defined(_MSC_VER)) || \
    (defined(_MSC_VER) && _MSC_VER <= 1900)
#   ifdef _MSC_VER
#       pragma warning(push)
#       pragma warning(disable: 4996)
#   endif
#   include <mapbox/variant.hpp>
#   ifdef _MSC_VER
#       pragma warning(pop)
#   endif

namespace rainbow
{
    template <typename... Types>
    using variant = mapbox::util::variant<Types...>;

    template <typename T, typename... Types>
    auto get(variant<Types...>& v)
    {
        return !v.template is<T>() ? T{} : v.template get_unchecked<T>();
    }

    template <typename T, typename... Types>
    auto get(const variant<Types...>& v)
    {
        return !v.template is<T>() ? T{} : v.template get_unchecked<T>();
    }

    template <typename... Args>
    auto visit(Args&&... args)
    {
        return mapbox::util::apply_visitor(std::forward<Args>(args)...);
    }
}
#else
#   include <utility>
#   include <variant>

namespace rainbow
{
    template <typename... Types>
    using variant = std::variant<Types...>;

    template <typename V>
    auto get(V&& v)
    {
        return std::get<T>(std::forward<V>(v));
    }

    template <typename... Args>
    auto visit(Args&&... args)
    {
        return std::visit(std::forward<Args>(args)...);
    }
}
#endif

#endif
