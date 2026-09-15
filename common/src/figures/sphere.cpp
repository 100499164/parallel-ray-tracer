#include <iostream>
#include <regex>
#include <sstream>
#include <string>

#include "figures/sphere.hpp"

namespace render {

  Sphere::Sphere()
      : regex_pattern(std::regex(
            R"(^\s*([-\d.]+)\s+([-\d.]+)\s+([-\d.]+)\s+([\d.]+)\s+(\w+)(?:\s+(.*))?\s*$)")) { }

  bool Sphere::validate_parameters(std::string & Parameters) {
    // Create a match object to hold the results of the regex search
    std::smatch matches;

    // Check whether there are not enough
    if (!std::regex_match(Parameters, matches, this->regex_pattern)) {
      std::cerr << "ERROR: Invalid sphere parameters\nLine: 'sphere: " << Parameters << "'\n";
      return false;
    }

    // Check that there are not too many parameters
    if (matches.size() > 6 and !matches[6].str().empty()) {
      std::cerr << "ERROR: Extra data after configuration value for key: [sphere:]\nExtra: '"
                << matches[6].str() << "'\nLine: 'sphere: " << Parameters << "'\n";
      return false;
    }

    position[0]   = std::stod(matches[1].str());
    position[1]   = std::stod(matches[2].str());
    position[2]   = std::stod(matches[3].str());
    radius        = std::stod(matches[4].str());
    material_name = matches[5].str();

    return true;
  }

  std::string Sphere::str() const {
    std::ostringstream oss;
    oss << "sphere: " << position[0] << " " << position[1] << " " << position[2] << " " << radius
        << " " << material_name;
    return oss.str();
  }

}  // namespace render
