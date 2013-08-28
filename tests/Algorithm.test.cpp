#include "Common/Algorithm.h"

using Rainbow::equal;

TEST(AlgorithmTest, ApproximatelyEqualFloats)
{
	ASSERT_TRUE(equal(0.0f, 0.0f));
	ASSERT_FALSE(equal(0.0f, 0.00001f));
	ASSERT_FALSE(equal(0.0f, -0.00001f));
	ASSERT_TRUE(equal(3.14285714f, 22.0f / 7.0f));
}

TEST(AlgorithmTest, DegreesToRadians)
{
	ASSERT_TRUE(equal(kPi, Rainbow::radians(Rainbow::degrees(kPi))));
}

TEST(AlgorithmTest, IsPowerOfTwo)
{
	unsigned int p = 1;
	for (unsigned int i = 0; i < 100; ++i)
	{
		if (i == p)
		{
			ASSERT_TRUE(Rainbow::is_pow2(i));
			p *= 2;
		}
		else
			ASSERT_FALSE(Rainbow::is_pow2(i));
	}
}

TEST(AlgorithmTest, MaxMin)
{
	ASSERT_EQ(2, Rainbow::max(1, 2));
	ASSERT_EQ(1, Rainbow::max(1, 1));
	ASSERT_EQ(1, Rainbow::max(1, 0));
	ASSERT_EQ(1, Rainbow::min(1, 2));
	ASSERT_EQ(1, Rainbow::min(1, 1));
	ASSERT_EQ(0, Rainbow::min(1, 0));
}

TEST(AlgorithmTest, NextPowerOfTwo)
{
	unsigned int p = 1;
	for (unsigned int i = 1; i < 100; ++i)
	{
		ASSERT_EQ(p, Rainbow::next_pow2(i));
		if (i == p)
			p *= 2;
	}
}

TEST(AlgorithmTest, RadiansToDegrees)
{
	ASSERT_TRUE(equal(0.05483f, Rainbow::degrees(Rainbow::radians(0.05483f))));
}

TEST(AlgorithmTest, Swap)
{
	float a = kPi;
	float b = kPi_2;
	Rainbow::swap(a, b);
	ASSERT_EQ(kPi, b);
	ASSERT_EQ(kPi_2, a);
	Rainbow::swap(a, b);
	ASSERT_EQ(kPi, a);
	ASSERT_EQ(kPi_2, b);
}

TEST(AlgorithmTest, UTF8ToUTF32)
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
			ASSERT_EQ(utf32[i], kInvalidCharacter);
			++l;
			continue;
		}
		ASSERT_EQ(utf32[i], c.code);
		l += c.bytes;
	}
}
