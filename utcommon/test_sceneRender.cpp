#include "figures/cylinder.hpp"
#include "figures/figure.hpp"
#include "figures/sphere.hpp"
#include "infocolision.hpp"
#include "materials/matte.hpp"
#include "ray.hpp"
#include "sceneRender.hpp"
#include "vector.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <string>

using namespace render;

// ID: SCN-RND-1
TEST(test_sceneRender, EmptyScene) {
  // Create the scene
  SceneRender const scn;

  // Declare required inputs
  Ray const ray({0.0, 0.0, -5.0}, vector(0.0, 0.0, 1.0));
  InfoColision record;
  double const t_min = 0.001;
  double const t_max = 100.0;

  // Compute the intersection between the ray and the scene
  bool const hit = scn.hit(ray, t_min, t_max, record);
  EXPECT_FALSE(hit);

  // Check that the record is not updated
  EXPECT_EQ(record.t, 0);
  EXPECT_EQ(record.p, vector(0, 0, 0));
  EXPECT_EQ(record.normal, vector(0, 0, 0));
}

// ID: SCN-RND-2
TEST(test_sceneRender, RayGoesBetweenFigures) {
  // Create the scene
  SceneRender scn;
  std::string params_sph1 = "-2.0 0.0 0.0 1.0 mat1";
  auto sph1               = std::make_shared<Sphere>();
  sph1->validate_parameters(params_sph1);
  scn.add_render_figure(sph1, nullptr);
  std::string params_sph2 = "2.0 0.0 0.0 1.0 mat1";
  auto sph2               = std::make_shared<Sphere>();
  sph2->validate_parameters(params_sph2);
  scn.add_render_figure(sph2, nullptr);

  // Declare required inputs
  Ray const ray({0.0, 0.0, -5.0}, vector(0.0, 0.0, 1.0));
  InfoColision record;
  double const t_min = 0.001;
  double const t_max = 100.0;

  // Compute the intersection between the ray and the scene
  bool const hit = scn.hit(ray, t_min, t_max, record);
  EXPECT_FALSE(hit);

  // Check that the record is not updated
  EXPECT_EQ(record.t, 0);
  EXPECT_EQ(record.p, vector(0, 0, 0));
  EXPECT_EQ(record.normal, vector(0, 0, 0));
}

// ID: SCN-RND-3
TEST(test_sceneRender, RayHitsFigureButIsTooFar) {
  // Create the scene
  SceneRender scn;
  std::string params_sph1 = "0.0 0.0 0.0 1.0 mat1";
  auto sph1               = std::make_shared<Sphere>();
  sph1->validate_parameters(params_sph1);
  scn.add_render_figure(sph1, nullptr);

  // Declare required inputs
  Ray const ray({0.0, 0.0, -5.0}, vector(0.0, 0.0, 1.0));
  InfoColision record;
  double const t_min = 0.001;
  double const t_max = 3.0;

  // Compute the intersection between the ray and the scene
  bool const hit = scn.hit(ray, t_min, t_max, record);
  EXPECT_FALSE(hit);

  // Check that the record is not updated
  EXPECT_EQ(record.t, 0);
  EXPECT_EQ(record.p, vector(0, 0, 0));
  EXPECT_EQ(record.normal, vector(0, 0, 0));
}

// ID: SCN-RND-4
TEST(test_sceneRender, SceneWith2FiguresOneNearAnother) {
  // Create the scene
  SceneRender scn;

  // Create a matte material to assign to the spheres
  std::string params_mat = "0.2 0.4 0.6";
  auto matte             = std::make_shared<Matte>();
  matte->material_alias  = "mat1";
  matte->validate_parameters(params_mat);

  // Create two spheres side by side
  std::string params_sph1 = "-2.0 0.0 0.0 1.0 mat1";
  auto sph1               = std::make_shared<Sphere>();
  sph1->validate_parameters(params_sph1);
  scn.add_render_figure(sph1, matte);
  std::string params_sph2 = "2.0 0.0 0.0 1.0 mat1";
  auto sph2               = std::make_shared<Sphere>();
  sph2->validate_parameters(params_sph2);
  scn.add_render_figure(sph2, matte);

  // Declare required inputs
  Ray const ray({2.0, 0.0, -5.0}, vector(0.0, 0.0, 1.0));
  InfoColision record;
  double const t_min = 0.001;
  double const t_max = 100.0;

  // Compute the intersection between the ray and the scene
  bool const hit = scn.hit(ray, t_min, t_max, record);
  EXPECT_TRUE(hit);

  // Check that the record is correctly updated
  EXPECT_EQ(record.t, 4.0);
  EXPECT_EQ(record.p, vector(2.0, 0, -1.0));
  EXPECT_EQ(record.normal, vector(0, 0, -1.0));
}

