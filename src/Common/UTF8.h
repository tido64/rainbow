// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_UTF8_H_
#define COMMON_UTF8_H_

#include <cstdint>

namespace rainbow
{
	struct utf_t
	{
		uint32_t code = 0;
		uint32_t bytes = 0;

		operator bool() const { return this->bytes > 0; }
		operator uint32_t() const { return this->code; }
	};

	/// <summary>Expands a UTF-8 character to UTF-32.</summary>
	utf_t utf8_decode(const unsigned char *str);
}

#endif
