// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/DisableWarnings.h"

#if defined(_MSC_VER)
#    pragma warning(disable : 4100)  // unreferenced formal parameter
#    pragma warning(disable : 4189)  // local variable is initialized but not referenced
#    pragma warning(disable : 4701)  // potentially uninitialized local variable 'res' used
#elif defined(__GNUC__)
#    pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#    pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#    pragma GCC diagnostic ignored "-Wnested-anon-types"
#    pragma GCC diagnostic ignored "-Wreorder"
#    pragma GCC diagnostic ignored "-Wunused-private-field"
#endif

#define VMA_IMPLEMENTATION 1
#include <vk_mem_alloc.h>

#include "ThirdParty/ReenableWarnings.h"
