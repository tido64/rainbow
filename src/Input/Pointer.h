// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef INPUT_POINTER_H_
#define INPUT_POINTER_H_

#include <cstdint>

/// <summary>A pointer event.</summary>
struct Pointer
{
	uint32_t hash;       ///< Unique hash value for each pointer.
	int32_t x;           ///< Position of pointer (x-component).
	int32_t y;           ///< Position of pointer (y-component).
	uint64_t timestamp;  ///< The relative time at which this event occurred, in milliseconds.

	Pointer() : hash(0), x(0), y(0), timestamp(0) {}

	Pointer(const uint32_t hash,
	        const int32_t x,
	        const int32_t y,
	        const uint64_t t)
	    : hash(hash), x(x), y(y), timestamp(t) {}
};

#endif
