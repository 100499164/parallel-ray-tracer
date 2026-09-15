#include "ray.hpp"
#include "vector.hpp"
#include <gtest/gtest.h>

using namespace render;

// ID: RAY-1
TEST(test_ray, default_constructor_initializes_zero_vectors) {
  Ray const ray;
  EXPECT_DOUBLE_EQ(ray.origen.x, 0.0);
  EXPECT_DOUBLE_EQ(ray.origen.y, 0.0);
  EXPECT_DOUBLE_EQ(ray.origen.z, 0.0);
  EXPECT_DOUBLE_EQ(ray.direccion.x, 0.0);
  EXPECT_DOUBLE_EQ(ray.direccion.y, 0.0);
  EXPECT_DOUBLE_EQ(ray.direccion.z, 0.0);
}

// ID: RAY-2
TEST(test_ray, parameterized_constructor_assigns_values) {
  vector const origin(1.0, 2.0, 3.0);
  vector const direction(0.5, -0.5, 1.0);
  Ray const ray(origin, direction);

  EXPECT_EQ(ray.origen, origin);
  EXPECT_EQ(ray.direccion, direction);
}

// ID: RAY-3
TEST(test_ray, at_method_computes_correct_point) {
  Ray const ray(vector(0.0, 0.0, 0.0), vector(1.0, 2.0, 3.0));

  vector const point = ray.at(2.0);
  EXPECT_EQ(point, vector(2.0, 4.0, 6.0));
}

// ID: RAY-4
TEST(test_ray, at_method_handles_negative_t) {
  Ray const ray(vector(1.0, 1.0, 1.0), vector(1.0, 0.0, 0.0));

  vector const point = ray.at(-3.0);
  EXPECT_EQ(point, vector(-2.0, 1.0, 1.0));
}

// ID: RAY-5
TEST(test_ray, at_method_zero_t_returns_origin) {
  Ray const ray(vector(4.0, 5.0, 6.0), vector(0.5, 0.5, 0.5));
  EXPECT_EQ(ray.at(0.0), ray.origen);
}
