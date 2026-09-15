#include "materials/refractive.hpp"
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

namespace render {

  Refractive::Refractive() : regex_pattern(R"(^\s*(\w+)\s+([\d.]+)(?:\s+(.*))?\s*$)") { }

  bool Refractive::validate_parameters(std::string & Parameters) {
    // Create a match object to hold the results of the regex search
    std::smatch matches;

    // Check whether there are not enough
    if (!std::regex_match(Parameters, matches, this->regex_pattern)) {
      std::cerr << "ERROR: Invalid refractive material parameters\nLine: 'refractive: "
                << Parameters << "'\n";
      return false;
    }

    // Check that there are not too many parameters
    if (matches.size() > 3 and !matches[3].str().empty()) {
      std::cerr << "ERROR: Extra data after configuration value for key: [refractive:]\nExtra: '"
                << matches[3].str() << "'\nLine: 'refractive: " << Parameters << "'\n";
      return false;
    }

    // Extract the values
    material_alias   = matches[1].str();
    refraction_index = std::stod(matches[2].str());

    return true;
  }

  std::string Refractive::str() const {
    std::ostringstream oss;
    oss << "refractive: " << material_alias << " " << refraction_index;
    return oss.str();
  }

}  // namespace render
