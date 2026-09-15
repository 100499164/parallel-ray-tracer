#include "figures/cylinder.hpp"
#include <cmath>
#include <gtest/gtest.h>
#include <string>

TEST(test_cylinder, general_valid_case) {
  render::Cylinder cylinder;
  std::string params = "0 0 0 0.5 20 10 -5 metal1";
  // Validate that validate_parameters returns true
  EXPECT_TRUE(cylinder.validate_parameters(params));
  // Validate that the attributes have been correctly parsed
  EXPECT_DOUBLE_EQ(cylinder.position[0], 0.0);
  EXPECT_DOUBLE_EQ(cylinder.position[1], 0.0);
  EXPECT_DOUBLE_EQ(cylinder.position[2], 0.0);
  EXPECT_DOUBLE_EQ(cylinder.radius, 0.5);
  EXPECT_DOUBLE_EQ(cylinder.axis.get_x(), 20.0);
  EXPECT_DOUBLE_EQ(cylinder.axis.get_y(), 10.0);
  EXPECT_DOUBLE_EQ(cylinder.axis.get_z(), -5.0);
  EXPECT_DOUBLE_EQ(cylinder.height, std::sqrt(20.0 * 20.0 + 10.0 * 10.0 + (-5.0) * (-5.0)));
  EXPECT_EQ(cylinder.material_name, "metal1");
}

TEST(test_cylinder, not_enough_parameters) {
  render::Cylinder cylinder;
  testing::internal::CaptureStderr();
  std::string params = "0 0 0 0.5 20 10 metal1";
  // Validate that validate_parameters returns false
  EXPECT_FALSE(cylinder.validate_parameters(params));
  // Validate that the attributes have been correctly parsed
  std::string const error_output = testing::internal::GetCapturedStderr();
  std::string const expected     = "ERROR: Invalid cylinder parameters\n"
                                   "Line: 'cylinder: 0 0 0 0.5 20 10 metal1'\n";
  EXPECT_EQ(error_output, expected);
}

TEST(test_cylinder, too_many_parameters) {
  render::Cylinder cylinder;
  testing::internal::CaptureStderr();
  std::string params = "0 0 0 0.5 20 10 -5 20 metal1";
  // Validate that validate_parameters returns false
  EXPECT_FALSE(cylinder.validate_parameters(params));
  // Validate that the attributes have been correctly parsed
  std::string const error_output = testing::internal::GetCapturedStderr();
  std::string const expected = "ERROR: Extra data after configuration value for key: [cylinder:]\n"
                               "Extra: 'metal1'\n"
                               "Line: 'cylinder: 0 0 0 0.5 20 10 -5 20 metal1'\n";
  EXPECT_EQ(error_output, expected);
}

TEST(test_cylinder, invalid_parameters) {
  render::Cylinder cylinder;
  testing::internal::CaptureStderr();
  std::string params = "0 0 0 a 20 10 -5 metal1";
  // Validate that validate_parameters returns false
  EXPECT_FALSE(cylinder.validate_parameters(params));
  // Validate that the attributes have been correctly parsed
  std::string const error_output = testing::internal::GetCapturedStderr();
  std::string const expected     = "ERROR: Invalid cylinder parameters\n"
                                   "Line: 'cylinder: 0 0 0 a 20 10 -5 metal1'\n";
  EXPECT_EQ(error_output, expected);
}

TEST(test_cylinder, negative_radius) {
  render::Cylinder cylinder;
  testing::internal::CaptureStderr();
  std::string params = "0 0 0 -0.5 20 10 -5 metal1";
  // Validate that validate_parameters returns false
  EXPECT_FALSE(cylinder.validate_parameters(params));
  // Validate that the attributes have been correctly parsed
  std::string const error_output = testing::internal::GetCapturedStderr();
  std::string const expected     = "ERROR: Invalid cylinder parameters\n"
                                   "Line: 'cylinder: 0 0 0 -0.5 20 10 -5 metal1'\n";
  EXPECT_EQ(error_output, expected);
}

TEST(test_cylinder, str_output_correct) {
  render::Cylinder cylinder;
  std::string params = "1 2 3 0.5 0 0 10 metal1";
  cylinder.validate_parameters(params);

  std::string const expected = "cylinder: 1 2 3 0.5 0 0 10 metal1";
  EXPECT_EQ(cylinder.str(), expected);
}
