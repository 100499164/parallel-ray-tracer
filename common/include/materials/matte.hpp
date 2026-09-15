#ifndef MATTE_HPP
#define MATTE_HPP

#include <string>
#include <array>

#include "material.hpp"

namespace render {
    class Matte : public Material
    {
        public:
            Matte();

            // Define the attributes
            std::array<double, 3> reflectance;

            // Define the methods
            [[nodiscard]] std::string str() const override;       
            bool validate_parameters(std::string & Parameters) override;

        protected:
            std::regex regex_pattern;
    };
}

#endif