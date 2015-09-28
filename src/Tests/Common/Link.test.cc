// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

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

TEST(LinkTest, Constructs)
{
	const Number n{0};

	ASSERT_EQ(0, n.value);
	ASSERT_EQ(nullptr, n.next());
	ASSERT_EQ(nullptr, n.prev());
}

TEST(LinkTest, AppendsItemAfterCurrent)
{
	Number numbers[]{Number{1}, Number{2}, Number{3}, Number{4}};
	Number n{0};

	std::for_each(numbers, numbers + 4, [&n](Number& m) {
		const auto o = n.next();
		ASSERT_EQ(nullptr, m.prev());

		n.append(&m);

		ASSERT_EQ(&n, m.prev());
		ASSERT_EQ(o, m.next());
		ASSERT_EQ(nullptr, n.prev());
		ASSERT_EQ(&m, n.next());
	});

	ASSERT_EQ(0, n.value);
	ASSERT_EQ(numbers + 3, n.next());

	auto m = n.next();
	for (int i = 4; i > 0; --i)
	{
		ASSERT_EQ(i, m->value);
		m = m->next();
	}
}

TEST(LinkTest, PopsItemBeforeInsertion)
{
	Number numbers[]{Number{1}, Number{2}, Number{3}, Number{4}};
	Number n{0};

	std::for_each(numbers, numbers + 4, [&n](Number& m) { n.append(&m); });

	ASSERT_EQ(numbers + 3, n.next());
	ASSERT_EQ(&n, numbers[3].prev());
	ASSERT_EQ(numbers + 2, numbers[3].next());
	ASSERT_EQ(nullptr, numbers[0].next());

	numbers[0].append(numbers + 3);

	ASSERT_EQ(numbers + 3, numbers[0].next());
	ASSERT_EQ(numbers, numbers[3].prev());
	ASSERT_EQ(nullptr, numbers[3].next());
	ASSERT_EQ(numbers + 2, n.next());
	ASSERT_EQ(&n, numbers[2].prev());
}

TEST(LinkTest, RemovesItselfWhenPopped)
{
	Number numbers[]{Number{1}, Number{2}, Number{3}, Number{4}};
	Number n{0};

	std::for_each(numbers, numbers + 4, [&n](Number& m) { n.append(&m); });

	ASSERT_EQ(numbers + 3, n.next());
	ASSERT_EQ(nullptr, n.prev());
	ASSERT_EQ(numbers + 2, numbers[3].next());
	ASSERT_EQ(&n, numbers[3].prev());

	n.pop();

	ASSERT_EQ(0, n.value);
	ASSERT_EQ(nullptr, n.next());
	ASSERT_EQ(nullptr, numbers[3].prev());

	ASSERT_EQ(numbers + 2, numbers[3].next());
	ASSERT_EQ(numbers + 3, numbers[2].prev());
	ASSERT_EQ(numbers + 1, numbers[2].next());
	ASSERT_EQ(numbers + 2, numbers[1].prev());

	numbers[2].pop();

	ASSERT_EQ(nullptr, numbers[2].prev());
	ASSERT_EQ(nullptr, numbers[2].next());
	ASSERT_EQ(numbers + 1, numbers[3].next());
	ASSERT_EQ(numbers + 3, numbers[1].prev());

	ASSERT_EQ(numbers, numbers[1].next());
	ASSERT_EQ(numbers + 1, numbers[0].prev());
	ASSERT_EQ(nullptr, numbers[0].next());

	numbers[0].pop();

	ASSERT_EQ(nullptr, numbers[1].next());
	ASSERT_EQ(numbers + 3, numbers[1].prev());
	ASSERT_EQ(nullptr, numbers[0].next());
	ASSERT_EQ(nullptr, numbers[0].prev());
}

TEST(LinkTest, PopsItselfOnDestruction)
{
	Number a{0};
	ASSERT_EQ(nullptr, a.next());
	{
		Number b{1};
		a.append(&b);
		ASSERT_EQ(&b, a.next());
		ASSERT_EQ(&a, b.prev());
		ASSERT_EQ(nullptr, b.next());
		{
			Number c{1};
			b.append(&c);
			ASSERT_EQ(&c, b.next());
			ASSERT_EQ(&b, c.prev());
			ASSERT_EQ(&a, b.prev());
			ASSERT_EQ(&b, a.next());
		}
		ASSERT_EQ(nullptr, b.next());
		ASSERT_EQ(&a, b.prev());
	}
	ASSERT_EQ(nullptr, a.next());
}

TEST(LinkTest, ForEachTraversesLinks)
{
	Number numbers[]{Number{0}, Number{1}, Number{2}, Number{3}, Number{4}};
	numbers[0].append(&numbers[4]);
	numbers[0].append(&numbers[3]);
	numbers[0].append(&numbers[2]);
	numbers[0].append(&numbers[1]);

	int prev = -1;
	ASSERT_FALSE(for_each(numbers, [&prev](const Number* n) {
		[&] { ASSERT_GT(n->value, prev); }();
		prev = n->value;
		return false;
	}));
	ASSERT_EQ(numbers[4].value, prev);

	const int max = 3;
	ASSERT_TRUE(for_each(numbers, [max, &prev](const Number* n) {
		prev = n->value;
		return n->value >= max;
	}));
	ASSERT_EQ(max, prev);
}
