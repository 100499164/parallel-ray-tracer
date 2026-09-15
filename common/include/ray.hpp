#ifndef RAYO_HPP
#define RAYO_HPP

#include "vector.hpp"

namespace render {

  /**
   * @brief Represents a ray in 3D space.
   *
   * Formula: P(t) = Origen + t * Dirección
   *
   */
  class Ray {
  public:
    render::vector origen;     // Origin point of the ray (O)
    render::vector direccion;  // Direction vector of the ray (D)

  public:
    Ray() = default;

    /**
     * @brief Main constructor for Ray.
     * @param o Origin point of the ray.
     * @param d Direction vector of the ray.
     */
    Ray(render::vector const & o, render::vector const & d) : origen(o), direccion(d) { }

    /**
     * @brief Calculates a point along the ray at parameter t.
     *
     * Implements forumla: P(t) = O + t*D
     *
     */
    [[nodiscard]] render::vector at(double t) const { return origen + direccion * t; }
  };

}  // namespace render

#endif  // RAYO_HPP
