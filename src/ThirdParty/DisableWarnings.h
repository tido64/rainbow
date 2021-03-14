// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#if defined(__GNUC__)
#    pragma GCC diagnostic push
#    ifdef __clang__
#        pragma clang diagnostic ignored "-Wcomma"
#        pragma clang diagnostic ignored "-Wformat-pedantic"
#    endif
#    pragma GCC diagnostic ignored "-Wformat"
#    pragma GCC diagnostic ignored "-Wignored-qualifiers"
#    pragma GCC diagnostic ignored "-Wold-style-cast"
#    pragma GCC diagnostic ignored "-Woverflow"
#    pragma GCC diagnostic ignored "-Wpedantic"
#    pragma GCC diagnostic ignored "-Wsign-compare"
#    pragma GCC diagnostic ignored "-Wsign-promo"
#    pragma GCC diagnostic ignored "-Wstrict-aliasing"
#    pragma GCC diagnostic ignored "-Wtype-limits"
#    pragma GCC diagnostic ignored "-Wunused-function"
#    pragma GCC diagnostic ignored "-Wunused-parameter"
#    pragma GCC diagnostic ignored "-Wunused-variable"
#elif defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable : 4189)  // local variable is initialized but not referenced
#    pragma warning(disable : 4702)  // unreachable code
#    pragma warning(disable : 5054)  // operator '|': deprecated between
                                     // enumerations of different types
#endif
