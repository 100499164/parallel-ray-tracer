#include "infocolision.hpp"
#include "ray.hpp"
#include "render_figures/sphereRender.hpp"
#include <gtest/gtest.h>

using namespace render;

// ID: SPH-RND-1
TEST(SphereRender, RayTowardsSphereButMisses) {
  // Declare required inputs
  SphereRender const sphere({0.0, 0.0, 0.0}, 2.0, nullptr);
  Ray const ray({0.0, 3.0, -5.0}, vector(0.0, 0.0, 1.0));
  double const t_min = 0.001;
  double const t_max = 100.0;
  InfoColision record;

  // Perform the hit test
  bool const hit = sphere.hit(ray, t_min, t_max, record);
  EXPECT_FALSE(hit);

  // Check that the record is not updated
  EXPECT_EQ(record.t, 0);
  EXPECT_EQ(record.p, vector(0, 0, 0));
  EXPECT_EQ(record.normal, vector(0, 0, 0));
}

// ID: SPH-RND-2
TEST(SphereRender, RayOppositeToSphere) {
  // Declare required inputs
  SphereRender const sphere({0.0, 0.0, 0.0}, 2.0, nullptr);
  Ray const ray({0.0, 0.0, -5.0}, vector(0.0, 0.0, -1.0));
  double const t_min = 0.001;
  double const t_max = 100.0;
  InfoColision record;

  // Perform the hit test
  bool const hit = sphere.hit(ray, t_min, t_max, record);
  EXPECT_FALSE(hit);

  // Check that the record is not updated
  EXPECT_EQ(record.t, 0);
  EXPECT_EQ(record.p, vector(0, 0, 0));
  EXPECT_EQ(record.normal, vector(0, 0, 0));
}

// ID: SPH-RND-3
TEST(SphereRender, RayHitsButIsTooFar) {
  // Declare required inputs
  SphereRender const sphere({0.0, 0.0, 0.0}, 2.0, nullptr);
  Ray const ray({0.0, 0.0, -5.0}, vector(0.0, 0.0, 1.0));
  double const t_min = 0.001;
  double const t_max = 2.9;
  InfoColision record;

  // Perform the hit test
  bool const hit = sphere.hit(ray, t_min, t_max, record);
  EXPECT_FALSE(hit);

  // Check that the record is not updated
  EXPECT_EQ(record.t, 0);
  EXPECT_EQ(record.p, vector(0, 0, 0));
  EXPECT_EQ(record.normal, vector(0, 0, 0));
}

// ID: SPH-RND-4
TEST(SphereRender, RayHitsButIsTooClose) {
  // Declare required inputs
  SphereRender const sphere({0.0, 0.0, 0.0}, 2.0, nullptr);
  Ray const ray({0.0, 2.0, -0.0001}, vector(0.0, 0.0, 1.0));
  double const t_min = 0.001;
  double const t_max = 100.0;
  InfoColision record;

  // Perform the hit test
  bool const hit = sphere.hit(ray, t_min, t_max, record);
  EXPECT_FALSE(hit);

  // Check that the record is not updated
  EXPECT_EQ(record.t, 0);
  EXPECT_EQ(record.p, vector(0, 0, 0));
  EXPECT_EQ(record.normal, vector(0, 0, 0));
}

// ID: SPH-RND-5
TEST(SphereRender, FrontFaceHit) {
  // Declare required inputs
  SphereRender const sphere({0.0, 0.0, 0.0}, 2.0, nullptr);
  Ray const ray({0.0, 0.0, -5.0}, vector(0.0, 0.0, 1.0));
  double const t_min = 0.001;
  double const t_max = 100.0;
  InfoColision record;

  // Perform the hit test
  bool const hit = sphere.hit(ray, t_min, t_max, record);
  EXPECT_TRUE(hit);

  // Check that the record is correctly updated
  EXPECT_EQ(record.t, 3.0);
  EXPECT_EQ(record.p, vector(0, 0, -2.0));
  EXPECT_EQ(record.normal, vector(0, 0, -1.0));
}

// ID: SPH-RND-6
TEST(SphereRender, LateralFaceHit) {
  // Declare required inputs
  SphereRender const sphere({0.0, 0.0, 0.0}, 2.0, nullptr);
  Ray const ray({-5.0, 0.0, 0.0}, vector(1.0, 0.0, 0.0));
  double const t_min = 0.001;
  double const t_max = 100.0;
  InfoColision record;

  // Perform the hit test
  bool const hit = sphere.hit(ray, t_min, t_max, record);
  EXPECT_TRUE(hit);

  // Check that the record is correctly updated
  EXPECT_EQ(record.t, 3.0);
  EXPECT_EQ(record.p, vector(-2.0, 0, 0.0));
  EXPECT_EQ(record.normal, vector(-1.0, 0, 0.0));
}

// ID: SPH-RND-7
TEST(SphereRender, TangentHit) {
  // Declare required inputs
  SphereRender const sphere({0.0, 0.0, 0.0}, 2.0, nullptr);
  Ray const ray({0.0, 2.0, -5.0}, vector(0.0, 0.0, 1.0));
  double const t_min = 0.001;
  double const t_max = 100.0;
  InfoColision record;

  // Perform the hit test
  bool const hit = sphere.hit(ray, t_min, t_max, record);
  EXPECT_TRUE(hit);

  // Check that the record is correctly updated
  EXPECT_EQ(record.t, 5.0);
  EXPECT_EQ(record.p, vector(0.0, 2.0, 0.0));
  EXPECT_EQ(record.normal, vector(0.0, -1.0, 0.0));
}

// ID: SPH-RND-8
TEST(SphereRender, InsideSphereHit) {
  // Declare required inputs
  SphereRender const sphere({0.0, 0.0, 0.0}, 2.0, nullptr);
  Ray const ray({0.0, 0.0, 0.0}, vector(0.0, 1.0, 0.0));
  double const t_min = 0.001;
  double const t_max = 100.0;
  InfoColision record;

  // Perform the hit test
  bool const hit = sphere.hit(ray, t_min, t_max, record);
  EXPECT_TRUE(hit);

  // Check that the record is correctly updated
  EXPECT_EQ(record.t, 2.0);
  EXPECT_EQ(record.p, vector(0.0, 2.0, 0.0));
  EXPECT_EQ(record.normal, vector(0.0, -1.0, 0.0));
}

// ID: SPH-RND-9
TEST(SphereRender, RayEntersAndExitsSphere) {
  // Declare required inputs
  SphereRender const sphere({0.0, 0.0, 0.0}, 2.0, nullptr);
  Ray const ray({0.0, 0.0, -3.0}, vector(0.0, 0.0, 1.0));
  double const t_min = 0.001;
  double const t_max = 100.0;
  InfoColision record;

  // Perform the hit test
  bool const hit = sphere.hit(ray, t_min, t_max, record);
  EXPECT_TRUE(hit);

  // Check that the record is correctly updated
  EXPECT_EQ(record.t, 1.0);
  EXPECT_EQ(record.p, vector(0.0, 0.0, -2.0));
  EXPECT_EQ(record.normal, vector(0.0, 0.0, -1.0));
}
