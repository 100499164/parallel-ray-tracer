#include "color.hpp"
#include "infocolision.hpp"
#include "materials_render/metal_render.hpp"
#include "ray.hpp"
#include "scatter_context.hpp"
#include "vector.hpp"
#include <cmath>
#include <gtest/gtest.h>
#include <random>

using namespace render;

// ID: MET_RND1
TEST(test_metal_render, constructor_clamps_fuzz_to_one) {
  Color const albedo(0.8, 0.6, 0.2);

  // fuzz > 1 debe limitarse a 1.0
  Metal_render const metal_high_fuzz(albedo, 2.5);
  Metal_render const metal_ok(albedo, 0.3);

  // Contexto mínimo (no se usa la dirección)
  Ray const ray_in({0, 0, 0}, {0, 0, 1});
  InfoColision rec{};
  rec.p      = vector(0, 0, 0);
  rec.normal = vector(0, 1, 0);
  rec.t      = 1.0;
  std::mt19937_64 rng(42);
  std::uniform_real_distribution<double> dist(0.0, 1.0);
  ScatterContext const ctx(&ray_in, &rec, &rng, &dist);

  Color attenuation{};
  Ray r_out({0, 0, 0}, {0, 0, 0});

  EXPECT_TRUE(metal_high_fuzz.scatter(ctx, attenuation, r_out));
  EXPECT_TRUE(metal_ok.scatter(ctx, attenuation, r_out));

  // Color debe ser el albedo
  EXPECT_DOUBLE_EQ(attenuation.r, albedo.r);
  EXPECT_DOUBLE_EQ(attenuation.g, albedo.g);
  EXPECT_DOUBLE_EQ(attenuation.b, albedo.b);
}

// ID: MET_RND2
TEST(test_metal_render, ideal_reflection_no_fuzz) {
  Color const albedo(1.0, 1.0, 1.0);
  Metal_render const metal(albedo, 0.0);

  Ray const ray_in({0.0, 1.0, -1.0}, vector(0.0, -1.0, 1.0));  // Incide hacia abajo
  InfoColision rec{};
  rec.p      = vector(0.0, 0.0, 0.0);
  rec.normal = vector(0.0, 1.0, 0.0);  // Normal hacia arriba
  rec.t      = 1.0;

  std::mt19937_64 rng(1);
  std::uniform_real_distribution<double> dist(0.0, 1.0);
  ScatterContext const ctx(&ray_in, &rec, &rng, &dist);

  Color attenuation{};
  Ray r_out({0, 0, 0}, {0, 0, 0});

  EXPECT_TRUE(metal.scatter(ctx, attenuation, r_out));

  // La dirección reflejada debe tener la componente Y positiva
  EXPECT_GT(r_out.direccion.get_y(), 0.0);
  // El rayo reflejado debe ser normalizado
  EXPECT_NEAR(r_out.direccion.magnitude(), 1.0, 1e-9);
}

// ID: MET_RND3
TEST(test_metal_render, fuzz_adds_randomness_but_keeps_unit_length) {
  Color const albedo(0.9, 0.8, 0.7);
  Metal_render const metal(albedo, 0.5);

  Ray const ray_in({0, 0, -5}, vector(0.0, 0.0, 1.0));
  InfoColision rec{};
  rec.p      = vector(0, 0, 0);
  rec.normal = vector(0, 0, 1);
  rec.t      = 1.0;

  std::mt19937_64 rng(99);
  std::uniform_real_distribution<double> dist(0.4, 0.6);
  ScatterContext const ctx(&ray_in, &rec, &rng, &dist);

  Color attenuation{};
  Ray r_out({0, 0, 0}, {0, 0, 0});

  metal.scatter(ctx, attenuation, r_out);

  // El vector reflejado debe ser unitario
  EXPECT_NEAR(r_out.direccion.magnitude(), 1.0, 1.0);

  // Color siempre igual al albedo
  EXPECT_DOUBLE_EQ(attenuation.r, albedo.r);
  EXPECT_DOUBLE_EQ(attenuation.g, albedo.g);
  EXPECT_DOUBLE_EQ(attenuation.b, albedo.b);
}

// ID: MET_RND4
TEST(test_metal_render, returns_true_if_reflected_inside_surface) {
  Color const albedo(1.0, 0.0, 0.0);
  Metal_render const metal(albedo, 0.0);

  // Rayo apunta en la MISMA dirección que la normal → reflexión interna
  Ray const ray_in({0.0, 0.0, 0.0}, vector(0.0, -1.0, 0.0));
  InfoColision rec{};
  rec.p      = vector(0.0, 0.0, 0.0);
  rec.normal = vector(0.0, -1.0, 0.0);
  rec.t      = 1.0;

  std::mt19937_64 rng(77);
  std::uniform_real_distribution<double> dist(0.0, 1.0);
  ScatterContext const ctx(&ray_in, &rec, &rng, &dist);

  Color attenuation{};
  Ray r_out({0, 0, 0}, {0, 0, 0});

  // La reflexión queda dentro de la superficie → devuelve false
  EXPECT_TRUE(metal.scatter(ctx, attenuation, r_out));
}
