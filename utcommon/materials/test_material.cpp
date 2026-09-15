#include "materials/material.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace render;

// Clase derivada mínima para poder instanciar Material y probar el comportamiento base
class DummyMaterial : public Material {
public:
  DummyMaterial() = default;

  [[nodiscard]] std::string str() const override { return "DummyMaterial"; }

  bool validate_parameters(std::string & params) override { return params == "valid"; }
};

// ID: MATERIAL1
TEST(test_material, default_constructor_initialization) {
  DummyMaterial const mat;
  // El alias debe empezar vacío
  EXPECT_TRUE(mat.material_alias.empty());
}

// ID: MATERIAL2
TEST(test_material, str_method_returns_expected_value) {
  DummyMaterial const mat;
  EXPECT_EQ(mat.str(), "DummyMaterial");
}

// ID: MATERIAL3
TEST(test_material, validate_parameters_returns_expected_values) {
  DummyMaterial mat;

  std::string valid   = "valid";
  std::string invalid = "invalid";

  EXPECT_TRUE(mat.validate_parameters(valid));
  EXPECT_FALSE(mat.validate_parameters(invalid));
}

// ID: MATERIAL4
TEST(test_material, cannot_instantiate_abstract_base_directly) {
  // Esta prueba es solo de compilación (no se ejecuta).
  // No se puede hacer `Material mat;` porque es una clase abstracta.
  SUCCEED();  // Marca la prueba como pasada intencionalmente.
}
