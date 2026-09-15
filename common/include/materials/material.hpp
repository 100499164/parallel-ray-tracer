#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <string>
#include <regex>

namespace render {
    class Material {
    public:
        Material();

        // Rule of Five
        virtual ~Material() = default;
        Material(const Material&) = default;
        Material& operator=(const Material&) = default;
        Material(Material&&) = default;
        Material& operator=(Material&&) = default;
        
        // Define the methods
        [[nodiscard]] virtual std::string str() const = 0;
        virtual bool validate_parameters(std::string & Parameters) = 0;
        std::string material_alias;

    protected:
        std::regex regex_pattern;        
    };
}

#endif
