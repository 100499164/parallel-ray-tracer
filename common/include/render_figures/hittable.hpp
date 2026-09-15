#ifndef HITTABLE_HPP
#define HITTABLE_HPP

#include "bvh/aabb.hpp"
#include "infocolision.hpp"
#include "ray.hpp"

namespace render {

  class Hittable {
  public:
    Hittable() = default;

    // Rule of Five
    virtual ~Hittable();
    Hittable(Hittable const &)                 = delete;
    Hittable & operator=(Hittable const &)     = delete;
    Hittable(Hittable &&) noexcept             = default;
    Hittable & operator=(Hittable &&) noexcept = default;

    virtual bool hit(Ray const & ray, double t_min, double t_max, InfoColision & record) const = 0;

    virtual bool bounding_box(AABB & output_box) const = 0;
  };

}  // namespace render

#endif