// ID: SCN-RND-5
TEST(test_sceneRender, SceneWith2FiguresOneBehindAnother) {
  // Create the scene
  SceneRender scn;

  // Create a matte material to assign to the spheres
  std::string params_mat = "0.2 0.4 0.6";
  auto matte             = std::make_shared<Matte>();
  matte->material_alias  = "mat1";
  matte->validate_parameters(params_mat);

  // Create two spheres aligned with the ray
  std::string params_sph2 = "0.0 0.0 -3.0 1.0 mat1";
  auto sph2               = std::make_shared<Sphere>();
  sph2->validate_parameters(params_sph2);
  scn.add_render_figure(sph2, matte);
  std::string params_sph1 = "0.0 0.0 0.0 1.0 mat1";
  auto sph1               = std::make_shared<Sphere>();
  sph1->validate_parameters(params_sph1);
  scn.add_render_figure(sph1, matte);

  // Declare required inputs
  Ray const ray({0.0, 0.0, -5.0}, vector(0.0, 0.0, 1.0));
  InfoColision record;
  double const t_min = 0.001;
  double const t_max = 100.0;

  // Compute the intersection between the ray and the scene (takes sph2)
  bool const hit = scn.hit(ray, t_min, t_max, record);
  EXPECT_TRUE(hit);

  // Check that the record is correctly updated
  EXPECT_EQ(record.t, 1.0);
  EXPECT_EQ(record.p, vector(0.0, 0, -4.0));
  EXPECT_EQ(record.normal, vector(0, 0, -1.0));
}

// ID: SCN-RND-6
TEST(test_sceneRender, SceneWith2FiguresOneNextToAnother) {
  // Create the scene
  SceneRender scn;

  // Create a matte material to assign to the spheres
  std::string params_mat = "0.2 0.4 0.6";
  auto matte             = std::make_shared<Matte>();
  matte->material_alias  = "mat1";
  matte->validate_parameters(params_mat);

  // Create two spheres side by side
  std::string params_sph1 = "-2.0 0.0 0.0 2.0 mat1";
  auto sph1               = std::make_shared<Sphere>();
  sph1->validate_parameters(params_sph1);
  scn.add_render_figure(sph1, matte);
  std::string params_sph2 = "2.0 0.0 0.0 2.0 mat1";
  auto sph2               = std::make_shared<Sphere>();
  sph2->validate_parameters(params_sph2);
  scn.add_render_figure(sph2, matte);

  // Declare required inputs
  Ray const ray({0.0, 0.0, -5.0}, vector(0.0, 0.0, 1.0));
  InfoColision record;
  double const t_min = 0.001;
  double const t_max = 100.0;

  // Compute the intersection between the ray and the scene (takes sph1)
  bool const hit = scn.hit(ray, t_min, t_max, record);
  EXPECT_TRUE(hit);

  // Check that the record is correctly updated
  EXPECT_EQ(record.t, 5.0);
  EXPECT_EQ(record.p, vector(0.0, 0.0, 0.0));
  EXPECT_EQ(record.normal, vector(-1.0, 0, 0.0));
}

// ID: SCN-RND-7
TEST(test_sceneRender, ClearSceneAndSize) {
  // Create the scene
  SceneRender scn;

  // Create a matte material to assign to the spheres
  std::string params_mat = "0.2 0.4 0.6";
  auto matte             = std::make_shared<Matte>();
  matte->material_alias  = "mat1";
  matte->validate_parameters(params_mat);

  // Create a sphere and add it to the scene
  std::string params_sph = "0.0 0.0 0.0 1.0 mat1";
  auto sph               = std::make_shared<Sphere>();
  sph->validate_parameters(params_sph);
  scn.add_render_figure(sph, matte);

  EXPECT_EQ(scn.size(), 1);
  scn.clear();
  EXPECT_EQ(scn.size(), 0);
}

// ID: SCN-RND-8
TEST(test_sceneRender, AddCylinderFigure) {
  // Create the scene
  SceneRender scn;

  // Create a cylinder and add it to the scene
  std::string params_cyl = "0.0 0.0 0.0 1.0 0 1 0 mat1";
  auto cyl               = std::make_shared<Cylinder>();
  cyl->validate_parameters(params_cyl);

  // Create a material (scene material, not render material)
  std::string params_mat = "0.2 0.4 0.6";
  auto matte             = std::make_shared<Matte>();
  matte->material_alias  = "mat1";
  matte->validate_parameters(params_mat);
  scn.add_render_figure(cyl, matte);

  // Check that the scene contains one element
  EXPECT_EQ(scn.size(), 1);
}

// ID: SCN-RND-9
TEST(test_sceneRender, AddUnsupportedFigureType) {
  // Create the scene
  SceneRender scn;

  // Create a dummy figure that is not supported
  class DummyFigure : public Figure {
  public:
    bool validate_parameters(std::string &) override { return true; }

    [[nodiscard]] std::string str() const override { return "dummy"; }
  };

  auto dummy = std::make_shared<DummyFigure>();

  // Create a material (scene material, not render material)
  std::string params_mat = "0.2 0.4 0.6";
  auto matte             = std::make_shared<Matte>();
  matte->material_alias  = "mat1";
  matte->validate_parameters(params_mat);

  // Capture stdout to check the output
  testing::internal::CaptureStdout();
  scn.add_render_figure(dummy, matte);
  std::string const output = testing::internal::GetCapturedStdout();

  // Verify that the message and size are correct
  EXPECT_EQ(output, "Unsupported figure type\n");
  EXPECT_EQ(scn.size(), 0);
}
