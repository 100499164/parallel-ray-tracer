#include "color.hpp"

#include <cmath>  // std::lround (por consistencia)
#include <sstream>
#include <string>

namespace render {

  // String representation
  std::string Color::str() const {
    std::ostringstream oss;
    oss << "Color(" << r << ", " << g << ", " << b << ")";
    return oss.str();
  }

}  // namespace render
