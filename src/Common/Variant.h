// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_VARIANT_H_
#define COMMON_VARIANT_H_

#if !__has_include(<variant>)
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

    template <typename T, typename... Types>
    constexpr auto holds_alternative(const variant<Types...>& v)
    {
        return v.template is<T>();
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

    template <typename T, typename... Types>
    auto get(variant<Types...>& v)
    {
        return std::get<T>(v);
    }

    template <typename T, typename... Types>
    constexpr auto holds_alternative(const variant<Types...>& v) noexcept
    {
        return std::holds_alternative<T>(v);
    }

    template <typename... Args>
    auto visit(Args&&... args)
    {
        return std::visit(std::forward<Args>(args)...);
    }
}
#endif

#endif
