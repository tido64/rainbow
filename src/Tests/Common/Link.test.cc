// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Link.h"

#include <gtest/gtest.h>

namespace
{
    struct Number : public rainbow::Link<Number>
    {
        int value;

        explicit Number(int v) : value(v) {}
    };
}  // namespace

TEST(LinkTest, Constructs)
{
    const Number n{0};

    ASSERT_EQ(n.value, 0);
    ASSERT_EQ(n.next(), nullptr);
    ASSERT_EQ(n.prev(), nullptr);
}

TEST(LinkTest, AppendsItemAfterCurrent)
{
    Number numbers[]{Number{1}, Number{2}, Number{3}, Number{4}};
    Number n{0};

    std::for_each(numbers, numbers + 4, [&n](Number& m) {
        const auto o = n.next();
        ASSERT_EQ(m.prev(), nullptr);

        n.append(m);

        ASSERT_EQ(m.prev(), &n);
        ASSERT_EQ(m.next(), o);
        ASSERT_EQ(n.prev(), nullptr);
        ASSERT_EQ(n.next(), &m);
    });

    ASSERT_EQ(n.value, 0);
    ASSERT_EQ(n.next(), numbers + 3);

    auto m = n.next();
    for (int i = 4; i > 0; --i)
    {
        ASSERT_EQ(m->value, i);
        m = m->next();
    }
}

TEST(LinkTest, PopsItemBeforeInsertion)
{
    Number numbers[]{Number{1}, Number{2}, Number{3}, Number{4}};
    Number n{0};

    std::for_each(numbers, numbers + 4, [&n](Number& m) { n.append(m); });

    ASSERT_EQ(n.next(), numbers + 3);
    ASSERT_EQ(numbers[3].prev(), &n);
    ASSERT_EQ(numbers[3].next(), numbers + 2);
    ASSERT_EQ(numbers[0].next(), nullptr);

    numbers[0].append(numbers[3]);

    ASSERT_EQ(numbers[0].next(), numbers + 3);
    ASSERT_EQ(numbers[3].prev(), numbers);
    ASSERT_EQ(numbers[3].next(), nullptr);
    ASSERT_EQ(n.next(), numbers + 2);
    ASSERT_EQ(numbers[2].prev(), &n);
}

TEST(LinkTest, RemovesItselfWhenPopped)
{
    Number numbers[]{Number{1}, Number{2}, Number{3}, Number{4}};
    Number n{0};

    std::for_each(numbers, numbers + 4, [&n](Number& m) { n.append(m); });

    ASSERT_EQ(n.next(), numbers + 3);
    ASSERT_EQ(n.prev(), nullptr);
    ASSERT_EQ(numbers[3].next(), numbers + 2);
    ASSERT_EQ(numbers[3].prev(), &n);

    n.pop();

    ASSERT_EQ(n.value, 0);
    ASSERT_EQ(n.next(), nullptr);
    ASSERT_EQ(numbers[3].prev(), nullptr);

    ASSERT_EQ(numbers[3].next(), numbers + 2);
    ASSERT_EQ(numbers[2].prev(), numbers + 3);
    ASSERT_EQ(numbers[2].next(), numbers + 1);
    ASSERT_EQ(numbers[1].prev(), numbers + 2);

    numbers[2].pop();

    ASSERT_EQ(numbers[2].prev(), nullptr);
    ASSERT_EQ(numbers[2].next(), nullptr);
    ASSERT_EQ(numbers[3].next(), numbers + 1);
    ASSERT_EQ(numbers[1].prev(), numbers + 3);

    ASSERT_EQ(numbers[1].next(), numbers);
    ASSERT_EQ(numbers[0].prev(), numbers + 1);
    ASSERT_EQ(numbers[0].next(), nullptr);

    numbers[0].pop();

    ASSERT_EQ(numbers[1].next(), nullptr);
    ASSERT_EQ(numbers[1].prev(), numbers + 3);
    ASSERT_EQ(numbers[0].next(), nullptr);
    ASSERT_EQ(numbers[0].prev(), nullptr);
}

TEST(LinkTest, PopsItselfOnDestruction)
{
    Number a{0};
    ASSERT_EQ(a.next(), nullptr);
    {
        Number b{1};
        a.append(b);
        ASSERT_EQ(a.next(), &b);
        ASSERT_EQ(b.prev(), &a);
        ASSERT_EQ(b.next(), nullptr);
        {
            Number c{1};
            b.append(c);
            ASSERT_EQ(b.next(), &c);
            ASSERT_EQ(c.prev(), &b);
            ASSERT_EQ(b.prev(), &a);
            ASSERT_EQ(a.next(), &b);
        }
        ASSERT_EQ(b.next(), nullptr);
        ASSERT_EQ(b.prev(), &a);
    }
    ASSERT_EQ(a.next(), nullptr);
}

TEST(LinkTest, ForEachTraversesLinks)
{
    static constexpr int kMax = 3;

    Number numbers[]{Number{0}, Number{1}, Number{2}, Number{3}, Number{4}};
    numbers[0].append(numbers[4]);
    numbers[0].append(numbers[3]);
    numbers[0].append(numbers[2]);
    numbers[0].append(numbers[1]);

    int prev = -1;
    ASSERT_FALSE(for_each(numbers, [&prev](const Number& n) {
        [&] { ASSERT_GT(n.value, prev); }();
        prev = n.value;
        return false;
    }));
    ASSERT_EQ(prev, numbers[4].value);

    ASSERT_TRUE(for_each(numbers, [&prev](const Number& n) {
        prev = n.value;
        return n.value >= kMax;
    }));
    ASSERT_EQ(prev, kMax);
}
