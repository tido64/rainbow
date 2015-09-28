// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/Algorithm.h"
#include "Common/UTF8.h"

TEST(AlgorithmTest, ClampsValues)
{
	ASSERT_EQ(1, rainbow::clamp(10, 0, 1));
	ASSERT_EQ(0, rainbow::clamp(-10, 0, 1));
	ASSERT_EQ(0, rainbow::clamp(0, 0, 1));
	ASSERT_EQ(1, rainbow::clamp(1, 0, 1));
}

TEST(AlgorithmTest, ConvertsRadiansToDegrees)
{
	ASSERT_TRUE(rainbow::is_equal(static_cast<float>(kPi),
	                              rainbow::radians(rainbow::degrees(kPi))));
}

TEST(AlgorithmTest, ApproximatesFloatEquality)
{
	ASSERT_TRUE(rainbow::is_equal(0.0f, 0.0f));
	ASSERT_FALSE(rainbow::is_equal(0.0f, 0.00001f));
	ASSERT_FALSE(rainbow::is_equal(0.0f, -0.00001f));
	ASSERT_TRUE(rainbow::is_equal(3.14285714f, 22.0f / 7.0f));
}

TEST(AlgorithmTest, IsPowerOfTwo)
{
	unsigned int p = 1;
	for (unsigned int i = 0; i < 100; ++i)
	{
		if (i == p)
		{
			ASSERT_TRUE(rainbow::is_pow2(i));
			p *= 2;
			continue;
		}
		ASSERT_FALSE(rainbow::is_pow2(i));
	}
}

TEST(AlgorithmTest, ReturnsNextPowerOfTwo)
{
	unsigned int p = 1;
	for (unsigned int i = 1; i < 100; ++i)
	{
		ASSERT_EQ(p, rainbow::next_pow2(i));
		if (i == p)
			p *= 2;
	}
}

TEST(AlgorithmTest, QuickErasesElementsInContainer)
{
	std::vector<int> v{1, 2, 3, 4, 5};
	rainbow::quick_erase(v, v.begin());

	ASSERT_EQ(4u, v.size());
	ASSERT_EQ(5, v[0]);
	ASSERT_EQ(2, v[1]);
	ASSERT_EQ(3, v[2]);
	ASSERT_EQ(4, v[3]);

	rainbow::quick_erase(v, v.begin() + 1);

	ASSERT_EQ(3u, v.size());
	ASSERT_EQ(5, v[0]);
	ASSERT_EQ(4, v[1]);
	ASSERT_EQ(3, v[2]);

	rainbow::quick_erase(v, v.end() - 1);

	ASSERT_EQ(2u, v.size());
	ASSERT_EQ(5, v[0]);
	ASSERT_EQ(4, v[1]);
}

TEST(AlgorithmTest, ConvertsDegreesToRadians)
{
	ASSERT_TRUE(rainbow::is_equal(static_cast<float>(kPi),
	                              rainbow::degrees(rainbow::radians(kPi))));
}

TEST(AlgorithmTest, RemovesValuesFromContainer)
{
	std::vector<int> v{1, 2, 3, 4, 3};
	rainbow::remove(v, 3);

	ASSERT_EQ(4u, v.size());
	ASSERT_EQ(1, v[0]);
	ASSERT_EQ(2, v[1]);
	ASSERT_EQ(4, v[2]);
	ASSERT_EQ(3, v[3]);

	rainbow::remove(v, 3);

	ASSERT_EQ(3u, v.size());
	ASSERT_EQ(1, v[0]);
	ASSERT_EQ(2, v[1]);
	ASSERT_EQ(4, v[2]);

	rainbow::remove(v, 9000);

	ASSERT_EQ(3u, v.size());
	ASSERT_EQ(1, v[0]);
	ASSERT_EQ(2, v[1]);
	ASSERT_EQ(4, v[2]);
}

TEST(AlgorithmTest, RemovesValuesSatisfyingPredicatesFromContainer)
{
	std::vector<int> v{1, 2, 3, 4, 5, 6};
	rainbow::remove_if(v, [](int i) { return i % 2 == 0; });

	ASSERT_EQ(3u, v.size());
	ASSERT_EQ(1, v[0]);
	ASSERT_EQ(3, v[1]);
	ASSERT_EQ(5, v[2]);

	rainbow::remove_if(v, [](int i) { return i > 5; });

	ASSERT_EQ(3u, v.size());
	ASSERT_EQ(1, v[0]);
	ASSERT_EQ(3, v[1]);
	ASSERT_EQ(5, v[2]);
}

TEST(AlgorithmTest, ExtractsSignOfRealNumbers)
{
	ASSERT_GT(0, rainbow::signum(-10));
	ASSERT_EQ(0, rainbow::signum(0));
	ASSERT_LT(0, rainbow::signum(10));
}

TEST(AlgorithmTest, ExpandsUTF8StringToUTF32)
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
		const rainbow::utf_t& c = rainbow::utf8_decode(&i);
		ASSERT_EQ(i, c.code);
		ASSERT_EQ(c.code, static_cast<uint32_t>(c));
	}

	const unsigned char* l = utf8;
	for (size_t i = 0; utf32[i]; ++i)
	{
		const rainbow::utf_t& c = rainbow::utf8_decode(l);
		if (c.bytes == 0)
		{
			ASSERT_EQ(kInvalidCharacter, utf32[i]);
			++l;
			continue;
		}
		ASSERT_EQ(static_cast<uint32_t>(c), utf32[i]);
		ASSERT_EQ(c.code, utf32[i]);
		l += c.bytes;
	}
}
