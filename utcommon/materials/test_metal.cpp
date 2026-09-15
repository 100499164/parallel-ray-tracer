#include "materials/metal.hpp"
#include <gtest/gtest.h>
#include <string>

TEST(test_Metal, general_valid_case) {
  render::Metal metal;
  std::string params = "metal1 0 0.8 0 2.0";
  // Validate tha validate_parameters returns true
  EXPECT_TRUE(metal.validate_parameters(params));
  // Validate that the attributes have been correctly parsed
  EXPECT_EQ(metal.material_alias, "metal1");
  EXPECT_DOUBLE_EQ(metal.reflectance[0], 0.0);
  EXPECT_DOUBLE_EQ(metal.reflectance[1], 0.8);
  EXPECT_DOUBLE_EQ(metal.reflectance[2], 0.0);
  EXPECT_DOUBLE_EQ(metal.diffusiveness, 2.0);
}

TEST(test_Metal, reflectance_equals_cero) {
  render::Metal metal;
  std::string params = "metal1 0 0 0 2.0";
  // Validate tha validate_parameters returns true
  EXPECT_TRUE(metal.validate_parameters(params));
  // Validate that the attributes have been correctly parsed
  EXPECT_EQ(metal.material_alias, "metal1");
  EXPECT_DOUBLE_EQ(metal.reflectance[0], 0.0);
  EXPECT_DOUBLE_EQ(metal.reflectance[1], 0.0);
  EXPECT_DOUBLE_EQ(metal.reflectance[2], 0.0);
  EXPECT_DOUBLE_EQ(metal.diffusiveness, 2.0);
}

TEST(test_Metal, reflectance_equals_one) {
  render::Metal metal;
  std::string params = "metal1 1 1 1 2.0";
  // Validate tha validate_parameters returns true
  EXPECT_TRUE(metal.validate_parameters(params));
  // Validate that the attributes have been correctly parsed
  EXPECT_EQ(metal.material_alias, "metal1");
  EXPECT_DOUBLE_EQ(metal.reflectance[0], 1.0);
  EXPECT_DOUBLE_EQ(metal.reflectance[1], 1.0);
  EXPECT_DOUBLE_EQ(metal.reflectance[2], 1.0);
  EXPECT_DOUBLE_EQ(metal.diffusiveness, 2.0);
}

TEST(test_Metal, not_enough_parameters) {
  render::Metal metal;
  // Captura stderr
  testing::internal::CaptureStderr();
  std::string params = "metal1 0 0.8";
  // Validate tha validate_parameters returns false
  EXPECT_FALSE(metal.validate_parameters(params));
  std::string const error_output = testing::internal::GetCapturedStderr();
  std::string const expected     = "ERROR: Invalid metal material parameters\n"
                                   "Line: 'metal: metal1 0 0.8'\n";
  EXPECT_EQ(error_output, expected);
}

TEST(test_Metal, too_many_parameters) {
  render::Metal metal;
  // Captura stderr
  testing::internal::CaptureStderr();
  std::string params = "metal1 0 0.8 0 2.0 2.0";
  // Validate tha validate_parameters returns false
  EXPECT_FALSE(metal.validate_parameters(params));
  std::string const error_output = testing::internal::GetCapturedStderr();
  std::string const expected     = "ERROR: Extra data after configuration value for key: [metal:]\n"
                                   "Extra: '2.0'\n"
                                   "Line: 'metal: metal1 0 0.8 0 2.0 2.0'\n";
  EXPECT_EQ(error_output, expected);
}

TEST(test_Metal, invalid_parameters) {
  render::Metal metal;
  // Captura stderr
  testing::internal::CaptureStderr();
  std::string params = "metal1 a 0.8 0 2.0";
  // Validate tha validate_parameters returns false
  EXPECT_FALSE(metal.validate_parameters(params));
  std::string const error_output = testing::internal::GetCapturedStderr();
  std::string const expected     = "ERROR: Invalid metal material parameters\n"
                                   "Line: 'metal: metal1 a 0.8 0 2.0'\n";
  EXPECT_EQ(error_output, expected);
}

TEST(test_Metal, less_than_cero_reflectance) {
  render::Metal metal;
  // Captura stderr
  testing::internal::CaptureStderr();
  std::string params = "metal1 -0.001 0.8 0 2.0";
  // Validate tha validate_parameters returns true
  EXPECT_FALSE(metal.validate_parameters(params));
  std::string const error_output = testing::internal::GetCapturedStderr();
  std::string const expected     = "ERROR: Invalid metal material parameters\n"
                                   "Line: 'metal: metal1 -0.001 0.8 0 2.0'\n";
  EXPECT_EQ(error_output, expected);
}

TEST(test_Metal, more_than_one_reflectance) {
  render::Metal metal;
  // Captura stderr
  testing::internal::CaptureStderr();
  std::string params = "metal1 1.001 0.8 0 2.0";
  // Validate tha validate_parameters returns true
  EXPECT_FALSE(metal.validate_parameters(params));
  std::string const error_output = testing::internal::GetCapturedStderr();
  std::string const expected     = "ERROR: Invalid metal material parameters\n"
                                   "Reflective values must be in the interval [0, 1]\n"
                                   "Line: 'metal: metal1 1.001 0.8 0 2.0'\n";
  EXPECT_EQ(error_output, expected);
}

TEST(test_Metal, string_representation) {
  render::Metal metal;
  std::string params = "metal1 0.1 0.2 0.3 0.4";
  ASSERT_TRUE(metal.validate_parameters(params));

  std::string const expected = "metal: metal1 0.1 0.2 0.3 0.4";
  EXPECT_EQ(metal.str(), expected);
}
