// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Random.h"

#ifdef __GNUC__
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wold-style-cast"
#endif
#include <dSFMT/dSFMT.c>
#ifdef __GNUC__
#	pragma GCC diagnostic pop
#endif

namespace rainbow
{
	dSFMT<float> random;
}
