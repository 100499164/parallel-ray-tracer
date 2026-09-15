#include "figures/figure.hpp"
#include <sstream>
#include <string>

namespace render {

  Figure::Figure() : position({30.0, 0.0, 0.0}), radius(5.0) { }

  std::string Figure::str() const {
    std::ostringstream oss;
    oss << "Figure(position=[" << position[0] << ", " << position[1] << ", " << position[2]
        << "], radius=" << radius << ", material_name=" << material_name << ")";
    return oss.str();
  }

  bool Figure::validate_parameters(std::string & Parameters) {
    (void) Parameters;
    return false;
  }

}  // namespace render
