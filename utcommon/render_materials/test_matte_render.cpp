#include "color.hpp"
#include "infocolision.hpp"
#include "materials_render/matte_render.hpp"
#include "ray.hpp"
#include "scatter_context.hpp"
#include <cmath>
#include <gtest/gtest.h>
#include <random>

using namespace render;

// ID: MATTE_RND1
TEST(test_matte_render, constructor_stores_albedo_correctly) {
  Color const albedo(0.2, 0.4, 0.6);
  Matte_render const matte(albedo);

  // Crear un contexto válido (aunque no se use aquí)
  Ray const ray_in({0, 0, 0}, {0, 0, 1});
  InfoColision const rec{};
  std::mt19937_64 rng(1'234);
  std::uniform_real_distribution<double> dist(0.0, 1.0);
  ScatterContext const ctx(&ray_in, &rec, &rng, &dist);

  Color attenuation{};
  Ray r_out({0, 0, 0}, {0, 0, 0});

  // scatter siempre devuelve true
  EXPECT_TRUE(matte.scatter(ctx, attenuation, r_out));

  // La atenuación debe ser igual al albedo original
  EXPECT_DOUBLE_EQ(attenuation.r, albedo.r);
  EXPECT_DOUBLE_EQ(attenuation.g, albedo.g);
  EXPECT_DOUBLE_EQ(attenuation.b, albedo.b);
}

// ID: MATTE_RND2
TEST(test_matte_render, scatter_generates_valid_direction) {
  Color const albedo(1.0, 0.5, 0.0);
  Matte_render const matte(albedo);

  Ray const ray_in({0.0, 0.0, -1.0}, {0.0, 0.0, 1.0});

  InfoColision rec{};
  rec.p      = vector(0.0, 0.0, 0.0);
  rec.normal = vector(0.0, 1.0, 0.0);
  rec.t      = 1.0;

  std::mt19937_64 rng(42);
  std::uniform_real_distribution<double> dist(0.0, 1.0);
  ScatterContext const ctx(&ray_in, &rec, &rng, &dist);

  Color attenuation{};
  Ray r_out({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0});

  EXPECT_TRUE(matte.scatter(ctx, attenuation, r_out));

  // La dirección debe estar normalizada
  double const mag = r_out.direccion.magnitude();
  EXPECT_NEAR(mag, 1.0, 1e-9);

  // Debe apuntar hacia alguna dirección distinta de (0,0,0)
  EXPECT_FALSE(std::isnan(r_out.direccion.x));
  EXPECT_FALSE(std::isnan(r_out.direccion.y));
  EXPECT_FALSE(std::isnan(r_out.direccion.z));
}

// ID: MATTE_RND3
TEST(test_matte_render, scatter_handles_near_zero_vector) {
  Color const albedo(0.5, 0.5, 0.5);
  Matte_render const matte(albedo);

  Ray const ray_in({0.0, 0.0, -1.0}, {0.0, 0.0, 1.0});
  InfoColision rec{};
  rec.p      = vector(0.0, 0.0, 0.0);
  rec.normal = vector(0.0, 1.0, 0.0);
  rec.t      = 1.0;

  std::mt19937_64 rng(12'345);
  std::uniform_real_distribution<double> dist(0.4999999999, 0.5000000001);
  ScatterContext const ctx(&ray_in, &rec, &rng, &dist);

  Color attenuation{};
  Ray r_out({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0});

  EXPECT_TRUE(matte.scatter(ctx, attenuation, r_out));

  // Debe usar el vector normal directamente (caso de fallback)
  EXPECT_NEAR(r_out.direccion.get_x(), 0.0, 0.5);
  EXPECT_NEAR(r_out.direccion.get_y(), 1.0, 0.5);
  EXPECT_NEAR(r_out.direccion.get_z(), 0.0, 0.5);
}
