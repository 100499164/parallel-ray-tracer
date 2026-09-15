#ifndef CYLINDER_HPP
#define CYLINDER_HPP

#include <string>

#include "figure.hpp"
#include "vector.hpp"

namespace render {

  class Cylinder : public Figure {
  public:
    Cylinder();
    [[nodiscard]] std::string str() const override;
    render::vector axis;
    double height;

    bool validate_parameters(std::string & Parameters) override;

  protected:
    std::regex regex_pattern;
  };

}  // namespace render

#endif
