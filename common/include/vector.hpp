#ifndef RENDER_VECTOR_HPP
#define RENDER_VECTOR_HPP

#include <cmath>    // sqrt, fabs
#include <ostream>  // std::ostream

namespace render {

  class vector {
  public:
    double x, y, z;

    // Constructores
    constexpr vector() : x{0.0}, y{0.0}, z{0.0} { }

    constexpr vector(double cx, double cy, double cz) : x{cx}, y{cy}, z{cz} { }

    // Getters
    [[nodiscard]] constexpr double get_x() const noexcept { return x; }

    [[nodiscard]] constexpr double get_y() const noexcept { return y; }

    [[nodiscard]] constexpr double get_z() const noexcept { return z; }

    // Setters
    constexpr void set_x(double val) noexcept { x = val; }

    constexpr void set_y(double val) noexcept { y = val; }

    constexpr void set_z(double val) noexcept { z = val; }

    // Magnitudes
    [[nodiscard]] double magnitude() const noexcept {
      // usa squared_magnitude() para evitar repetir cálculos
      return std::sqrt(squared_magnitude());
    }

    [[nodiscard]] constexpr double squared_magnitude() const noexcept {
      // constexpr y noexcept: habilita plegado en tiempo de compilación cuando sea posible
      return x * x + y * y + z * z;
    }

    [[nodiscard]] vector normalized() const noexcept {
      // camino rápido y seguro para norma cero
      double const m2 = squared_magnitude();  // una sola pasada
      if (m2 == 0.0) {
        return {0.0, 0.0, 0.0};
      }
      double const inv = 1.0 / std::sqrt(m2);  // multiplicar por inverso (1 div en vez de 3)
      return {x * inv, y * inv, z * inv};
    }

    // Operadores aritméticos
    [[nodiscard]] constexpr vector operator+(vector const & other) const noexcept {
      return {x + other.x, y + other.y, z + other.z};
    }

    [[nodiscard]] constexpr vector operator-(vector const & other) const noexcept {
      return {x - other.x, y - other.y, z - other.z};
    }

    [[nodiscard]] constexpr vector operator*(double scalar) const noexcept {
      return {x * scalar, y * scalar, z * scalar};
    }

    [[nodiscard]] vector operator/(double scalar) const noexcept {
      // multiplicar por el inverso: misma semántica, menos divisiones
      double const inv = 1.0 / scalar;
      return {x * inv, y * inv, z * inv};
    }

    // Operadores compuestos
    vector & operator+=(vector const & other) noexcept {
      x += other.x;
      y += other.y;
      z += other.z;
      return *this;
    }

    vector & operator-=(vector const & other) noexcept {
      x -= other.x;
      y -= other.y;
      z -= other.z;
      return *this;
    }

    vector & operator*=(double scalar) noexcept {
      x *= scalar;
      y *= scalar;
      z *= scalar;
      return *this;
    }

    double operator[](int i) const {
      if (i == 0) {
        return x;
      }
      if (i == 1) {
        return y;
      }
      return z;
    }

    // Productos
    [[nodiscard]] constexpr double dot2(vector const & other) const {
      return x * other.x + y * other.y + z * other.z;
    }

    static constexpr double dot(vector const & a, vector const & b) noexcept {
      return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    static constexpr vector cross(vector const & a, vector const & b) noexcept {
      return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
    }

    // Comparación (se mantiene igualdad exacta para no romper tests)
    [[nodiscard]] constexpr bool operator==(vector const & other) const noexcept {
      return x == other.x and y == other.y and z == other.z;
    }

    [[nodiscard]] constexpr bool operator!=(vector const & other) const noexcept {
      return !(*this == other);
    }

    // Salida
    friend std::ostream & operator<<(std::ostream & os, vector const & v) {
      os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
      return os;
    }
  };

}  // namespace render

#endif  // RENDER_VECTOR_HPP
