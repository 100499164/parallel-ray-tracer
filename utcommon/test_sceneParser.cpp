#include "figures/cylinder.hpp"
#include "figures/sphere.hpp"
#include "sceneParser.hpp"
#include <cstddef>
#include <gtest/gtest.h>
#include <memory>
#include <sstream>
#include <string>

TEST(test_sceneParser, parse_valid_file) {
  render::sceneParser scnParser;

  // 🔹 Simulamos el contenido del archivo directamente en memoria
  std::string const scenario_text = R"(
matte: mat1 0 0.8 0.8
metal: metal1 0 0.8 0 2.0
refractive: ref99 1.3
sphere: 0 0 0 0.65 mat1
cylinder: 0 0 0 0.5 20 10 -5 metal1
)";

  std::istringstream fake_file(scenario_text);

  EXPECT_TRUE(scnParser.parse(fake_file));

  EXPECT_EQ(scnParser.materials.size(), 3);

  EXPECT_TRUE(scnParser.materials.contains("mat1"));
  EXPECT_TRUE(scnParser.materials.contains("metal1"));
  EXPECT_TRUE(scnParser.materials.contains("ref99"));

  EXPECT_EQ(scnParser.figures.size(), 2);

  size_t sphere_count   = 0;
  size_t cylinder_count = 0;

  for (auto const & fptr : scnParser.figures) {
    if (std::dynamic_pointer_cast<render::Sphere>(fptr)) {
      ++sphere_count;
    } else if (std::dynamic_pointer_cast<render::Cylinder>(fptr)) {
      ++cylinder_count;
    }
  }

  EXPECT_EQ(sphere_count, 1);
  EXPECT_EQ(cylinder_count, 1);
}

TEST(test_sceneParser, unknown_material) {
  render::sceneParser scnParser;

  std::string const scenario_text = R"(
granito: mat1 0 0.8 0.8
metal: metal1 0 0.8 0 2.0
refractive: ref99 1.3
sphere: 0 0 0 0.65 mat1
cylinder: 0 0 0 0.5 20 10 -5 metal1
)";

  std::istringstream fake_file(scenario_text);

  // Capturamos stderr para validar el mensaje de error
  testing::internal::CaptureStderr();
  EXPECT_FALSE(scnParser.parse(fake_file));
  std::string const error_output = testing::internal::GetCapturedStderr();

  std::string const expected = "ERROR: Unknown scene entity: granito\n"
                               "Line: 'granito: mat1 0 0.8 0.8'\n";

  EXPECT_EQ(error_output, expected);
}

TEST(test_sceneParser, unknown_figure) {
  render::sceneParser scnParser;

  std::string const scenario_text = R"(
matte: mat1 0 0.8 0.8
metal: metal1 0 0.8 0 2.0
refractive: ref99 1.3
triangle: 0 0 0 0.65 mat1
cylinder: 0 0 0 0.5 20 10 -5 metal1
)";

  std::istringstream fake_file(scenario_text);

  // Capturamos stderr para validar el mensaje de error
  testing::internal::CaptureStderr();
  EXPECT_FALSE(scnParser.parse(fake_file));
  std::string const error_output = testing::internal::GetCapturedStderr();

  std::string const expected = "ERROR: Unknown scene entity: triangle\n"
                               "Line: 'triangle: 0 0 0 0.65 mat1'\n";

  EXPECT_EQ(error_output, expected);
}

TEST(test_sceneParser, undeclared_material) {
  render::sceneParser scnParser;

  std::string const scenario_text = R"(
matte: mat1 0 0.8 0.8
metal: metal1 0 0.8 0 2.0
refractive: ref99 1.3
sphere: 0 0 0 0.65 mat2
cylinder: 0 0 0 0.5 20 10 -5 metal1
)";

  std::istringstream fake_file(scenario_text);

  // Capturamos stderr para validar el mensaje de error
  testing::internal::CaptureStderr();
  EXPECT_FALSE(scnParser.parse(fake_file));
  std::string const error_output = testing::internal::GetCapturedStderr();

  std::string const expected = "ERROR: Material not found: [mat2]\n"
                               "Line: 'sphere: 0 0 0 0.65 mat2'\n";

  EXPECT_EQ(error_output, expected);
}

