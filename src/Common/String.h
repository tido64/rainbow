// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_STRING_H_
#define COMMON_STRING_H_

#include <cstring>
#include <memory>
#include <string>

namespace rainbow
{
    template <typename CharT>
    using basic_zstring = CharT*;

    using czstring = basic_zstring<const char>;
    using zstring = basic_zstring<char>;

    enum class StringComparison {
        Ordinal,
        IgnoreCase,
    };

    template <size_t N>
    auto ends_with(std::string_view str, const char (&suffix)[N]) -> bool
    {
        constexpr size_t length = N - 1;

        static_assert(
            length > 0, "Comparing against empty suffix makes no sense");

        return str.length() >= length &&
               memcmp(str.data() + str.length() - length, suffix, length) == 0;
    }

    template <size_t N>
    auto ends_with(std::string_view str,
                   const char (&suffix)[N],
                   StringComparison comparison) -> bool
    {
        if (comparison == StringComparison::Ordinal)
            return ends_with(str, suffix);

        constexpr size_t suffix_length = N - 1;
        if (suffix_length > str.length())
            return false;

        czstring s = str.data() + str.length() - suffix_length;
        for (size_t i = 0; i < suffix_length; ++i, ++s) {
            if (tolower(*s) != tolower(suffix[i]))
                return false;
        }

        return true;
    }

    constexpr auto is_empty(czstring str) -> bool
    {
        return str == nullptr || *str == '\0';
    }
}  // namespace rainbow

#endif
