#ifndef REFRACTIVE_HPP
#define REFRACTIVE_HPP

#include <string>

#include "material.hpp"

namespace render {

  class Refractive : public Material {
  public:
    Refractive();

    // Define the attributes
    double refraction_index = 1.0;

    // Define the methods
    [[nodiscard]] std::string str() const override;
    bool validate_parameters(std::string & Parameters) override;

  protected:
    std::regex regex_pattern;
  };

}  // namespace render

#endif
