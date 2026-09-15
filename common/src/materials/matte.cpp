#include "materials/matte.hpp"
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

namespace render {

  Matte::Matte()
      : reflectance({0.0, 0.0, 0.0}),
        regex_pattern(R"(^\s*(\w+)\s+([\d.]+)\s+([\d.]+)\s+([\d.]+)(?:\s+(.*))?\s*$)") { }

  bool Matte::validate_parameters(std::string & Parameters) {
    // Create a match object to hold the results of the regex search
    std::smatch matches;

    // Check whether there are not enough
    if (!std::regex_match(Parameters, matches, this->regex_pattern)) {
      std::cerr << "ERROR: Invalid matte material parameters\nLine: 'matte: " << Parameters
                << "'\n";
      return false;
    }

    // Check that there are not too many parameters
    if (matches.size() > 5 and !matches[5].str().empty()) {
      std::cerr << "ERROR: Extra data after configuration value for key: [matte:]\nExtra: '"
                << matches[5].str() << "'\nLine: 'matte: " << Parameters << "'\n";
      return false;
    }

    // Extract the values
    material_alias = matches[1].str();
    reflectance[0] = std::stod(matches[2].str());
    reflectance[1] = std::stod(matches[3].str());
    reflectance[2] = std::stod(matches[4].str());

    // Check that the values are no
    if (reflectance[0] > 1 or reflectance[1] > 1 or reflectance[2] > 1) {
      std::cerr << "ERROR: Invalid matte material parameters\nReflective values must be in the "
                   "interval [0, 1]\nLine: 'matte: "
                << Parameters << "'\n";
      return false;
    }

    return true;
  }

  std::string Matte::str() const {
    std::ostringstream oss;
    oss << "matte: " << material_alias << " " << reflectance[0] << " " << reflectance[1] << " "
        << reflectance[2] << " ";
    return oss.str();
  }

}  // namespace render
