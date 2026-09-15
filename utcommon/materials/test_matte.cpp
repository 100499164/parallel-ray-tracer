#include "materials/matte.hpp"
#include <gtest/gtest.h>
#include <string>

TEST(test_matte, general_valid_case) {
  render::Matte matte;
  std::string params = "mat1 0 0.8 0.8";
  // Validate tha validate_parameters returns true
  EXPECT_TRUE(matte.validate_parameters(params));
  // Validate that the attributes have been correctly parsed
  EXPECT_EQ(matte.material_alias, "mat1");
  EXPECT_DOUBLE_EQ(matte.reflectance[0], 0.0);
  EXPECT_DOUBLE_EQ(matte.reflectance[1], 0.8);
  EXPECT_DOUBLE_EQ(matte.reflectance[2], 0.8);
}

TEST(test_matte, reflectance_equals_cero) {
  render::Matte matte;
  std::string params = "mat1 0 0 0";
  // Validate tha validate_parameters returns true
  EXPECT_TRUE(matte.validate_parameters(params));
  // Validate that the attributes have been correctly parsed
  EXPECT_EQ(matte.material_alias, "mat1");
  EXPECT_DOUBLE_EQ(matte.reflectance[0], 0.0);
  EXPECT_DOUBLE_EQ(matte.reflectance[1], 0.0);
  EXPECT_DOUBLE_EQ(matte.reflectance[2], 0.0);
}

TEST(test_matte, reflectance_equals_one) {
  render::Matte matte;
  std::string params = "mat1 1 1 1";
  // Validate tha validate_parameters returns true
  EXPECT_TRUE(matte.validate_parameters(params));
  // Validate that the attributes have been correctly parsed
  EXPECT_EQ(matte.material_alias, "mat1");
  EXPECT_DOUBLE_EQ(matte.reflectance[0], 1.0);
  EXPECT_DOUBLE_EQ(matte.reflectance[1], 1.0);
  EXPECT_DOUBLE_EQ(matte.reflectance[2], 1.0);
}

TEST(test_matte, not_enough_parameters) {
  render::Matte matte;
  // Captura stderr
  testing::internal::CaptureStderr();
  std::string params = "mat1 0 0.8";
  // Validate tha validate_parameters returns false
  EXPECT_FALSE(matte.validate_parameters(params));
  std::string const error_output = testing::internal::GetCapturedStderr();
  std::string const expected     = "ERROR: Invalid matte material parameters\n"
                                   "Line: 'matte: mat1 0 0.8'\n";
  EXPECT_EQ(error_output, expected);
}

TEST(test_matte, too_many_parameters) {
  render::Matte matte;
  // Captura stderr
  testing::internal::CaptureStderr();
  std::string params = "mat1 0 0.8 0.8 2.0";
  // Validate tha validate_parameters returns false
  EXPECT_FALSE(matte.validate_parameters(params));
  std::string const error_output = testing::internal::GetCapturedStderr();
  std::string const expected     = "ERROR: Extra data after configuration value for key: [matte:]\n"
                                   "Extra: '2.0'\n"
                                   "Line: 'matte: mat1 0 0.8 0.8 2.0'\n";
  EXPECT_EQ(error_output, expected);
}

TEST(test_matte, invalid_parameters) {
  render::Matte matte;
  // Captura stderr
  testing::internal::CaptureStderr();
  std::string params = "mat1 a 0.8 0.8";
  // Validate tha validate_parameters returns false
  EXPECT_FALSE(matte.validate_parameters(params));
  std::string const error_output = testing::internal::GetCapturedStderr();
  std::string const expected     = "ERROR: Invalid matte material parameters\n"
                                   "Line: 'matte: mat1 a 0.8 0.8'\n";
  EXPECT_EQ(error_output, expected);
}

TEST(test_matte, less_than_cero_reflectance) {
  render::Matte matte;
  // Captura stderr
  testing::internal::CaptureStderr();
  std::string params = "mat1 -0.001 0.8 0.8";
  // Validate tha validate_parameters returns true
  EXPECT_FALSE(matte.validate_parameters(params));
  std::string const error_output = testing::internal::GetCapturedStderr();
  std::string const expected     = "ERROR: Invalid matte material parameters\n"
                                   "Line: 'matte: mat1 -0.001 0.8 0.8'\n";
  EXPECT_EQ(error_output, expected);
}

TEST(test_matte, more_than_one_reflectance) {
  render::Matte matte;
  // Captura stderr
  testing::internal::CaptureStderr();
  std::string params = "mat1 1.001 0.8 0.8";
  // Validate tha validate_parameters returns false
  EXPECT_FALSE(matte.validate_parameters(params));
  std::string const error_output = testing::internal::GetCapturedStderr();
  std::string const expected     = "ERROR: Invalid matte material parameters\n"
                                   "Reflective values must be in the interval [0, 1]\n"
                                   "Line: 'matte: mat1 1.001 0.8 0.8'\n";
  EXPECT_EQ(error_output, expected);
}

TEST(test_matte, string_representation) {
  render::Matte matte;
  std::string params = "mat1 0.2 0.5 0.8";
  ASSERT_TRUE(matte.validate_parameters(params));

  std::string const expected = "matte: mat1 0.2 0.5 0.8 ";
  EXPECT_EQ(matte.str(), expected);
}
