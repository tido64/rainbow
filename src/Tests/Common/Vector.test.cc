// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Vector.h"

namespace
{
	struct VectorConstructionTest
	{
		const bool was_constructed;
		const bool was_copied;
		const bool was_moved;

		VectorConstructionTest()
		    : was_constructed(true), was_copied(false), was_moved(false) { }

		VectorConstructionTest(const VectorConstructionTest& v)
		    : was_constructed(v.was_constructed), was_copied(true),
		      was_moved(false) { }

		VectorConstructionTest(VectorConstructionTest&& v)
		    : was_constructed(v.was_constructed), was_copied(false),
		      was_moved(true) { }
	};
}

// TODO: The following newlines were added for Catch to create unique names.

TEST_CASE("Vector operations", "[vector]")
{
	class BloatedInteger
	{
	public:
		BloatedInteger(const size_t i) : integer_(i) { }

		operator size_t() const { return integer_; };

	private:
		char random_padding_[13];
		size_t integer_;
	};

	const size_t kTestSize = 16;
	Vector<BloatedInteger> intvec;
	for (size_t i = 0; i < kTestSize; ++i)
		intvec.emplace_back(i);

	SECTION("Access")
	{
		for (size_t i = 0; i < kTestSize; ++i)
		{
			REQUIRE(intvec.at(i) == i);
			REQUIRE(intvec[i] == i);
		}
	}

	SECTION("begin() returns a pointer to the first element")
	{
		REQUIRE(intvec.begin() == &intvec[0]);
	}

	SECTION("Clear deletes all elements but keeps the block of memory")
	{
		const size_t capacity = intvec.capacity();
		REQUIRE(intvec.size() == kTestSize);
		intvec.clear();
		REQUIRE(intvec.size() == 0u);
		REQUIRE(intvec.capacity() == capacity);
	}

	SECTION("end() returns a pointer past the last element")
	{
		size_t count = 0;
		for (auto i : intvec)
		{
			REQUIRE(i == count);
			++count;
		}
		REQUIRE(count == kTestSize);
		REQUIRE(intvec.begin() != intvec.end());
		intvec.clear();
		REQUIRE(intvec.begin() == intvec.end());
	}

	SECTION("erase(0) erases first element and keeps the order")
	{
		for (size_t i = 0; i < intvec.size(); ++i)
			REQUIRE(intvec[i] == i);

		intvec.erase(0);
		REQUIRE(intvec.size() == kTestSize - 1);
		REQUIRE(intvec.find(0) == -1);

		for (size_t i = 0; i < intvec.size(); ++i)
			REQUIRE(intvec[i] == i + 1);
	}

	SECTION("erase(size() - 1) erases last element and keeps the order")
	{
		for (size_t i = 0; i < intvec.size(); ++i)
			REQUIRE(intvec[i] == i);

		const size_t last = intvec.size() - 1;
		intvec.erase(last);
		REQUIRE(intvec.find(last) == -1);

		for (size_t i = 0; i < intvec.size(); ++i)
			REQUIRE(intvec[i] == i);
	}

	SECTION("erase(n) erases an element and keeps the order")
	{
		for (size_t i = 0; i < intvec.size(); ++i)
			REQUIRE(intvec[i] == i);

		const size_t fifth = 5;
		intvec.erase(fifth);
		REQUIRE(intvec.find(fifth) == -1);

		for (size_t i = 0; i < intvec.size(); ++i)
		{
			if (i >= fifth)
				REQUIRE(intvec[i] == i + 1);
			else
				REQUIRE(intvec[i] == i);
		}
	}

	SECTION("Pushing passed capacity increases it")
	{
		REQUIRE(intvec.capacity() > kTestSize);
		REQUIRE(intvec.size() == kTestSize);
		for (size_t i = 0; i < kTestSize; ++i)
			REQUIRE(intvec[i] == i);
	}

	SECTION("qerase(0) replaces first element with last and deletes it")
	{
		for (size_t i = 0; i < intvec.size(); ++i)
			REQUIRE(intvec[i] == i);

		const size_t last = intvec.size() - 1;
		intvec.qerase(0);
		REQUIRE(intvec[0] == last);
		REQUIRE(intvec.find(0) == -1);

		for (size_t i = 1; i < intvec.size(); ++i)
			REQUIRE(intvec[i] == i);
	}

	SECTION("qerase(size() - 1) deletes last element")
	{
		for (size_t i = 0; i < intvec.size(); ++i)
			REQUIRE(intvec[i] == i);

		const size_t last = intvec.size() - 1;
		intvec.qerase(last);
		REQUIRE(intvec.find(last) == -1);

		for (size_t i = 0; i < intvec.size(); ++i)
			REQUIRE(intvec[i] == i);
	}

	SECTION("qerase(n) replaces n-th element with last and deletes it")
	{
		for (size_t i = 0; i < intvec.size(); ++i)
			REQUIRE(intvec[i] == i);

		const size_t height = intvec.size() >> 1;
		const size_t last = intvec.size() - 1;
		intvec.qerase(height);
		REQUIRE(intvec.find(height) == -1);

		for (size_t i = 0; i < height; ++i)
			REQUIRE(intvec[i] == i);
		REQUIRE(intvec[height] == last);
		for (size_t i = height + 1; i < intvec.size(); ++i)
			REQUIRE(intvec[i] == i);
	}

	SECTION("Find returns index of value; -1 if not found")
	{
		REQUIRE(intvec.find(-1) == -1);
		size_t i = 0;
		for (; i < intvec.size(); ++i)
		{
			const int needle = intvec.find(i);
			REQUIRE(needle >= 0);
			REQUIRE(static_cast<size_t>(i) == i);
		}
		REQUIRE(intvec.find(i) == -1);
	}

	SECTION("Reserving capacity less than size() tightens the vector")
	{
		intvec.reserve(0);
		REQUIRE(intvec.capacity() == kTestSize);
		REQUIRE(intvec.size() == kTestSize);
		for (size_t i = 0; i < kTestSize; ++i)
			REQUIRE(intvec[i] == i);
	}

	SECTION("Reserving capacity greater than size()")
	{
		const size_t cap = kTestSize + 10;
		intvec.reserve(cap);
		REQUIRE(intvec.capacity() == cap);
		REQUIRE(intvec.size() == kTestSize);
		for (size_t i = kTestSize; i < cap; ++i)
			intvec.emplace_back(i);
		REQUIRE(intvec.capacity() == cap);
		REQUIRE(intvec.size() == cap);
	}

	SECTION("Assignment")
	{
		for (auto &i : intvec)
			i = 0xb00bbabe;
		for (const auto &i : intvec)
			REQUIRE(i == 0xb00bbabe);
	}
}

TEST_CASE("Vector implements move semantics", "[vector]")
{
	VectorConstructionTest obj;
	REQUIRE(obj.was_constructed);
	REQUIRE_FALSE(obj.was_copied);
	REQUIRE_FALSE(obj.was_moved);

	Vector<VectorConstructionTest> vec;

	vec.push_back(obj);
	REQUIRE(vec[0].was_constructed);
	REQUIRE(vec[0].was_copied);
	REQUIRE_FALSE(vec[0].was_moved);

	vec.push_back(std::move(obj));
	REQUIRE(vec[1].was_constructed);
	REQUIRE_FALSE(vec[1].was_copied);
	REQUIRE(vec[1].was_moved);

	vec.push_back(VectorConstructionTest());
	REQUIRE(vec[2].was_constructed);
	REQUIRE_FALSE(vec[2].was_copied);
	REQUIRE(vec[2].was_moved);

	vec.emplace_back();
	REQUIRE(vec[3].was_constructed);
	REQUIRE_FALSE(vec[3].was_copied);
	REQUIRE_FALSE(vec[3].was_moved);
}
