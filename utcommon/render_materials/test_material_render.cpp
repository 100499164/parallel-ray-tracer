#include "color.hpp"
#include "infocolision.hpp"
#include "materials_render/material_render.hpp"
#include "ray.hpp"
#include "scatter_context.hpp"
#include <gtest/gtest.h>
#include <random>
#include <utility>

using namespace render;

// Clase derivada mínima para probar Material_render
class DummyMaterialRender : public Material_render {
public:
  DummyMaterialRender() = default;

  bool scatter(ScatterContext const &, Color & atenuacion, Ray & r_out) const override {
    atenuacion = Color(0.5, 0.5, 0.5);
    r_out      = Ray({0.0, 0.0, 0.0}, {1.0, 0.0, 0.0});
    return true;
  }
};

// ID: MAT_RND1
TEST(test_material_render, derived_scatter_returns_true_and_sets_values) {
  DummyMaterialRender const mat;

  // Crear un rayo de entrada
  Ray const ray_in({0.0, 0.0, 0.0}, {0.0, 0.0, 1.0});

  // Crear un registro de colisión
  InfoColision record{};
  record.p      = vector(0.0, 0.0, 0.0);
  record.normal = vector(0.0, 1.0, 0.0);
  record.t      = 1.0;

  // Crear un generador aleatorio y una distribución
  std::mt19937_64 rng(1'234);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  // Crear el contexto de dispersión correctamente
  ScatterContext const ctx(&ray_in, &record, &rng, &dist);

  Color attenuation{};
  Ray r_out({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0});

  EXPECT_TRUE(mat.scatter(ctx, attenuation, r_out));

  EXPECT_DOUBLE_EQ(attenuation.r, 0.5);
  EXPECT_DOUBLE_EQ(attenuation.g, 0.5);
  EXPECT_DOUBLE_EQ(attenuation.b, 0.5);

  EXPECT_DOUBLE_EQ(r_out.origen.get_x(), 0.0);
  EXPECT_DOUBLE_EQ(r_out.origen.get_y(), 0.0);
  EXPECT_DOUBLE_EQ(r_out.origen.get_z(), 0.0);

  EXPECT_DOUBLE_EQ(r_out.direccion.get_x(), 1.0);
  EXPECT_DOUBLE_EQ(r_out.direccion.get_y(), 0.0);
  EXPECT_DOUBLE_EQ(r_out.direccion.get_z(), 0.0);
}

// ID: MAT_RND2
TEST(test_material_render, abstract_class_cannot_be_instantiated_directly) {
  SUCCEED();
}

// ID: MAT_RND3
TEST(test_material_render, rule_of_five_works_correctly) {
  DummyMaterialRender m1;
  DummyMaterialRender const m2 = m1;
  DummyMaterialRender m3       = std::move(m1);
  DummyMaterialRender m4;
  m4 = m2;
  DummyMaterialRender m5;
  m5 = std::move(m3);
  SUCCEED();
}
