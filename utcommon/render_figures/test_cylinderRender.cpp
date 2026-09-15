#include "infocolision.hpp"
#include "ray.hpp"
#include "render_figures/cylinderRender.hpp"
#include <gtest/gtest.h>

using namespace render;

// ID: CYL-RND-1
TEST(CylinderRender, RayTowardsCylinderButMisses) {
  // Declare required inputs
  CylinderContext const ctx{
    {0.0, 0.0, 0.0},
    vector(0.0, 1.0, 0.0), 1.0, 2.0
  };
  CylinderRender const cylinder(ctx, nullptr);
  Ray const ray({2.0, 0.0, 0.0}, vector(0.0, 0.0, 1.0));
  double const t_min = 0.001;
  double const t_max = 100.0;
  InfoColision record;

  // Perform the hit test
  bool const hit = cylinder.hit(ray, t_min, t_max, record);
  EXPECT_FALSE(hit);

  // Check that the record is not updated
  EXPECT_EQ(record.t, 0);
  EXPECT_EQ(record.p, vector(0, 0, 0));
  EXPECT_EQ(record.normal, vector(0, 0, 0));
}

// ID: CYL-RND-2
TEST(CylinderRender, RayOppositeToCylinder) {
  // Declare required inputs
  CylinderContext const ctx{
    {0.0, 0.0, 0.0},
    vector(0.0, 1.0, 0.0), 1.0, 2.0
  };
  CylinderRender const cylinder(ctx, nullptr);
  Ray const ray({0.0, 0.0, -5.0}, vector(0.0, 0.0, -1.0));
  double const t_min = 0.001;
  double const t_max = 100.0;
  InfoColision record;

  // Perform the hit test
  bool const hit = cylinder.hit(ray, t_min, t_max, record);
  EXPECT_FALSE(hit);

  // Check that the record is not updated
  EXPECT_EQ(record.t, 0);
  EXPECT_EQ(record.p, vector(0, 0, 0));
  EXPECT_EQ(record.normal, vector(0, 0, 0));
}

// ID: CYL-RND-3
TEST(CylinderRender, RayHitsButIsTooClose) {
  // Declare required inputs
  CylinderContext const ctx{
    {0.0, 0.0, 0.0},
    vector(0.0, 1.0, 0.0), 1.0, 2.0
  };
  CylinderRender const cylinder(ctx, nullptr);
  Ray const ray({1.0, 0.0, -0.0001}, vector(0.0, 0.0, 1.0));
  double const t_min = 0.001;
  double const t_max = 100.0;
  InfoColision record;

  // Perform the hit test
  bool const hit = cylinder.hit(ray, t_min, t_max, record);
  EXPECT_FALSE(hit);

  // Check that the record is not updated
  EXPECT_EQ(record.t, 0);
  EXPECT_EQ(record.p, vector(0, 0, 0));
  EXPECT_EQ(record.normal, vector(0, 0, 0));
}

// ID: CYL-RND-4
TEST(CylinderRender, RayHitsButIsTooFar) {
  // Declare required inputs
  CylinderContext const ctx{
    {0.0, 0.0, 0.0},
    vector(0.0, 1.0, 0.0), 1.0, 2.0
  };
  CylinderRender const cylinder(ctx, nullptr);
  Ray const ray({0.0, 0.0, -5.0}, vector(0.0, 0.0, 1.0));
  double const t_min = 0.001;
  double const t_max = 3.0;
  InfoColision record;

  // Perform the hit test
  bool const hit = cylinder.hit(ray, t_min, t_max, record);
  EXPECT_FALSE(hit);

  // Check that the record is not updated
  EXPECT_EQ(record.t, 0);
  EXPECT_EQ(record.p, vector(0, 0, 0));
  EXPECT_EQ(record.normal, vector(0, 0, 0));
}

// ID: CYL-RND-5
TEST(CylinderRender, RayHitsTangentToCurveSurface) {
  // Declare required inputs
  CylinderContext const ctx{
    {0.0, 0.0, 0.0},
    vector(0.0, 1.0, 0.0), 1.0, 2.0
  };
  CylinderRender const cylinder(ctx, nullptr);
  Ray const ray({1.0, 0.0, -5.0}, vector(0.0, 0.0, 1.0));
  double const t_min = 0.001;
  double const t_max = 100.0;
  InfoColision record;

  // Perform the hit test
  bool const hit = cylinder.hit(ray, t_min, t_max, record);
  EXPECT_TRUE(hit);

  // Check that the record is not updated
  EXPECT_EQ(record.t, 5.0);
  EXPECT_EQ(record.p, vector(1, 0, 0));
  EXPECT_EQ(record.normal, vector(-1, 0, 0));
}

