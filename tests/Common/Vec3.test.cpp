#include "Common/Vec3.h"

TEST(Vec3Test, OperatorAdd)
{
	Vec3f v0(1.0f, 2.0f, 3.0f);
	Vec3f v1(4.0f, 5.0f, 6.0f);
	v0 += v1;
	ASSERT_FLOAT_EQ(5.0f, v0.x);
	ASSERT_FLOAT_EQ(7.0f, v0.y);
	ASSERT_FLOAT_EQ(9.0f, v0.z);
}

TEST(Vec3Test, OperatorEq)
{
	Vec3f v0;
	Vec3f v1(4.0f, 5.0f, 6.0f);
	v0 = v1;
	ASSERT_FLOAT_EQ(4.0f, v0.x);
	ASSERT_FLOAT_EQ(5.0f, v0.y);
	ASSERT_FLOAT_EQ(6.0f, v0.z);
}

TEST(Vec3Test, OperatorMul)
{
	Vec3f v0(1.0f, 2.0f, 3.0f);
	v0 *= 2;
	ASSERT_FLOAT_EQ(2.0f, v0.x);
	ASSERT_FLOAT_EQ(4.0f, v0.y);
	ASSERT_FLOAT_EQ(6.0f, v0.z);
}

TEST(Vec3Test, OperatorSub)
{
	Vec3f v0(1, 2, 3);
	Vec3f v1(4, 5, 6);
	v0 -= v1;
	ASSERT_FLOAT_EQ(-3.0f, v0.x);
	ASSERT_FLOAT_EQ(-3.0f, v0.y);
	ASSERT_FLOAT_EQ(-3.0f, v0.z);
}

TEST(Vec3Test, Zero)
{
	Vec3f v0;
	ASSERT_FLOAT_EQ(0.0f, v0.x);
	ASSERT_FLOAT_EQ(0.0f, v0.y);
	ASSERT_FLOAT_EQ(0.0f, v0.z);

	v0.x = 1.0f;
	v0.y = 2.0f;
	v0.z = 3.0f;
	ASSERT_FALSE(v0.is_zero());
}
