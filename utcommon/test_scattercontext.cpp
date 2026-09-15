#include "infocolision.hpp"
#include "ray.hpp"
#include "scatter_context.hpp"
#include <gtest/gtest.h>
#include <random>

using namespace render;

// ID: SCT-1
TEST(test_scatter_context, constructor_initializes_all_pointers_correctly) {
  Ray const ray(vector(0, 0, 0), vector(1, 0, 0));
  InfoColision const info;
  std::mt19937_64 rng(123);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  ScatterContext const ctx(&ray, &info, &rng, &dist);

  EXPECT_EQ(ctx.r_in, &ray);
  EXPECT_EQ(ctx.info, &info);
  EXPECT_EQ(ctx.rng, &rng);
  EXPECT_EQ(ctx.dist, &dist);
}

// ID: SCT-2
TEST(test_scatter_context, next_returns_random_value_between_0_and_1) {
  Ray const ray(vector(0, 0, 0), vector(1, 0, 0));
  InfoColision const info;
  std::mt19937_64 rng(42);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  ScatterContext const ctx(&ray, &info, &rng, &dist);

  double const value = ctx.next();
  EXPECT_GE(value, 0.0);
  EXPECT_LE(value, 1.0);
}

// ID: SCT-3
TEST(test_scatter_context, ray_returns_reference_to_input_ray) {
  Ray const ray(vector(0, 0, 0), vector(0, 1, 0));
  InfoColision const info;
  std::mt19937_64 rng(10);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  ScatterContext const ctx(&ray, &info, &rng, &dist);

  Ray const & ref = ctx.ray();
  EXPECT_EQ(&ref, &ray);
}

// ID: SCT-4
TEST(test_scatter_context, collision_returns_reference_to_input_info) {
  Ray const ray(vector(0, 0, 0), vector(0, 1, 0));
  InfoColision info;
  info.t      = 5.0;
  info.p      = vector(1.0, 2.0, 3.0);
  info.normal = vector(0.0, 1.0, 0.0);

  std::mt19937_64 rng(77);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  ScatterContext const ctx(&ray, &info, &rng, &dist);

  InfoColision const & ref = ctx.collision();
  EXPECT_EQ(&ref, &info);
  EXPECT_DOUBLE_EQ(ref.t, 5.0);
  EXPECT_EQ(ref.p, vector(1.0, 2.0, 3.0));
  EXPECT_EQ(ref.normal, vector(0.0, 1.0, 0.0));
}