// ID: CYL-RND-6
TEST(CylinderRender, RayHitsTwoPointsOfCurveSurface) {
  // Declare required inputs
  CylinderContext const ctx{
    {0.0, 0.0, 0.0},
    vector(0.0, 1.0, 0.0), 1.0, 2.0
  };
  CylinderRender const cylinder(ctx, nullptr);
  Ray const ray({0.0, 0.0, -5.0}, vector(0.0, 0.0, 1.0));
  double const t_min = 0.001;
  double const t_max = 100.0;
  InfoColision record;

  // Perform the hit test
  bool const hit = cylinder.hit(ray, t_min, t_max, record);
  EXPECT_TRUE(hit);

  // Check that the record is not updated
  EXPECT_EQ(record.t, 4.0);
  EXPECT_EQ(record.p, vector(0, 0, -1));
  EXPECT_EQ(record.normal, vector(0, 0, -1));
}

// ID: CYL-RND-7
TEST(CylinderRender, RayHitsInferiorBaseFirst) {
  // Declare required inputs
  CylinderContext const ctx{
    {0.0, 0.0, 0.0},
    vector(0.0, 0.0, 1.0), 1.0, 2.0
  };
  CylinderRender const cylinder(ctx, nullptr);
  Ray const ray({0.0, 0.0, -5.0}, vector(0.0, 0.0, 1.0));
  double const t_min = 0.001;
  double const t_max = 100.0;
  InfoColision record;

  // Perform the hit test
  bool const hit = cylinder.hit(ray, t_min, t_max, record);
  EXPECT_TRUE(hit);

  // Check that the record is not updated
  EXPECT_EQ(record.t, 4.0);
  EXPECT_EQ(record.p, vector(0, 0, -1));
  EXPECT_EQ(record.normal, vector(0, 0, -1));
}

// ID: CYL-RND-8
TEST(CylinderRender, RayHitsSuperiorBaseFirst) {
  // Declare required inputs
  CylinderContext const ctx{
    {0.0, 0.0, 0.0},
    vector(0.0, 0.0, 1.0), 1.0, 2.0
  };
  CylinderRender const cylinder(ctx, nullptr);
  Ray const ray({0.0, 0.0, 5.0}, vector(0.0, 0.0, -1.0));
  double const t_min = 0.001;
  double const t_max = 100.0;
  InfoColision record;

  // Perform the hit test
  bool const hit = cylinder.hit(ray, t_min, t_max, record);
  EXPECT_TRUE(hit);

  // Check that the record is not updated
  EXPECT_EQ(record.t, 4.0);
  EXPECT_EQ(record.p, vector(0, 0, 1));
  EXPECT_EQ(record.normal, vector(0, 0, 1));
}

// ID: CYL-RND-9
TEST(CylinderRender, RayHitsBaseFirstAndCurveSurfaceSecond) {
  // Declare required inputs
  CylinderContext const ctx{
    {0.0, 0.0, 0.0},
    vector(0.0, 1.0, 0.0), 1.0, 2.0
  };
  CylinderRender const cylinder(ctx, nullptr);
  Ray const ray({0.0, -2.0, -1.0}, vector(0.0, 1.0, 1.0));
  double const t_min = 0.001;
  double const t_max = 100.0;
  InfoColision record;

  // Perform the hit test
  bool const hit = cylinder.hit(ray, t_min, t_max, record);
  EXPECT_TRUE(hit);

  // Check that the record is not updated
  EXPECT_EQ(record.t, 1.0);
  EXPECT_EQ(record.p, vector(0, -1.0, 0));
  EXPECT_EQ(record.normal, vector(0, -1.0, 0));
}

// ID: CYL-RND-10
TEST(CylinderRender, InsideCylinderHit) {
  // Declare required inputs
  CylinderContext const ctx{
    {0.0, 0.0, 0.0},
    vector(0.0, 1.0, 0.0), 1.0, 2.0
  };
  CylinderRender const cylinder(ctx, nullptr);
  Ray const ray({0.0, 0.0, 0.0}, vector(0.0, 1.0, 0.0));
  double const t_min = 0.001;
  double const t_max = 100.0;
  InfoColision record;

  // Perform the hit test
  bool const hit = cylinder.hit(ray, t_min, t_max, record);
  EXPECT_TRUE(hit);

  // Check that the record is not updated
  EXPECT_EQ(record.t, 1.0);
  EXPECT_EQ(record.p, vector(0, 1, 0));
  EXPECT_EQ(record.normal, vector(0, -1, 0));
}
