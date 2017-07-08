// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_ATTRIBUTES_H_
#define COMMON_ATTRIBUTES_H_

#define RAINBOW_ATTRIBUTE(attr) __attribute__((annotate("rainbow " attr)))

#define RAINBOW_INTERNAL RAINBOW_ATTRIBUTE("internal")

#endif
