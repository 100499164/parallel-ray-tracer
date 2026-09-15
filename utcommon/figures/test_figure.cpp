#include "figures/figure.hpp"
#include <gtest/gtest.h>

using namespace render;

// ID: FIGURE1
TEST(test_figure, default_constructor_initialization) {
  Figure fig;

  // Validar posición inicial
  EXPECT_DOUBLE_EQ(fig.position[0], 30.0);
  EXPECT_DOUBLE_EQ(fig.position[1], 0.0);
  EXPECT_DOUBLE_EQ(fig.position[2], 0.0);

  // Validar radio inicial
  EXPECT_DOUBLE_EQ(fig.radius, 5.0);

  // El material_name debería empezar vacío
  EXPECT_TRUE(fig.material_name.empty());
}

// ID: FIGURE2
TEST(test_figure, str_output_correct_format) {
  Figure fig;
  fig.material_name = "test_material";

  std::string const expected = "Figure(position=[30, 0, 0], radius=5, material_name=test_material)";

  EXPECT_EQ(fig.str(), expected);
}

// ID: FIGURE3
TEST(test_figure, validate_parameters_always_returns_false) {
  Figure fig;
  std::string dummy_params = "any string";
  EXPECT_FALSE(fig.validate_parameters(dummy_params));
}
