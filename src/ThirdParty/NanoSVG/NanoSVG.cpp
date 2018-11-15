// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <cstdio>

#if defined(__GNUC__)
#    pragma GCC diagnostic push
#    ifndef __clang__
#        pragma GCC diagnostic ignored "-Wpedantic"
#    endif
#    pragma GCC diagnostic ignored "-Wold-style-cast"
#elif defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable : 4702)
#endif

#define NANOSVG_IMPLEMENTATION
#include <nanosvg.h>

#define NANOSVGRAST_IMPLEMENTATION
#include <nanosvgrast.h>

#if defined(__GNUC__)
#    pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#    pragma warning(pop)
#endif
