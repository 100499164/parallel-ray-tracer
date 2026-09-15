#include "color.hpp"
#include "gtest/gtest.h"
#include <string>

using namespace render;

// ID: COLOR1
TEST(ColorTest, OperatorEqualsTrue) {
  Color const c1(0.1, 0.2, 0.3);
  Color const c2(0.1, 0.2, 0.3);
  ASSERT_TRUE(c1 == c2);
}

// ID: COLOR2
TEST(ColorTest, OperatorEqualsFalse) {
  Color const c1(0.1, 0.2, 0.3);
  Color const c2(0.4, 0.2, 0.3);
  ASSERT_FALSE(c1 == c2);
}

// ID: COLOR3
TEST(ColorTest, DefaultConstructor) {
  Color const c_default{};
  ASSERT_DOUBLE_EQ(c_default.r, 0.0);
  ASSERT_DOUBLE_EQ(c_default.g, 0.0);
  ASSERT_DOUBLE_EQ(c_default.b, 0.0);
}

// ID: COLOR4
TEST(ColorTest, OperatorPlusEquals) {
  Color c1(0.1, 0.2, 0.3);
  c1 += Color(0.5, 0.6, 0.7);
  ASSERT_DOUBLE_EQ(c1.r, 0.6);
  ASSERT_DOUBLE_EQ(c1.g, 0.8);
  ASSERT_DOUBLE_EQ(c1.b, 1.0);
}

// ID: COLOR5
TEST(ColorTest, OperatorMultiplyEqualsColor) {
  Color c1(0.5, 1.0, 0.2);
  c1 *= Color(0.5, 0.8, 0.5);
  ASSERT_DOUBLE_EQ(c1.r, 0.25);
  ASSERT_DOUBLE_EQ(c1.g, 0.8);
  ASSERT_DOUBLE_EQ(c1.b, 0.1);
}

// ID: COLOR6
TEST(ColorTest, OperatorMultiplyEqualsScalar) {
  Color c1(0.2, 0.4, 0.8);
  c1 *= 0.5;
  ASSERT_DOUBLE_EQ(c1.r, 0.1);
  ASSERT_DOUBLE_EQ(c1.g, 0.2);
  ASSERT_DOUBLE_EQ(c1.b, 0.4);
}

// ID: COLOR7
TEST(ColorTest, OperatorDivideEqualsScalar) {
  Color c1(0.2, 0.4, 0.8);
  c1 /= 2.0;
  ASSERT_DOUBLE_EQ(c1.r, 0.1);
  ASSERT_DOUBLE_EQ(c1.g, 0.2);
  ASSERT_DOUBLE_EQ(c1.b, 0.4);
}

// ID: COLOR8
TEST(ColorTest, OperatorPlus) {
  Color const c1(0.1, 0.2, 0.3);
  Color const c2(0.5, 0.6, 0.7);
  Color const result = c1 + c2;
  ASSERT_DOUBLE_EQ(result.r, 0.6);
  ASSERT_DOUBLE_EQ(result.g, 0.8);
  ASSERT_DOUBLE_EQ(result.b, 1.0);
}

// ID: COLOR9
TEST(ColorTest, OperatorMultiplyColor) {
  Color const c1(0.5, 1.0, 0.2);
  Color const c2(0.5, 0.8, 0.5);
  Color const result = c1 * c2;
  ASSERT_DOUBLE_EQ(result.r, 0.25);
  ASSERT_DOUBLE_EQ(result.g, 0.8);
  ASSERT_DOUBLE_EQ(result.b, 0.1);
}

// ID: COLOR10
TEST(ColorTest, OperatorMultiplyScalarLeft) {
  Color const c1(0.2, 0.4, 0.8);
  Color const result = 0.5 * c1;
  ASSERT_DOUBLE_EQ(result.r, 0.1);
  ASSERT_DOUBLE_EQ(result.g, 0.2);
  ASSERT_DOUBLE_EQ(result.b, 0.4);
}

// ID: COLOR11
TEST(ColorTest, OperatorMultiplyScalarRight) {
  Color const c1(0.2, 0.4, 0.8);
  Color const result = c1 * 0.5;
  ASSERT_DOUBLE_EQ(result.r, 0.1);
  ASSERT_DOUBLE_EQ(result.g, 0.2);
  ASSERT_DOUBLE_EQ(result.b, 0.4);
}

// ID: COLOR12
TEST(ColorTest, StrRepresentation) {
  Color const c(0.1, 0.2, 0.3);
  std::string const expected = "Color(0.1, 0.2, 0.3)";
  EXPECT_EQ(c.str(), expected);
}

// ID: COLOR13
TEST(ColorTest, LerpMidpoint) {
  Color const c1(0.0, 0.0, 0.0);
  Color const c2(1.0, 1.0, 1.0);
  Color const result = Color::lerp(c1, c2, 0.5);
  EXPECT_DOUBLE_EQ(result.r, 0.5);
  EXPECT_DOUBLE_EQ(result.g, 0.5);
  EXPECT_DOUBLE_EQ(result.b, 0.5);
}

// ID: COLOR14
TEST(ColorTest, LerpClampOutOfRange) {
  Color const c1(0.0, 0.0, 0.0);
  Color const c2(2.0, 2.0, 2.0);

  // t < 0 → debe devolver c1
  Color const below = Color::lerp(c1, c2, -0.5);
  EXPECT_DOUBLE_EQ(below.r, 0.0);
  EXPECT_DOUBLE_EQ(below.g, 0.0);
  EXPECT_DOUBLE_EQ(below.b, 0.0);

  // t > 1 → debe devolver c2 clamped a [0,1]
  Color const above = Color::lerp(c1, c2, 1.5);
  EXPECT_DOUBLE_EQ(above.r, 1.0);
  EXPECT_DOUBLE_EQ(above.g, 1.0);
  EXPECT_DOUBLE_EQ(above.b, 1.0);
}
