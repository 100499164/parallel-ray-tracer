#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "figure.hpp"

namespace render {

    class Sphere : public Figure {
        public:
            Sphere();
            [[nodiscard]] std::string str() const override;
            bool validate_parameters(std::string & Parameters) override;

        protected:
            std::regex regex_pattern;
    };
}

#endif