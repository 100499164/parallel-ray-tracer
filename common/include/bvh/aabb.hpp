#ifndef AABB_HPP
#define AABB_HPP

#include "ray.hpp"
#include "vector.hpp"
#include <algorithm>

namespace render {

  class AABB {
  public:
    render::vector min;
    render::vector max;

    AABB() = default;

    AABB(render::vector const & a, render::vector const & b) {
      min = render::vector(fmin(a.x, b.x), fmin(a.y, b.y), fmin(a.z, b.z));
      max = render::vector(fmax(a.x, b.x), fmax(a.y, b.y), fmax(a.z, b.z));
    }

    // Método optimizado de intersección (Slab method)
    [[nodiscard]] bool hit(render::Ray const & r, double t_min, double t_max) const {
      for (int a = 0; a < 3; a++) {
        auto invD = 1.0F / r.direccion[a];
        auto t0   = (min[a] - r.origen[a]) * invD;
        auto t1   = (max[a] - r.origen[a]) * invD;
        if (invD < 0.0F) {
          std::swap(t0, t1);
        }
        t_min = t0 > t_min ? t0 : t_min;
        t_max = t1 < t_max ? t1 : t_max;
        if (t_max <= t_min) {
          return false;
        }
      }
      return true;
    }
  };

  // Función auxiliar para combinar dos cajas
  inline AABB surrounding_box(const AABB & box0, const AABB & box1) {
    const render::vector small(fmin(box0.min.x, box1.min.x), fmin(box0.min.y, box1.min.y),
                  fmin(box0.min.z, box1.min.z));
    const render::vector big(fmax(box0.max.x, box1.max.x), fmax(box0.max.y, box1.max.y),
                fmax(box0.max.z, box1.max.z));
    return {small, big};
  }

}  // namespace render

#endif
