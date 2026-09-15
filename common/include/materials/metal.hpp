#ifndef METAL_HPP
#define METAL_HPP

#include <array>
#include <string>

#include "material.hpp"

namespace render {

  class Metal final : public Material {
  public:
    Metal();

    // Define the attributes
    std::array<double, 3> reflectance;
    double diffusiveness = 1.0;

    // Define the methods
    [[nodiscard]] std::string str() const override;
    bool validate_parameters(std::string & Parameters) override;

  protected:
    std::regex regex_pattern;
  };

}  // namespace render

#endif