TEST(test_sceneParser, duplicated_material) {
  render::sceneParser scnParser;

  std::string const scenario_text = R"(
matte: mat1 0 0.8 0.8
metal: mat1 0 0.8 0 2.0
refractive: ref99 1.3
sphere: 0 0 0 0.65 mat1
cylinder: 0 0 0 0.5 20 10 -5 mat1
)";

  std::istringstream fake_file(scenario_text);

  // Capturamos stderr para validar el mensaje de error
  testing::internal::CaptureStderr();
  EXPECT_FALSE(scnParser.parse(fake_file));
  std::string const error_output = testing::internal::GetCapturedStderr();

  std::string const expected = "ERROR: Material with name [mat1] already exists\n"
                               "Line: 'metal: mat1 0 0.8 0 2.0'\n";

  EXPECT_EQ(error_output, expected);
}

TEST(test_sceneParser, invalid_parameters) {
  render::sceneParser scnParser;

  std::string const scenario_text = R"(
matte: mat1 0 0.8 0.8
metal: metal1 0 a 0 2.0
refractive: ref99 1.3
sphere: 0 0 0 0.65 mat1
cylinder: 0 0 0 0.5 20 10 -5 metal1
)";

  std::istringstream fake_file(scenario_text);

  // Capturamos stderr para validar el mensaje de error
  testing::internal::CaptureStderr();
  EXPECT_FALSE(scnParser.parse(fake_file));
  std::string const error_output = testing::internal::GetCapturedStderr();

  std::string const expected = "ERROR: Invalid metal material parameters\n"
                               "Line: 'metal:  metal1 0 a 0 2.0'\n";

  EXPECT_EQ(error_output, expected);
}

TEST(test_sceneParser, not_use_semicolon) {
  render::sceneParser scnParser;

  std::string const scenario_text = R"(
matte;: mat1 0 0.8 0.8
metal: metal1 0 0.8 0 2.0
refractive: ref99 1.3
sphere: 0 0 0 0.65 mat1
cylinder: 0 0 0 0.5 20 10 -5 metal1
)";

  std::istringstream fake_file(scenario_text);

  // Capturamos stderr para validar el mensaje de error
  testing::internal::CaptureStderr();
  EXPECT_FALSE(scnParser.parse(fake_file));
  std::string const error_output = testing::internal::GetCapturedStderr();

  std::string const expected = "ERROR: Unknown scene entity: matte;\n"
                               "Line: 'matte;: mat1 0 0.8 0.8'\n";

  EXPECT_EQ(error_output, expected);
}

TEST(test_sceneParser, additional_space_before_colon) {
  render::sceneParser scnParser;

  std::string const scenario_text = R"(
matte : mat1 0 0.8 0.8
metal: metal1 0 0.8 0 2.0
refractive: ref99 1.3
sphere: 0 0 0 0.65 mat1
cylinder: 0 0 0 0.5 20 10 -5 metal1
)";

  std::istringstream fake_file(scenario_text);

  // Capturamos stderr para validar el mensaje de error
  testing::internal::CaptureStderr();
  EXPECT_FALSE(scnParser.parse(fake_file));
  std::string const error_output = testing::internal::GetCapturedStderr();

  std::string const expected = "ERROR: Unknown scene entity: matte \n"
                               "Line: 'matte : mat1 0 0.8 0.8'\n";

  EXPECT_EQ(error_output, expected);
}

TEST(test_sceneParser, invalid_path) {
  render::sceneParser scnParser;

  std::string const invalid_path = "/this/path/does/not/exist.txt";

  // Capturamos stderr para validar el mensaje de error
  testing::internal::CaptureStderr();
  EXPECT_FALSE(scnParser.parse(invalid_path));
  std::string const error_output = testing::internal::GetCapturedStderr();

  std::string const expected = "ERROR: Failed to open file: /this/path/does/not/exist.txt\n";

  EXPECT_EQ(error_output, expected);
}

TEST(test_sceneParser, missing_colon_in_line) {
  render::sceneParser scnParser;
  std::string const scenario_text = R"(
matte mat1 0 0.8 0.8
metal: metal1 0 0.8 0 2.0
refractive: ref99 1.3
sphere: 0 0 0 0.65 mat1
cylinder: 0 0 0 0.5 20 10 -5 metal1
)";
  std::istringstream fake_file(scenario_text);
  testing::internal::CaptureStderr();
  EXPECT_FALSE(scnParser.parse(fake_file));
  std::string const error_output = testing::internal::GetCapturedStderr();
  std::string const expected = "ERROR: Invalid line format (missing ':'): matte mat1 0 0.8 0.8\n";
  EXPECT_EQ(error_output, expected);
}
