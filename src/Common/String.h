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

    enum class StringComparison
    {
        Ordinal,
        IgnoreCase
    };

    template <size_t N>
    bool ends_with(czstring str, size_t str_length, const char (&suffix)[N])
    {
        constexpr size_t length = N - 1;
        return str_length >= length &&
               memcmp(str + str_length - length, suffix, length) == 0;
    }

    template <size_t N>
    bool ends_with(czstring str, const char (&suffix)[N])
    {
        return ends_with(str, strlen(str), suffix);
    }

    template <size_t N>
    bool ends_with(czstring str,
                   const char (&suffix)[N],
                   StringComparison comparison)
    {
        const auto str_length = strlen(str);
        if (comparison == StringComparison::Ordinal)
            return ends_with(str, str_length, suffix);

        constexpr size_t suffix_length = N - 1;
        if (suffix_length > str_length)
            return false;

        czstring s = str + str_length - suffix_length;
        for (size_t i = 0; i < suffix_length; ++i, ++s)
        {
            if (tolower(*s) != tolower(suffix[i]))
                return false;
        }

        return true;
    }

    template <size_t N>
    bool ends_with(const std::string& str, const char (&suffix)[N])
    {
        return ends_with(str.c_str(), str.length(), suffix);
    }
}

#endif
