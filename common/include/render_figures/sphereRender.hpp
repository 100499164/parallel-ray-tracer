#ifndef SPHERE_RENDER_HPP
#define SPHERE_RENDER_HPP

#include "hittable.hpp"
#include "infocolision.hpp"
#include "materials_render/material_render.hpp"
#include "ray.hpp"
#include "vector.hpp"
#include <array>
#include <cmath>
#include <memory>

namespace render {

  class SphereRender : public Hittable {
  public:
    SphereRender(std::array<double, 3> const & center, double radius,
                 std::shared_ptr<Material_render> const & material)
        : center(center[0], center[1], center[2]), radius(radius), radius_squared(radius * radius),
          material(material) { }

    bool hit(Ray const & ray, double t_min, double t_max, InfoColision & record) const override {
      vector const r_c = center - ray.origen;

      double const a            = ray.direccion.dot2(ray.direccion);
      double const b            = -2.0 * ray.direccion.dot2(r_c);
      double const c            = r_c.dot2(r_c) - radius_squared;
      double const discriminant = b * b - 4 * a * c;

      if (discriminant < 0.0) {
        return false;
      }

      double const sqrt_disc = std::sqrt(discriminant);
      double const inv_2a    = 1.0 / (2.0 * a);

      double const lambda_1 = (-b - sqrt_disc) * inv_2a;
      double const lambda_2 = (-b + sqrt_disc) * inv_2a;
      double const t        = select_valid_root(lambda_1, lambda_2, t_min, t_max);

      if (t < 0.0) {
        return false;
      }

      fill_hit_record(ray, t, record);
      return true;
    }

    bool bounding_box(AABB & output_box) const override {
      output_box = AABB(center - render::vector(radius, radius, radius),
                        center + render::vector(radius, radius, radius));
      return true;
    }

  private:
    [[nodiscard]] static double select_valid_root(double lambda_1, double lambda_2, double t_min,
                                                  double t_max) {
      if (lambda_1 > t_min and lambda_1 < t_max) {
        return lambda_1;
      }
      if (lambda_2 > t_min and lambda_2 < t_max) {
        return lambda_2;
      }
      return -1.0;
    }

    void fill_hit_record(Ray const & ray, double t, InfoColision & record) const {
      record.t      = t;
      record.p      = ray.at(t);
      record.normal = (record.p - center) / radius;
      record.set_face_normal(ray, record.normal);
      record.mat_ptr = material.get();
    }

    vector center;
    double radius;
    double radius_squared;
    std::shared_ptr<Material_render> material;
  };

}  // namespace render

#endif
