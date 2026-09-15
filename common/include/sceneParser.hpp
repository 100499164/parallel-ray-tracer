#ifndef SCENE_PARSER_HPP
#define SCENE_PARSER_HPP

#include <istream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "figures/figure.hpp"
#include "materials/material.hpp"

namespace render {

  class sceneParser {
  public:
    sceneParser() = default;

    // Rule of Five
    ~sceneParser()                                   = default;
    sceneParser(sceneParser const &)                 = delete;
    sceneParser & operator=(sceneParser const &)     = delete;
    sceneParser(sceneParser &&) noexcept             = default;
    sceneParser & operator=(sceneParser &&) noexcept = default;

    // Define the attributes
    // Dictionary of Materials
    std::unordered_map<std::string, std::shared_ptr<Material>> materials;
    // Array of Figures
    std::vector<std::shared_ptr<Figure>> figures;

    // Define the methods
    [[nodiscard]] std::string str() const;

    bool parse(std::string const & scenario_path);
    bool parse(std::istream & stream);

  private:
    bool _parse_lines(std::istream & stream);

    bool _parse_lines();
    // Adds a material to the materials dictionary
    bool _create_material(std::string & material_name, std::string & parameters);
    // Adds a figure to the figures array
    bool _create_figure(std::string & figure_name, std::string & parameters);
    // Checks whether all the materials used by figures are declared
    bool _check_undeclared_materials();
    // Checks whether a material is duplicated
    bool _check_duplicated_materials(render::Material const & material) const;
  };

}  // namespace render

#endif
