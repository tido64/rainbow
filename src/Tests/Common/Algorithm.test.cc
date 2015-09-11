// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <catch.hpp>

#include "Common/Algorithm.h"
#include "Common/UTF8.h"

TEST_CASE("Clamp values to a range", "[algorithm]")
{
	REQUIRE(rainbow::clamp(10, 0, 1) == 1);
	REQUIRE(rainbow::clamp(-10, 0, 1) == 0);
	REQUIRE(rainbow::clamp(0, 0, 1) == 0);
	REQUIRE(rainbow::clamp(1, 0, 1) == 1);
}

TEST_CASE("Convert radians to degrees", "[algorithm]")
{
	REQUIRE(rainbow::is_equal(static_cast<float>(kPi),
	                          rainbow::radians(rainbow::degrees(kPi))));
}

TEST_CASE("floats are approximately equal", "[algorithm]")
{
	REQUIRE(rainbow::is_equal(0.0f, 0.0f));
	REQUIRE_FALSE(rainbow::is_equal(0.0f, 0.00001f));
	REQUIRE_FALSE(rainbow::is_equal(0.0f, -0.00001f));
	REQUIRE(rainbow::is_equal(3.14285714f, 22.0f / 7.0f));
}

TEST_CASE("Determine whether an integer is a power of two", "[algorithm]")
{
	unsigned int p = 1;
	for (unsigned int i = 0; i < 100; ++i)
	{
		if (i == p)
		{
			REQUIRE(rainbow::is_pow2(i));
			p *= 2;
			continue;
		}
		REQUIRE_FALSE(rainbow::is_pow2(i));
	}
}

TEST_CASE("Return the next power of two", "[algorithm]")
{
	unsigned int p = 1;
	for (unsigned int i = 1; i < 100; ++i)
	{
		REQUIRE(rainbow::next_pow2(i) == p);
		if (i == p)
			p *= 2;
	}
}

TEST_CASE("Quick erase an element in a container", "[algorithm]")
{
	std::vector<int> v{1, 2, 3, 4, 5};
	rainbow::quick_erase(v, v.begin());

	REQUIRE(v.size() == 4);
	REQUIRE(v[0] == 5);
	REQUIRE(v[1] == 2);
	REQUIRE(v[2] == 3);
	REQUIRE(v[3] == 4);

	rainbow::quick_erase(v, v.begin() + 1);

	REQUIRE(v.size() == 3);
	REQUIRE(v[0] == 5);
	REQUIRE(v[1] == 4);
	REQUIRE(v[2] == 3);

	rainbow::quick_erase(v, v.end() - 1);

	REQUIRE(v.size() == 2);
	REQUIRE(v[0] == 5);
	REQUIRE(v[1] == 4);
}

TEST_CASE("Convert degrees to radians", "[algorithm]")
{
	REQUIRE(rainbow::is_equal(static_cast<float>(kPi),
	                          rainbow::degrees(rainbow::radians(kPi))));
}

TEST_CASE("Remove a value from a container", "[algorithm]")
{
	std::vector<int> v{1, 2, 3, 4, 3};
	rainbow::remove(v, 3);

	REQUIRE(v.size() == 4);
	REQUIRE(v[0] == 1);
	REQUIRE(v[1] == 2);
	REQUIRE(v[2] == 4);
	REQUIRE(v[3] == 3);

	rainbow::remove(v, 3);

	REQUIRE(v.size() == 3);
	REQUIRE(v[0] == 1);
	REQUIRE(v[1] == 2);
	REQUIRE(v[2] == 4);

	rainbow::remove(v, 9000);

	REQUIRE(v.size() == 3);
	REQUIRE(v[0] == 1);
	REQUIRE(v[1] == 2);
	REQUIRE(v[2] == 4);
}

TEST_CASE("Remove values satisfying a predicate from a container", "[algorithm]")
{
	std::vector<int> v{1, 2, 3, 4, 5, 6};
	rainbow::remove_if(v, [](int i) { return i % 2 == 0; });

	REQUIRE(v.size() == 3);
	REQUIRE(v[0] == 1);
	REQUIRE(v[1] == 3);
	REQUIRE(v[2] == 5);

	rainbow::remove_if(v, [](int i) { return i > 5; });

	REQUIRE(v.size() == 3);
	REQUIRE(v[0] == 1);
	REQUIRE(v[1] == 3);
	REQUIRE(v[2] == 5);
}

TEST_CASE("Signum function extracts the sign of a real number", "[algorithm]")
{
	REQUIRE(rainbow::signum(-10) < 0);
	REQUIRE(rainbow::signum(0) == 0);
	REQUIRE(rainbow::signum(10) > 0);
}

TEST_CASE("Expand UTF-8 characters to UTF-32", "[algorithm]")
{
	const unsigned int kInvalidCharacter = 0xb00bbabe;

	const unsigned char utf8[]{
	    0x80,                     // <invalid>
	    0xff,                     // <invalid>
	    0xc3, 0x85,               // LATIN CAPITAL LETTER A WITH RING ABOVE
	    0xc3, 0x86,               // LATIN CAPITAL LETTER AE
	    0xc3, 0x98,               // LATIN CAPITAL LETTER O WITH STROKE
	    0xc3, 0xa5,               // LATIN SMALL LETTER A WITH RING ABOVE
	    0xc3, 0xa6,               // LATIN SMALL LETTER AE
	    0xc3, 0xb8,               // LATIN SMALL LETTER O WITH STROKE
	    0xe2, 0x82, 0xac,         // EURO SIGN
	    0xf0, 0x9f, 0x92, 0xa9};  // PILE OF POO

	const unsigned int utf32[]{
	    kInvalidCharacter,
	    kInvalidCharacter,
	    0x00c5,
	    0x00c6,
	    0x00d8,
	    0x00e5,
	    0x00e6,
	    0x00f8,
	    0x20ac,
	    0x1f4a9,
	    0};

	for (unsigned char i = 0; i < 0x80; ++i)
	{
		const rainbow::utf_t &c = rainbow::utf8_decode(&i);
		REQUIRE(c.code == i);
		REQUIRE(static_cast<uint32_t>(c) == c.code);
	}

	const unsigned char *l = utf8;
	for (size_t i = 0; utf32[i]; ++i)
	{
		const rainbow::utf_t &c = rainbow::utf8_decode(l);
		if (c.bytes == 0)
		{
			REQUIRE(utf32[i] == kInvalidCharacter);
			++l;
			continue;
		}
		REQUIRE(utf32[i] == static_cast<uint32_t>(c));
		REQUIRE(utf32[i] == c.code);
		l += c.bytes;
	}
}
