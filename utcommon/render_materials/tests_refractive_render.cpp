#include "color.hpp"
#include "infocolision.hpp"
#include "materials_render/refractive_render.hpp"
#include "ray.hpp"
#include "scatter_context.hpp"
#include <cmath>
#include <gtest/gtest.h>
#include <random>

using namespace render;

// ID: REF-RND-1
TEST(test_refractive_render, constructor_stores_index_of_refraction) {
  Refractive_render const refr(1.5);
  // No hay getter, pero se puede comprobar indirectamente con scatter()
  Ray const ray_in({0, 0, -1}, vector(0, 0, 1));
  InfoColision rec{};
  rec.p          = vector(0, 0, 0);
  rec.normal     = vector(0, 0, 1);
  rec.front_face = true;
  rec.t          = 1.0;

  std::mt19937_64 rng(1);
  std::uniform_real_distribution<double> dist(0.0, 1.0);
  ScatterContext const ctx(&ray_in, &rec, &rng, &dist);

  Color attenuation{};
  Ray r_out({0, 0, 0}, {0, 0, 0});
  EXPECT_TRUE(refr.scatter(ctx, attenuation, r_out));
  EXPECT_NEAR(r_out.direccion.magnitude(), 1.0, 1e-9);
}

// ID: REF-RND-2
TEST(test_refractive_render, refracts_from_air_to_glass) {
  Refractive_render const refr(1.5);
  Ray const ray_in({0, 0, -1}, vector(0, 0, 1));  // Del aire hacia el objeto
  InfoColision rec{};
  rec.p          = vector(0, 0, 0);
  rec.normal     = vector(0, 0, 1);
  rec.front_face = true;

  std::mt19937_64 rng(42);
  std::uniform_real_distribution<double> dist(0.0, 1.0);
  ScatterContext const ctx(&ray_in, &rec, &rng, &dist);

  Color attenuation{};
  Ray r_out({0, 0, 0}, {0, 0, 0});

  EXPECT_TRUE(refr.scatter(ctx, attenuation, r_out));
  EXPECT_NEAR(attenuation.r, 1.0, 1e-9);
  EXPECT_NEAR(attenuation.g, 1.0, 1e-9);
  EXPECT_NEAR(attenuation.b, 1.0, 1e-9);

  // El rayo refractado entra al material (z < 0)
  EXPECT_LT(r_out.direccion.get_z(), 0.0);
}

// ID: REF-RND-3
TEST(test_refractive_render, refracts_from_glass_to_air) {
  Refractive_render const refr(1.5);
  Ray const ray_in({0, 0, 0}, vector(0, 0, 1));  // Desde dentro del vidrio hacia fuera
  InfoColision rec{};
  rec.p          = vector(0, 0, 0);
  rec.normal     = vector(0, 0, -1);  // Normal apunta hacia dentro (porque estamos saliendo)
  rec.front_face = false;

  std::mt19937_64 rng(123);
  std::uniform_real_distribution<double> dist(0.0, 1.0);
  ScatterContext const ctx(&ray_in, &rec, &rng, &dist);

  Color attenuation{};
  Ray r_out({0, 0, 0}, {0, 0, 0});

  EXPECT_TRUE(refr.scatter(ctx, attenuation, r_out));

  // El rayo sale del material (z > 0)
  EXPECT_GT(r_out.direccion.get_z(), 0.0);
}

// ID: REF-RND-4
TEST(test_refractive_render, total_internal_reflection_occurs) {
  Refractive_render const refr(1.5);
  Ray const ray_in({0, 0, 0}, vector(1.0, 0.0, 1.0));  // incidencia oblicua
  InfoColision rec{};
  rec.p          = vector(0, 0, 0);
  rec.normal     = vector(0, 0, -1);
  rec.front_face = false;  // Dentro → fuera

  std::mt19937_64 rng(9);
  std::uniform_real_distribution<double> dist(0.0, 1.0);
  ScatterContext const ctx(&ray_in, &rec, &rng, &dist);

  Color attenuation{};
  Ray r_out({0, 0, 0}, {0, 0, 0});

  EXPECT_TRUE(refr.scatter(ctx, attenuation, r_out));

  // Reflexión total: rebota hacia dentro (z < 0)
  EXPECT_LT(r_out.direccion.get_z(), 0.0);
  EXPECT_NEAR(r_out.direccion.magnitude(), 1.0, 1e-9);
}

// ID: REF-RND-5
TEST(test_refractive_render, always_returns_true_and_unit_vector) {
  Refractive_render const refr(1.33);
  Ray const ray_in({0, 0, -1}, vector(0, 0, 1));
  InfoColision rec{};
  rec.p          = vector(0, 0, 0);
  rec.normal     = vector(0, 0, 1);
  rec.front_face = true;

  std::mt19937_64 rng(10);
  std::uniform_real_distribution<double> dist(0.0, 1.0);
  ScatterContext const ctx(&ray_in, &rec, &rng, &dist);

  Color attenuation{};
  Ray r_out({0, 0, 0}, {0, 0, 0});

  EXPECT_TRUE(refr.scatter(ctx, attenuation, r_out));
  EXPECT_NEAR(r_out.direccion.magnitude(), 1.0, 1e-9);
}
