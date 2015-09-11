// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <catch.hpp>

#include "Common/Link.h"

namespace
{
	struct Number : public Link
	{
		int value;

		Number(int v) : value(v) {}

		Number* next() const { return static_cast<Number*>(Link::next()); }
	};
}

TEST_CASE("Link construction", "[link]")
{
	Number n{0};
	REQUIRE(n.value == 0);
	REQUIRE(n.next() == nullptr);
	REQUIRE(n.prev() == nullptr);
}

TEST_CASE("Link operations", "[link]")
{
	Number numbers[]{Number{1}, Number{2}, Number{3}, Number{4}};
	Number n{0};

	SECTION("append() inserts item after current")
	{
		std::for_each(numbers, numbers + 4, [&n](Number &m) {
			const auto o = n.next();
			REQUIRE(m.prev() == nullptr);
			n.append(&m);
			REQUIRE(m.prev() == &n);
			REQUIRE(m.next() == o);
			REQUIRE(n.prev() == nullptr);
			REQUIRE(n.next() == &m);
		});
		REQUIRE(n.value == 0);
		REQUIRE(n.next() == numbers + 3);
		auto m = n.next();
		for (int i = 4; i > 0; --i)
		{
			REQUIRE(m->value == i);
			m = m->next();
		}
	}

	SECTION("append() pops the item before insertion")
	{
		std::for_each(numbers, numbers + 4, [&n](Number &m) { n.append(&m); });
		REQUIRE(n.next() == numbers + 3);
		REQUIRE(numbers[3].prev() == &n);
		REQUIRE(numbers[3].next() == numbers + 2);
		REQUIRE(numbers[0].next() == nullptr);
		numbers[0].append(numbers + 3);
		REQUIRE(numbers[0].next() == numbers + 3);
		REQUIRE(numbers[3].prev() == numbers);
		REQUIRE(numbers[3].next() == nullptr);
		REQUIRE(n.next() == numbers + 2);
		REQUIRE(numbers[2].prev() == &n);
	}

	SECTION("pop() removes itself from the list")
	{
		std::for_each(numbers, numbers + 4, [&n](Number &m) { n.append(&m); });

		REQUIRE(n.next() == numbers + 3);
		REQUIRE(n.prev() == nullptr);
		REQUIRE(numbers[3].next() == numbers + 2);
		REQUIRE(numbers[3].prev() == &n);
		n.pop();
		REQUIRE(n.value == 0);
		REQUIRE(n.next() == nullptr);
		REQUIRE(numbers[3].prev() == nullptr);

		REQUIRE(numbers[3].next() == numbers + 2);
		REQUIRE(numbers[2].prev() == numbers + 3);
		REQUIRE(numbers[2].next() == numbers + 1);
		REQUIRE(numbers[1].prev() == numbers + 2);
		numbers[2].pop();
		REQUIRE(numbers[2].prev() == nullptr);
		REQUIRE(numbers[2].next() == nullptr);
		REQUIRE(numbers[3].next() == numbers + 1);
		REQUIRE(numbers[1].prev() == numbers + 3);

		REQUIRE(numbers[1].next() == numbers);
		REQUIRE(numbers[0].prev() == numbers + 1);
		REQUIRE(numbers[0].next() == nullptr);
		numbers[0].pop();
		REQUIRE(numbers[1].next() == nullptr);
		REQUIRE(numbers[1].prev() == numbers + 3);
		REQUIRE(numbers[0].next() == nullptr);
		REQUIRE(numbers[0].prev() == nullptr);
	}
}

TEST_CASE("Link pops itself on destruction", "[link]")
{
	Number a{0};
	REQUIRE(a.next() == nullptr);
	{
		Number b{1};
		a.append(&b);
		REQUIRE(a.next() == &b);
		REQUIRE(b.prev() == &a);
		REQUIRE(b.next() == nullptr);
		{
			Number c{1};
			b.append(&c);
			REQUIRE(b.next() == &c);
			REQUIRE(c.prev() == &b);
			REQUIRE(b.prev() == &a);
			REQUIRE(a.next() == &b);
		}
		REQUIRE(b.next() == nullptr);
		REQUIRE(b.prev() == &a);
	}
	REQUIRE(a.next() == nullptr);
}

TEST_CASE("Traverse Links with for_each", "[link]")
{
	Number numbers[]{Number{0}, Number{1}, Number{2}, Number{3}, Number{4}};
	numbers[0].append(&numbers[4]);
	numbers[0].append(&numbers[3]);
	numbers[0].append(&numbers[2]);
	numbers[0].append(&numbers[1]);

	int prev = -1;
	REQUIRE_FALSE(for_each(numbers, [&prev](const Number *n) {
		REQUIRE(n->value > prev);
		prev = n->value;
		return false;
	}));
	REQUIRE(prev == numbers[4].value);

	const int max = 3;
	REQUIRE(for_each(numbers, [max, &prev](const Number *n) {
		prev = n->value;
		return n->value >= max;
	}));
	REQUIRE(prev == max);
}
