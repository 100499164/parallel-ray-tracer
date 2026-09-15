#include "vector.hpp"
#include "gtest/gtest.h"
#include <cmath>
#include <sstream>

TEST(VectorTest, ConstructorAndComponents) {
  render::vector const v(1.0, 2.0, 3.0);
  EXPECT_DOUBLE_EQ(v.x, 1.0);
  EXPECT_DOUBLE_EQ(v.y, 2.0);
  EXPECT_DOUBLE_EQ(v.z, 3.0);
}

TEST(VectorTest, GettersAndSetters) {
  render::vector v;

  // --- Setters ---
  v.set_x(3.5);
  v.set_y(-2.1);
  v.set_z(7.8);
  EXPECT_DOUBLE_EQ(v.x, 3.5);
  EXPECT_DOUBLE_EQ(v.y, -2.1);
  EXPECT_DOUBLE_EQ(v.z, 7.8);

  EXPECT_DOUBLE_EQ(v.get_x(), 3.5);
  EXPECT_DOUBLE_EQ(v.get_y(), -2.1);
  EXPECT_DOUBLE_EQ(v.get_z(), 7.8);

  v.set_x(0.0);
  v.set_y(10.0);
  v.set_z(-5.5);

  EXPECT_DOUBLE_EQ(v.get_x(), 0.0);
  EXPECT_DOUBLE_EQ(v.get_y(), 10.0);
  EXPECT_DOUBLE_EQ(v.get_z(), -5.5);
}

TEST(VectorTest, DefaultConstructorInitializesToZero) {
  render::vector const v;
  EXPECT_DOUBLE_EQ(v.x, 0.0);
  EXPECT_DOUBLE_EQ(v.y, 0.0);
  EXPECT_DOUBLE_EQ(v.z, 0.0);
}

TEST(VectorTest, Magnitude) {
  render::vector const v(3.0, 4.0, 0.0);
  EXPECT_DOUBLE_EQ(v.magnitude(), 5.0);
}

TEST(VectorTest, SquaredMagnitudeIsMagnitudeSquared) {
  render::vector const v(1.0, -2.0, 2.0);
  double const expected = v.x * v.x + v.y * v.y + v.z * v.z;
  EXPECT_DOUBLE_EQ(v.squared_magnitude(), expected);
  EXPECT_NEAR(v.squared_magnitude(), std::pow(v.magnitude(), 2.0), 1e-9);
}

TEST(VectorTest, Normalized) {
  render::vector const v(0.0, 3.0, 4.0);
  render::vector const n = v.normalized();
  EXPECT_NEAR(n.x, 0.0, 1e-9);
  EXPECT_NEAR(n.y, 0.6, 1e-9);
  EXPECT_NEAR(n.z, 0.8, 1e-9);
  EXPECT_NEAR(n.magnitude(), 1.0, 1e-9);
}

TEST(VectorTest, AdditionAndSubtraction) {
  render::vector const a(1.0, 2.0, 3.0);
  render::vector const b(4.0, 5.0, 6.0);

  render::vector const sum  = a + b;
  render::vector const diff = b - a;

  EXPECT_DOUBLE_EQ(sum.x, 5.0);
  EXPECT_DOUBLE_EQ(sum.y, 7.0);
  EXPECT_DOUBLE_EQ(sum.z, 9.0);

  EXPECT_DOUBLE_EQ(diff.x, 3.0);
  EXPECT_DOUBLE_EQ(diff.y, 3.0);
  EXPECT_DOUBLE_EQ(diff.z, 3.0);
}

TEST(VectorTest, ScalarMultiplicationAndDivision) {
  render::vector const v(1.0, -2.0, 3.0);

  render::vector const scaled  = v * 2.0;
  render::vector const divided = v / 2.0;

  EXPECT_DOUBLE_EQ(scaled.x, 2.0);
  EXPECT_DOUBLE_EQ(scaled.y, -4.0);
  EXPECT_DOUBLE_EQ(scaled.z, 6.0);

  EXPECT_DOUBLE_EQ(divided.x, 0.5);
  EXPECT_DOUBLE_EQ(divided.y, -1.0);
  EXPECT_DOUBLE_EQ(divided.z, 1.5);
}

TEST(VectorTest, DotProduct) {
  render::vector const a(1.0, 2.0, 3.0);
  render::vector const b(4.0, -5.0, 6.0);

  double const result = render::vector::dot(a, b);
  EXPECT_DOUBLE_EQ(result, 12.0);
}

TEST(VectorTest, CrossProduct) {
  render::vector const a(1.0, 0.0, 0.0);
  render::vector const b(0.0, 1.0, 0.0);
  render::vector const c = render::vector::cross(a, b);

  EXPECT_NEAR(c.x, 0.0, 1e-9);
  EXPECT_NEAR(c.y, 0.0, 1e-9);
  EXPECT_NEAR(c.z, 1.0, 1e-9);
}

TEST(VectorTest, CompoundOperators) {
  render::vector v(1.0, 1.0, 1.0);
  render::vector const u(2.0, 3.0, 4.0);

  v += u;
  EXPECT_DOUBLE_EQ(v.x, 3.0);
  EXPECT_DOUBLE_EQ(v.y, 4.0);
  EXPECT_DOUBLE_EQ(v.z, 5.0);

  v -= u;
  EXPECT_DOUBLE_EQ(v.x, 1.0);
  EXPECT_DOUBLE_EQ(v.y, 1.0);
  EXPECT_DOUBLE_EQ(v.z, 1.0);

  v *= 2.0;
  EXPECT_DOUBLE_EQ(v.x, 2.0);
  EXPECT_DOUBLE_EQ(v.y, 2.0);
  EXPECT_DOUBLE_EQ(v.z, 2.0);
}

TEST(VectorTest, NormalizeZeroVector) {
  render::vector const v(0.0, 0.0, 0.0);
  render::vector const n = v.normalized();

  EXPECT_DOUBLE_EQ(n.x, 0.0);
  EXPECT_DOUBLE_EQ(n.y, 0.0);
  EXPECT_DOUBLE_EQ(n.z, 0.0);
  EXPECT_DOUBLE_EQ(n.magnitude(), 0.0);
}

TEST(VectorTest, Dot2MemberFunction) {
  render::vector const a(1.0, 2.0, 3.0);
  render::vector const b(4.0, -5.0, 6.0);
  double const result = a.dot2(b);
  EXPECT_DOUBLE_EQ(result, 12.0);
}

TEST(VectorTest, EqualityAndInequalityOperators) {
  render::vector const a(1.0, 2.0, 3.0);
  render::vector const b(1.0, 2.0, 3.0);
  render::vector const c(1.0, 2.1, 3.0);

  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a != b);
  EXPECT_TRUE(a != c);
  EXPECT_FALSE(a == c);
}

TEST(VectorTest, OutputStreamOperator) {
  render::vector const v(1.0, 2.0, 3.0);
  std::ostringstream oss;
  oss << v;
  EXPECT_EQ(oss.str(), "(1, 2, 3)");
}
