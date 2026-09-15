#include "sceneParser.hpp"
#include "figures/cylinder.hpp"
#include "figures/sphere.hpp"
#include "materials/material.hpp"
#include "materials/matte.hpp"
#include "materials/metal.hpp"
#include "materials/refractive.hpp"
#include <cstddef>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

namespace render {

  bool sceneParser::parse(std::string const & scenario_path) {
    std::ifstream file(scenario_path);
    if (!file.is_open()) {
      std::cerr << "ERROR: Failed to open file: " << scenario_path << "\n";
      return false;
    }
    // Llama al método de parseo principal pasándole el stream del fichero
    return parse(file);
  }

  bool sceneParser::parse(std::istream & stream) {
    // Limpiamos el estado anterior por si se reutiliza el objeto parser
    this->materials.clear();
    this->figures.clear();

    if (!this->_parse_lines(stream)) {
      return false;
    }

    if (!this->_check_undeclared_materials()) {
      return false;
    }

    // std::cout << this->str() << "\n";
    return true;
  }

  // CAMBIO: _parse_lines ahora recibe el stream como argumento
  bool sceneParser::_parse_lines(std::istream & stream) {
    std::string line;
    // Lee del stream que se le ha pasado, no de un miembro de la clase
    while (std::getline(stream, line)) {
      // Ignorar líneas vacías o que solo contienen espacios en blanco
      if (line.empty() or line.find_first_not_of(" \t\n\r") == std::string::npos) {
        continue;
      }

      size_t const colon_pos = line.find(':');
      if (colon_pos == std::string::npos) {
        std::cerr << "ERROR: Invalid line format (missing ':'): " << line << "\n";
        return false;
      }

      std::string entity     = line.substr(0, colon_pos);
      std::string parameters = line.substr(colon_pos + 1);

      if (entity == "matte" or entity == "metal" or entity == "refractive") {
        if (!this->_create_material(entity, parameters)) {
          return false;
        }
      } else if (entity == "sphere" or entity == "cylinder") {
        if (!this->_create_figure(entity, parameters)) {
          return false;
        }
      } else {
        std::cerr << "ERROR: Unknown scene entity: " << entity << "\nLine: '" << line << "'\n";
        return false;
      }
    }
    return true;
  }

  bool sceneParser::_create_material(std::string & material_name, std::string & parameters) {
    // Instantiate the material and add it to the corresponding dictionary
    if (material_name == "matte") {
      // Create matte material
      auto matte = std::make_shared<render::Matte>();
      if (matte->validate_parameters(parameters) and this->_check_duplicated_materials(*matte)) {
        // Add to dictionary
        this->materials[matte->material_alias] = matte;
      } else {
        return false;
      }
    } else if (material_name == "metal") {
      // Create metal material
      auto metal = std::make_shared<render::Metal>();
      if (metal->validate_parameters(parameters) and this->_check_duplicated_materials(*metal)) {
        // Add to dictionary
        this->materials[metal->material_alias] = metal;
      } else {
        return false;
      }
    } else {
      // Create refractive material
      auto refractive = std::make_shared<render::Refractive>();
      if (refractive->validate_parameters(parameters) and
          this->_check_duplicated_materials(*refractive))
      {
        // Add to dictionary
        this->materials[refractive->material_alias] = refractive;
      } else {
        return false;
      }
    }
    return true;
  }

  bool sceneParser::_create_figure(std::string & figure_name, std::string & parameters) {
    // Instantiate the figure and add it to the corresponding dynamic vector
    // std::cout << parameters << '\n';
    if (figure_name == "sphere") {
      // Create sphere figure
      auto sphere = std::make_shared<render::Sphere>();
      if (sphere->validate_parameters(parameters)) {
        // Add to dynamic vector
        this->figures.push_back(sphere);
      } else {
        return false;
      }
    } else {
      // Create cylinder figure
      auto cylinder = std::make_shared<render::Cylinder>();
      if (cylinder->validate_parameters(parameters)) {
        // Add to dynamic vector
        this->figures.push_back(cylinder);
      } else {
        return false;
      }
    }
    return true;
  }

  bool sceneParser::_check_undeclared_materials() {
    for (auto const & figure : this->figures) {
      // Check whether the material used by the figure is any key of materials dictionary
      if (!this->materials.contains(figure->material_name)) {
        std::cerr << "ERROR: Material not found: [" << figure->material_name << "]\nLine: '"
                  << figure->str() << "'\n";
        return false;
      }
    }
    return true;
  }

  bool sceneParser::_check_duplicated_materials(render::Material const & material) const {
    if (this->materials.contains(material.material_alias)) {
      std::cerr << "ERROR: Material with name [" << material.material_alias
                << "] already exists\nLine: '" << material.str() << "'\n";
      return false;
    }
    return true;
  }

  std::string sceneParser::str() const {
    std::ostringstream oss;
    oss << "Scene Parser State:\n\n";

    oss << "Materials:\n";
    for (auto const & [type, material] : this->materials) {
      oss << "  Material Key: " << type << "\n";
      oss << "    " << material->str() << "\n";
    }

    oss << "Figures:\n";
    for (auto const & figure : this->figures) {
      oss << "  " << figure->str() << "\n";
    }

    return oss.str();
  }

}  // namespace render
