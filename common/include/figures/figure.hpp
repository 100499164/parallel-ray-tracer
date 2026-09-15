#ifndef FIGURE_HPP
#define FIGURE_HPP

#include <array>
#include <regex>
#include <string>

namespace render {

  class Figure {
  public:
    Figure();

    // Rule of five
    virtual ~Figure()                  = default;
    Figure(Figure const &)             = default;
    Figure & operator=(Figure const &) = default;
    Figure(Figure &&)                  = default;
    Figure & operator=(Figure &&)      = default;

    // Define the attributes
    std::array<double, 3> position;
    double radius = 1.0;
    // Alias of the material (the parser is responsible for checking it)
    std::string material_name;

    // Define the methods
    [[nodiscard]] virtual std::string str() const;
    virtual bool validate_parameters(std::string & Parameters);

  protected:
    std::regex regex_pattern;
  };

}  // namespace render

#endif
