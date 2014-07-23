// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Algorithm.h"

TEST_CASE("floats are approximately equal", "[algorithm]")
{
	REQUIRE(Rainbow::is_equal(0.0f, 0.0f));
	REQUIRE_FALSE(Rainbow::is_equal(0.0f, 0.00001f));
	REQUIRE_FALSE(Rainbow::is_equal(0.0f, -0.00001f));
	REQUIRE(Rainbow::is_equal(3.14285714f, 22.0f / 7.0f));
}

TEST_CASE("Converts radians to degrees", "[algorithm]")
{
	REQUIRE(Rainbow::is_equal(static_cast<float>(kPi),
	                          Rainbow::radians(Rainbow::degrees(kPi))));
}

TEST_CASE("Determines whether an integer is a power of two", "[algorithm]")
{
	unsigned int p = 1;
	for (unsigned int i = 0; i < 100; ++i)
	{
		if (i == p)
		{
			REQUIRE(Rainbow::is_pow2(i));
			p *= 2;
			continue;
		}
		REQUIRE_FALSE(Rainbow::is_pow2(i));
	}
}

TEST_CASE("Returns the next power of two", "[algorithm]")
{
	unsigned int p = 1;
	for (unsigned int i = 1; i < 100; ++i)
	{
		REQUIRE(Rainbow::next_pow2(i) == p);
		if (i == p)
			p *= 2;
	}
}

TEST_CASE("Converts degrees to radians", "[algorithm]")
{
	REQUIRE(Rainbow::is_equal(static_cast<float>(kPi),
	                          Rainbow::degrees(Rainbow::radians(kPi))));
}

TEST_CASE("Converts UTF-8 characters to UTF-32", "[algorithm]")
{
	const unsigned int kInvalidCharacter = 0xb00bbabe;
	const unsigned char utf8[] = {
		0x41,        // LATIN CAPITAL LETTER A
		0x7f,        // <control>
		0x80,        // <invalid>
		0xff,        // <invalid>
		0xc3, 0x85,  // LATIN CAPITAL LETTER A WITH RING ABOVE
		0xc3, 0x86,  // LATIN CAPITAL LETTER AE
		0xc3, 0x98,  // LATIN CAPITAL LETTER O WITH STROKE
		0xc3, 0xa5,  // LATIN SMALL LETTER A WITH RING ABOVE
		0xc3, 0xa6,  // LATIN SMALL LETTER AE
		0xc3, 0xb8,  // LATIN SMALL LETTER O WITH STROKE
		0xe2, 0x82, 0xac,  // EURO SIGN
		0xf0, 0xa4, 0xad, 0xa2  // RANDOM CHINESE CHARACTER
	};
	const unsigned int utf32[] = {
		utf8[0],
		utf8[1],
		kInvalidCharacter,
		kInvalidCharacter,
		0x00c5,
		0x00c6,
		0x00d8,
		0x00e5,
		0x00e6,
		0x00f8,
		0x20ac,
		0x24b62,
		0
	};

	const unsigned char *l = utf8;
	for (size_t i = 0; utf32[i]; ++i)
	{
		const Rainbow::utf_t &c = Rainbow::utf8_decode(l);
		if (c.bytes == 0)
		{
			REQUIRE(utf32[i] == kInvalidCharacter);
			++l;
			continue;
		}
		REQUIRE(utf32[i] == c.code);
		l += c.bytes;
	}
}
