#include "figures/cylinder.hpp"

#include <iostream>
#include <regex>  // Para std::regex, std::smatch, std::regex_match
#include <sstream>
#include <string>  // Para std::string y std::stod

namespace render {

  Cylinder::Cylinder()
      : axis({0.0, 0.0, 10.0}), height(axis.magnitude()),
        regex_pattern(std::regex(
            R"(^\s*([-\d.]+)\s+([-\d.]+)\s+([-\d.]+)\s+([\d.]+)\s+([-\d.]+)\s+([-\d.]+)\s+([-\d.]+)\s+(\w+)(?:\s+(.*))?\s*$)")) {
  }

  bool Cylinder::validate_parameters(std::string & Parameters) {
    // Create a match object to hold the results of the regex search
    std::smatch matches;

    // Check whether there are not enough
    if (!std::regex_match(Parameters, matches, this->regex_pattern)) {
      std::cerr << "ERROR: Invalid cylinder parameters\nLine: 'cylinder: " << Parameters << "'\n";
      return false;
    }

    // Check that there are not too many parameters
    if (matches.size() > 9 and !matches[9].str().empty()) {
      std::cerr << "ERROR: Extra data after configuration value for key: [cylinder:]\nExtra: '"
                << matches[9].str() << "'\nLine: 'cylinder: " << Parameters << "'\n";
      return false;
    }

    position[0] = std::stod(matches[1].str());
    position[1] = std::stod(matches[2].str());
    position[2] = std::stod(matches[3].str());
    radius      = std::stod(matches[4].str());
    axis.set_x(std::stod(matches[5].str()));
    axis.set_y(std::stod(matches[6].str()));
    axis.set_z(std::stod(matches[7].str()));
    height        = axis.magnitude();
    material_name = matches[8].str();

    return true;
  }

  std::string Cylinder::str() const {
    std::ostringstream oss;
    oss << "cylinder: " << position[0] << " " << position[1] << " " << position[2] << " " << radius
        << " " << axis.get_x() << " " << axis.get_y() << " " << axis.get_z() << " "
        << material_name;
    return oss.str();
  }

}  // namespace render
