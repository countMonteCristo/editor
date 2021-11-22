#include <gtest/gtest.h>

#include "la.hpp"


class VecFixture : public ::testing::Test {
protected:
    void SetUp() override {
        v1 = Vec2i(3, 4);
        v2 = Vec2i(2, 1);
    }

    Vec2i v1;
    Vec2i v2;
};

TEST_F(VecFixture, VecOperations) {
    Vec2i x1 = v1 + v2;
    EXPECT_EQ(x1.x, v1.x + v2.x);
    EXPECT_EQ(x1.y, v1.y + v2.y);

    Vec2i x2 = v1 - v2;
    EXPECT_EQ(x2.x, v1.x - v2.x);
    EXPECT_EQ(x2.y, v1.y - v2.y);

    Vec2i x3 = v1 * v2;
    EXPECT_EQ(x3.x, v1.x * v2.x);
    EXPECT_EQ(x3.y, v1.y * v2.y);

    Vec2i x4 = v1 / v2;
    EXPECT_EQ(x4.x, v1.x / v2.x);
    EXPECT_EQ(x4.y, v1.y / v2.y);

    Vec2i x5(-2, 1);
    Vec2i x6 = x5;
    x5 += v1;
    EXPECT_EQ(x5.x, x6.x + v1.x);
    EXPECT_EQ(x5.y, x6.y + v1.y);

    Vec2i x7(-2, 1);
    Vec2i x8 = x7;
    x7 -= v1;
    EXPECT_EQ(x7.x, x8.x - v1.x);
    EXPECT_EQ(x7.y, x8.y - v1.y);

    Vec2i x9(-2, 1);
    Vec2i x10 = x9;
    x9 *= v1;
    EXPECT_EQ(x9.x, x10.x * v1.x);
    EXPECT_EQ(x9.y, x10.y * v1.y);

    Vec2i x11(-2, 1);
    Vec2i x12 = x11;
    x11 /= v1;
    EXPECT_EQ(x11.x, x12.x / v1.x);
    EXPECT_EQ(x11.y, x12.y / v1.y);

    Vec2i x13(4, 5);
    Vec2i x14(4, 5);
    EXPECT_EQ(x13, x14);

    EXPECT_EQ(bounded(-5, 6, -6), -5);
    EXPECT_EQ(bounded(-5, 6, 7), 6);
    EXPECT_EQ(bounded(-5, 6, 2), 2);

    EXPECT_DEATH(bounded(6, -5, 2), "Assertion `max_value >= min_value' failed");

}
