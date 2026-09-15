#include <gtest/gtest.h>
#include <string>

#include "materials/refractive.hpp"

TEST(test_refractive, general_valid_case) {
  render::Refractive refractive;
  std::string params = "ref99 1.3";
  // Validate tha validate_parameters returns true
  EXPECT_TRUE(refractive.validate_parameters(params));
  // Validate that the attributes have been correctly parsed
  EXPECT_EQ(refractive.material_alias, "ref99");
  EXPECT_DOUBLE_EQ(refractive.refraction_index, 1.3);
}

TEST(test_refractive, not_enough_parameters) {
  render::Refractive refractive;
  // Captura stderr
  testing::internal::CaptureStderr();
  std::string params = "ref99";
  // Validate tha validate_parameters returns false
  EXPECT_FALSE(refractive.validate_parameters(params));
  std::string const error_output = testing::internal::GetCapturedStderr();
  std::string const expected     = "ERROR: Invalid refractive material parameters\n"
                                   "Line: 'refractive: ref99'\n";
  EXPECT_EQ(error_output, expected);
}

TEST(test_refractive, too_many_parameters) {
  render::Refractive refractive;
  // Captura stderr
  testing::internal::CaptureStderr();
  std::string params = "ref99 1.3 2.0";
  // Validate tha validate_parameters returns false
  EXPECT_FALSE(refractive.validate_parameters(params));
  std::string const error_output = testing::internal::GetCapturedStderr();
  std::string const expected =
      "ERROR: Extra data after configuration value for key: [refractive:]\n"
      "Extra: '2.0'\n"
      "Line: 'refractive: ref99 1.3 2.0'\n";
  EXPECT_EQ(error_output, expected);
}

TEST(test_refractive, invalid_parameters) {
  render::Refractive refractive;
  // Captura stderr
  testing::internal::CaptureStderr();
  std::string params = "ref99 a";
  // Validate tha validate_parameters returns false
  EXPECT_FALSE(refractive.validate_parameters(params));
  std::string const error_output = testing::internal::GetCapturedStderr();
  std::string const expected     = "ERROR: Invalid refractive material parameters\n"
                                   "Line: 'refractive: ref99 a'\n";
  EXPECT_EQ(error_output, expected);
}

TEST(test_refractive, string_representation) {
  render::Refractive refractive;
  std::string params = "ref99 1.3";
  ASSERT_TRUE(refractive.validate_parameters(params));

  std::string const expected = "refractive: ref99 1.3";
  EXPECT_EQ(refractive.str(), expected);
}
