// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <cstdio>

#ifdef __GNUC__
#   pragma GCC diagnostic push
#   ifndef __clang__
#      pragma GCC diagnostic ignored "-Wpedantic"
#   endif
#   pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

#define NANOSVG_IMPLEMENTATION
#include <nanosvg.h>

#define NANOSVGRAST_IMPLEMENTATION
#include <nanosvgrast.h>

#ifdef __GNUC__
#   pragma GCC diagnostic pop
#endif
