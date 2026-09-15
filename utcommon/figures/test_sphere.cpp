#include "figures/sphere.hpp"
#include <gtest/gtest.h>
#include <string>

TEST(test_sphere, general_valid_case) {
  render::Sphere sphere;
  std::string params = "0 0 0 0.65 mat1";
  // Validate that validate_parameters returns true
  EXPECT_TRUE(sphere.validate_parameters(params));
  // Validate that the attributes have been correctly parsed
  EXPECT_DOUBLE_EQ(sphere.position[0], 0.0);
  EXPECT_DOUBLE_EQ(sphere.position[1], 0.0);
  EXPECT_DOUBLE_EQ(sphere.position[2], 0.0);
  EXPECT_DOUBLE_EQ(sphere.radius, 0.65);
  EXPECT_EQ(sphere.material_name, "mat1");
}

TEST(test_sphere, not_enough_parameters) {
  render::Sphere sphere;
  testing::internal::CaptureStderr();
  std::string params = "0 0 0 mat1";
  // Validate that validate_parameters returns false
  EXPECT_FALSE(sphere.validate_parameters(params));
  // Validate that the attributes have been correctly parsed
  std::string const error_output = testing::internal::GetCapturedStderr();
  std::string const expected     = "ERROR: Invalid sphere parameters\n"
                                   "Line: 'sphere: 0 0 0 mat1'\n";
  EXPECT_EQ(error_output, expected);
}

TEST(test_sphere, too_many_parameters) {
  render::Sphere sphere;
  testing::internal::CaptureStderr();
  std::string params = "0 0 0 0.65 20 mat1";
  // Validate that validate_parameters returns false
  EXPECT_FALSE(sphere.validate_parameters(params));
  // Validate that the attributes have been correctly parsed
  std::string const error_output = testing::internal::GetCapturedStderr();
  std::string const expected = "ERROR: Extra data after configuration value for key: [sphere:]\n"
                               "Extra: 'mat1'\n"
                               "Line: 'sphere: 0 0 0 0.65 20 mat1'\n";
  EXPECT_EQ(error_output, expected);
}

TEST(test_sphere, invalid_parameters) {
  render::Sphere sphere;
  testing::internal::CaptureStderr();
  std::string params = "0 0 a 0.65 mat1";
  // Validate that validate_parameters returns false
  EXPECT_FALSE(sphere.validate_parameters(params));
  // Validate that the attributes have been correctly parsed
  std::string const error_output = testing::internal::GetCapturedStderr();
  std::string const expected     = "ERROR: Invalid sphere parameters\n"
                                   "Line: 'sphere: 0 0 a 0.65 mat1'\n";
  EXPECT_EQ(error_output, expected);
}

TEST(test_sphere, negative_radius) {
  render::Sphere sphere;
  testing::internal::CaptureStderr();
  std::string params = "0 0 0 -0.65 mat1";
  // Validate that validate_parameters returns false
  EXPECT_FALSE(sphere.validate_parameters(params));
  // Validate that the attributes have been correctly parsed
  std::string const error_output = testing::internal::GetCapturedStderr();
  std::string const expected     = "ERROR: Invalid sphere parameters\n"
                                   "Line: 'sphere: 0 0 0 -0.65 mat1'\n";
  EXPECT_EQ(error_output, expected);
}

TEST(test_sphere, str_output_correct) {
  render::Sphere sphere;
  std::string params = "1 2 3 4 mat1";
  sphere.validate_parameters(params);

  std::string const expected = "sphere: 1 2 3 4 mat1";
  EXPECT_EQ(sphere.str(), expected);
}
