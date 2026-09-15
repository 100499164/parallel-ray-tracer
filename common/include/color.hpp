#ifndef COLOR_HPP
#define COLOR_HPP

#include <algorithm>
#include <string>

namespace render {

  // Structure for an RGB pixel
  struct Color {
    double r, g, b;

    constexpr Color(double r_, double g_, double b_) : r(r_), g(g_), b(b_) { }

    // Method to get a string representation of the Color
    [[nodiscard]] std::string str() const;

    // Default constructor
    Color() = default;

    // --- Linear interpolation between two colors ---
    static Color lerp(Color const & c1, Color const & c2, double t) {
      t = std::clamp(t, 0.0, 1.0);

      auto mix = [t](double a, double b) -> double {
        double const value = (1.0 - t) * a + t * b;
        return std::clamp(value, 0.0, 1.0);
      };

      return Color{mix(c1.r, c2.r), mix(c1.g, c2.g), mix(c1.b, c2.b)};
    }

    // Compound assignment operators
    Color & operator+=(Color const & rhs) {
      r += rhs.r;
      g += rhs.g;
      b += rhs.b;
      return *this;
    }

    // Component-wise multiplication
    Color & operator*=(Color const & rhs) {
      r *= rhs.r;
      g *= rhs.g;
      b *= rhs.b;
      return *this;
    }

    // Scalar multiplication
    Color & operator*=(double scalar) {
      r *= scalar;
      g *= scalar;
      b *= scalar;
      return *this;
    }

    Color & operator/=(double scalar) { return *this *= (1.0 / scalar); }
  };

  // Color addition
  inline Color operator+(Color const & lhs, Color const & rhs) {
    return {lhs.r + rhs.r, lhs.g + rhs.g, lhs.b + rhs.b};
  }

  // Scalar multiplication (double * Color)
  inline Color operator*(double s, Color const & c) {
    return {c.r * s, c.g * s, c.b * s};
  }

  // Scalar multiplication (Color * double)
  inline Color operator*(Color const & c, double s) {
    return s * c;  // Llama al operador anterior
  }

  // Component-wise multiplication (Color * Color)
  inline Color operator*(Color const & c, Color const & d) {
    return {c.r * d.r, c.g * d.g, c.b * d.b};
  }

  // Equality operator for Color
  inline bool operator==(Color const & lhs, Color const & rhs) {
    return (lhs.r == rhs.r) and (lhs.g == rhs.g) and (lhs.b == rhs.b);
  }

}  // namespace render

#endif  // COLOR_HPP
