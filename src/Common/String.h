// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_STRING_H_
#define COMMON_STRING_H_

#include <cstring>
#include <memory>

#if __cpp_lib_experimental_string_view < 201411
#   include <string>
#else
#   include <experimental/string_view>
#endif

namespace rainbow
{
#if __cpp_lib_experimental_string_view < 201411
    class string_view
    {
    public:
        string_view() : data_(nullptr), length_(0) {}
        string_view(const char* str) : data_(str), length_(strlen(str)) {}
        string_view(const char* str, size_t length)
            : data_(str), length_(length) {}
        string_view(const std::string& str)
            : data_(str.data()), length_(str.length()) {}

        auto data() const { return data_; }
        auto length() const { return length_; }

    private:
        const char* data_;
        size_t length_;
    };
#else
    using std::experimental::string_view;
#endif

    template <size_t N>
    bool ends_with(const string_view& str, const char (&postfix)[N])
    {
        constexpr size_t length = N - 1;
        return str.length() >= length &&
               memcmp(str.data() + str.length() - length, postfix, length) == 0;
    }

    inline std::unique_ptr<char[]> make_string_copy(const char* str)
    {
        auto copy = std::make_unique<char[]>(strlen(str) + 1);
        strcpy(copy.get(), str);
        return copy;
    }

    inline std::unique_ptr<char[]> make_string_copy(const string_view& str)
    {
        const size_t length = str.length();
        auto copy = std::make_unique<char[]>(length + 1);
        strncpy(copy.get(), str.data(), length);
        copy[length] = '\0';
        return copy;
    }
}

#endif
