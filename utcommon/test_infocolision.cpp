#include "color.hpp"
#include "infocolision.hpp"
#include "materials_render/material_render.hpp"
#include "ray.hpp"
#include "scatter_context.hpp"
#include "vector.hpp"
#include <gtest/gtest.h>

using namespace render;

// ID: INF-1
TEST(test_infocolision, default_initialization_sets_zero_values) {
  InfoColision const info;

  EXPECT_DOUBLE_EQ(info.t, 0.0);
  EXPECT_EQ(info.p, vector(0.0, 0.0, 0.0));
  EXPECT_EQ(info.normal, vector(0.0, 0.0, 0.0));
  EXPECT_FALSE(info.front_face);
  EXPECT_EQ(info.mat_ptr, nullptr);
}

// ID: INF-2
TEST(test_infocolision, set_face_normal_sets_front_face_true_when_ray_hits_outside) {
  Ray const ray(vector(0, 0, 0), vector(0, 0, 1));
  vector const outward_normal(0, 0, -1);

  InfoColision info;
  info.set_face_normal(ray, outward_normal);

  EXPECT_TRUE(info.front_face);
  EXPECT_EQ(info.normal, outward_normal);
}

// ID: INF-3
TEST(test_infocolision, set_face_normal_sets_front_face_false_when_ray_inside_surface) {
  Ray const ray(vector(0, 0, 0), vector(0, 0, -1));
  vector const outward_normal(0, 0, -1);

  InfoColision info;
  info.set_face_normal(ray, outward_normal);

  EXPECT_FALSE(info.front_face);
  EXPECT_EQ(info.normal, outward_normal * -1.0);
}

// ID: INF-4
TEST(test_infocolision, set_face_normal_flips_normal_correctly) {
  Ray const ray(vector(0, 0, 0), vector(1, 0, 0));
  vector const outward_normal(-1, 0, 0);

  InfoColision info;
  info.set_face_normal(ray, outward_normal);

  EXPECT_TRUE(info.front_face);
  EXPECT_EQ(info.normal, outward_normal);
}

// ID: INF-5
TEST(test_infocolision, mat_ptr_can_be_assigned_safely) {
  // Clase dummy local para evitar reinterpret_cast
  class DummyMaterial : public render::Material_render {
  public:
    bool scatter(render::ScatterContext const &, render::Color &, render::Ray &) const override {
      return true;
    }
  };

  InfoColision info;
  DummyMaterial dummy_material;

  // Asignamos un puntero válido del tipo correcto
  info.mat_ptr = &dummy_material;

  EXPECT_NE(info.mat_ptr, nullptr);
}
