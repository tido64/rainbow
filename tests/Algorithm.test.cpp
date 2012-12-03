#include "Algorithm.h"

using Rainbow::equalf;

TEST(AlgorithmTest, ApproximatelyEqualFloats)
{
	ASSERT_TRUE(equalf(0.0f, 0.0f));
	ASSERT_FALSE(equalf(0.0f, 0.1f));
	ASSERT_FALSE(equalf(0.0f, -0.1f));
	ASSERT_TRUE(equalf(3.14285714f, 22.0f / 7.0f));
}

TEST(AlgorithmTest, DegreesToRadians)
{
	ASSERT_TRUE(equalf(3.14f, Rainbow::radians(Rainbow::degrees(3.14f))));
}

TEST(AlgorithmTest, DistanceBetweenTwoPoints)
{
	ASSERT_EQ(5.0f, Rainbow::distance(0.0f, 0.0f, 3.0f, 4.0f));
}

TEST(AlgorithmTest, IntegerToString)
{
	char *number = Rainbow::itoa(65535);
	ASSERT_STREQ("65535", number);
	delete[] number;

	number = Rainbow::itoa(-65535);
	ASSERT_STREQ("-65535", number);
	delete[] number;
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
	ASSERT_TRUE(equalf(0.05483f, Rainbow::degrees(Rainbow::radians(0.05483f))));
}

TEST(AlgorithmTest, UTF8ToUTF32)
{
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
	const unsigned long utf32[] = {
		utf8[0],
		utf8[1],
		static_cast<unsigned long>(-1),
		static_cast<unsigned long>(-1),
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

	size_t bytes;
	const unsigned char *l = utf8;
	for (size_t i = 0; utf32[i]; ++i)
	{
		ASSERT_EQ(utf32[i], Rainbow::utf8_decode(l, bytes));
		l += !bytes ? 1 : bytes;
	}
}
