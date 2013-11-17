// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Vector.h"

class VectorTest : public testing::Test
{
protected:
	const size_t test_size;
	Vector<size_t> intvec;

	VectorTest() : test_size(100)
	{
		// Initialise vector with some content
		for (size_t i = 0; i < this->test_size; ++i)
			this->intvec.push_back(i);
	}
};

TEST_F(VectorTest, Access)
{
	for (size_t i = 0; i < test_size; ++i)
	{
		ASSERT_EQ(i, intvec.at(i));
		ASSERT_EQ(i, intvec[i]);
	}
}

TEST_F(VectorTest, Begin)
{
	ASSERT_EQ(&intvec[0], intvec.begin());
}

TEST_F(VectorTest, Clear)
{
	const size_t capacity = intvec.capacity();
	ASSERT_EQ(test_size, intvec.size());
	intvec.clear();
	ASSERT_EQ(0u, intvec.size());
	ASSERT_EQ(capacity, intvec.capacity());
}

TEST_F(VectorTest, End)
{
	size_t count = 0;
	for (auto i : intvec)
	{
		ASSERT_EQ(count, i);
		++count;
	}
	ASSERT_EQ(test_size, count);
	ASSERT_NE(intvec.begin(), intvec.end());
	intvec.clear();
	ASSERT_EQ(intvec.begin(), intvec.end());
}

TEST_F(VectorTest, EraseFirstElement)
{
	for (size_t i = 0; i < intvec.size(); ++i)
		ASSERT_EQ(i, intvec[i]);

	intvec.erase(0);
	ASSERT_EQ(-1, intvec.find(0));

	for (size_t i = 0; i < intvec.size(); ++i)
		ASSERT_EQ(i + 1, intvec[i]);
}

TEST_F(VectorTest, EraseLastElement)
{
	for (size_t i = 0; i < intvec.size(); ++i)
		ASSERT_EQ(i, intvec[i]);

	const size_t last = intvec.size() - 1;
	intvec.erase(last);
	ASSERT_EQ(-1, intvec.find(last));

	for (size_t i = 0; i < intvec.size(); ++i)
		ASSERT_EQ(i, intvec[i]);
}

TEST_F(VectorTest, EraseMiddleElement)
{
	for (size_t i = 0; i < intvec.size(); ++i)
		ASSERT_EQ(i, intvec[i]);

	const size_t fifth = 5;
	intvec.erase(fifth);
	ASSERT_EQ(-1, intvec.find(fifth));

	for (size_t i = 0; i < intvec.size(); ++i)
	{
		if (i >= fifth)
			ASSERT_EQ(i + 1, intvec[i]);
		else
			ASSERT_EQ(i, intvec[i]);
	}
}

TEST_F(VectorTest, PushBackPastCapacity)
{
	ASSERT_LT(test_size, intvec.capacity());
	ASSERT_EQ(test_size, intvec.size());
	for (size_t i = 0; i < test_size; ++i)
		ASSERT_EQ(i, intvec[i]);
}

TEST_F(VectorTest, QuickEraseFirstElement)
{
	for (size_t i = 0; i < intvec.size(); ++i)
		ASSERT_EQ(i, intvec[i]);

	const size_t last = intvec.size() - 1;
	intvec.qerase(0);
	ASSERT_EQ(last, intvec[0]);
	ASSERT_EQ(-1, intvec.find(0));

	for (size_t i = 1; i < intvec.size(); ++i)
		ASSERT_EQ(i, intvec[i]);
}

TEST_F(VectorTest, QuickEraseLastElement)
{
	for (size_t i = 0; i < intvec.size(); ++i)
		ASSERT_EQ(i, intvec[i]);

	const size_t last = intvec.size() - 1;
	intvec.qerase(last);
	ASSERT_EQ(-1, intvec.find(last));

	for (size_t i = 0; i < intvec.size(); ++i)
		ASSERT_EQ(i, intvec[i]);
}

TEST_F(VectorTest, QuickEraseMiddleElement)
{
	for (size_t i = 0; i < intvec.size(); ++i)
		ASSERT_EQ(i, intvec[i]);

	const size_t height = intvec.size() >> 1;
	const size_t last = intvec.size() - 1;
	intvec.qerase(height);
	ASSERT_EQ(-1, intvec.find(height));

	for (size_t i = 0; i < height; ++i)
		ASSERT_EQ(i, intvec[i]);
	ASSERT_EQ(last, intvec[height]);
	for (size_t i = height + 1; i < intvec.size(); ++i)
		ASSERT_EQ(i, intvec[i]);
}

TEST_F(VectorTest, Find)
{
	ASSERT_EQ(-1, intvec.find(-1));
	size_t i = 0;
	for (; i < intvec.size(); ++i)
	{
		const int needle = intvec.find(i);
		ASSERT_LE(0, needle);
		ASSERT_EQ(i, static_cast<size_t>(i));
	}
	ASSERT_EQ(-1, intvec.find(i));
}

TEST_F(VectorTest, ReserveLessThanSize)
{
	intvec.reserve(0);
	ASSERT_EQ(test_size, intvec.capacity());
	ASSERT_EQ(test_size, intvec.size());
	for (size_t i = 0; i < test_size; ++i)
		ASSERT_EQ(i, intvec[i]);
}

TEST_F(VectorTest, ReserveGreaterThanSize)
{
	const size_t cap = test_size + 10;
	intvec.reserve(cap);
	ASSERT_EQ(cap, intvec.capacity());
	ASSERT_EQ(test_size, intvec.size());
	for (size_t i = test_size; i < cap; ++i)
		intvec.push_back(i);
	ASSERT_EQ(cap, intvec.capacity());
	ASSERT_EQ(cap, intvec.size());
}

TEST_F(VectorTest, Assignment)
{
	for (auto &i : intvec)
		i = 0xb00bbabe;
	for (const auto &i : intvec)
		ASSERT_EQ(0xb00bbabe, i);
}
