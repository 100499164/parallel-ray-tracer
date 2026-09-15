#ifndef INFO_COLISION_HPP
#define INFO_COLISION_HPP

#include "ray.hpp"     // Define la clase Rayo
#include "vector.hpp"  // Define render::vector

namespace render {

  class Material_render;

  /**
   * Contiene la información completa de una colisión entre un rayo y un objeto.
   * - Es rellenada por Hittable::hit()
   * - Es usada por Material_render::scatter()
   */
  struct InfoColision {
    double t{};                   ///< Distancia desde el origen del rayo al punto de impacto
    render::vector p;             ///< Punto de colisión en coordenadas del espacio
    render::vector normal;        ///< Normal en el punto de colisión (ajustada con set_face_normal)
    bool front_face{};            ///< Indica si el rayo golpea la cara exterior del objeto
    Material_render * mat_ptr{};  ///< Puntero al material del objeto golpeado

    void set_face_normal(render::Ray const & r, render::vector const & outward_normal) {
      front_face = render::vector::dot(r.direccion, outward_normal) < 0;
      normal     = front_face ? outward_normal : outward_normal * -1.0;
    }
  };

}  // namespace render

#endif  // INFO_COLISION_HPP
