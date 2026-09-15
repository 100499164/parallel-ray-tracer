#ifndef RENDER_CYLINDER_HPP
#define RENDER_CYLINDER_HPP

#include "hittable.hpp"
#include "infocolision.hpp"
#include "materials_render/material_render.hpp"
#include "ray.hpp"
#include "vector.hpp"
#include <cmath>

namespace render {

  // Constante para comprobaciones de punto flotante (paralelismo)
  inline constexpr double CYLINDER_EPSILON = 1e-8;

  struct CylinderContext {
    render::vector center{0.0, 0.0, 0.0};
    render::vector axis{0.0, 1.0, 0.0};
    double radius{1.0};
    double height{1.0};
  };

  class CylinderRender : public Hittable {
  public:
    CylinderRender(CylinderContext const & ctx, std::shared_ptr<Material_render> m)
        : C(ctx.center), a_hat(ctx.axis.normalized()), r(ctx.radius), h(ctx.height),
          mat_ptr(std::move(m)), h_over_2(h * 0.5), r_squared(r * r) {
      // Precalculamos los centros de las bases
      render::vector const h_vec = a_hat * h_over_2;
      p_base_sup                 = C + h_vec;
      p_base_inf                 = C - h_vec;
    }

    bool hit(render::Ray const & r_in, double t_min, double t_max,
             render::InfoColision & rec) const override {
      InfoColision temp_rec;
      double closest_so_far   = t_max;
      bool hit_curved_surface = false;

      // Comprobar la superficie curva.
      if (hit_curved(r_in, t_min, closest_so_far, temp_rec)) {
        hit_curved_surface = true;
        rec                = temp_rec;
      }

      // Comprobar las bases (tapas).
      if (hit_caps(r_in, t_min, closest_so_far, temp_rec)) {
        rec = temp_rec;
        return true;
      }

      return hit_curved_surface;
    }

    bool bounding_box(AABB & output_box) const override {
      // Usamos las variables miembro: C, a_hat, h, r
      // (No podemos usar 'ctx' aquí porque no existe en este ámbito)

      // 1. Calcular extremos de la línea central
      const render::vector half_h_vec = a_hat * (h * 0.5);
      const render::vector top        = C + half_h_vec;
      const render::vector bot        = C - half_h_vec;

      // 2. Calcular la extensión del radio proyectado en cada eje
      // Fórmula: radius * sqrt(1 - componente_eje^2)
      const double ext_x = r * std::sqrt(1.0 - a_hat.x * a_hat.x);
      const double ext_y = r * std::sqrt(1.0 - a_hat.y * a_hat.y);
      const double ext_z = r * std::sqrt(1.0 - a_hat.z * a_hat.z);

      const render::vector extension(ext_x, ext_y, ext_z);

      // 3. Encontramos los mínimos y máximos absolutos
      // Min = min(top, bot) - extension
      // Max = max(top, bot) + extension
      const render::vector p_min(std::fmin(top.x, bot.x) - extension.x,
                 std::fmin(top.y, bot.y) - extension.y,
                 std::fmin(top.z, bot.z) - extension.z);

      const render::vector p_max(std::fmax(top.x, bot.x) + extension.x,
                 std::fmax(top.y, bot.y) + extension.y,
                 std::fmax(top.z, bot.z) + extension.z);

      output_box = AABB(p_min, p_max);
      return true;
    }

  private:
    // Parametros del cilindro
    render::vector C;      // Centro
    render::vector a_hat;  // Eje unitario
    double r;              // Radio
    double h;              // Altura
    std::shared_ptr<Material_render> mat_ptr;

    // Valores precalculados para ser más eficiente
    double h_over_2;            // h / 2
    double r_squared;           // r * r (para comprobación de bases)
    render::vector p_base_sup;  // Centro de la base superior
    render::vector p_base_inf;  // Centro de la base inferior

    bool check_root_validity(render::Ray const & r_in, double t_root, double & t_max,
                             render::InfoColision & rec) const {
      if (t_root >= t_max) {
        return false;
      }

      render::vector const rc = r_in.origen - C;
      double const dot_rc_a   = render::vector::dot(rc, a_hat);
      double const dot_dr_a   = render::vector::dot(r_in.direccion, a_hat);
      double const dist_eje   = dot_rc_a + dot_dr_a * t_root;

      if (std::abs(dist_eje) > h_over_2) {
        return false;
      }

      t_max = t_root;
      rec.t = t_root;
      rec.p = r_in.at(t_root);

      render::vector const outward_normal = (rec.p - C) - a_hat * dist_eje;
      rec.set_face_normal(r_in, outward_normal);
      rec.mat_ptr = mat_ptr.get();

      return true;
    }

    bool hit_curved(render::Ray const & r_in, double t_min, double & t_max,
                    render::InfoColision & rec) const {
      render::vector const rc = r_in.origen - C;

      double const dot_rc_a = render::vector::dot(rc, a_hat);
      double const dot_dr_a = render::vector::dot(r_in.direccion, a_hat);

      render::vector const rc_perp = rc - a_hat * dot_rc_a;
      render::vector const dr_perp = r_in.direccion - a_hat * dot_dr_a;

      double const A = dr_perp.squared_magnitude();
      if (std::abs(A) < CYLINDER_EPSILON) {
        return false;
      }

      double const B            = 2.0 * render::vector::dot(rc_perp, dr_perp);
      double const C_q          = rc_perp.squared_magnitude() - r_squared;
      double const discriminant = B * B - 4 * A * C_q;

      if (discriminant < 0.0) {
        return false;
      }

      double const sqrt_d = std::sqrt(discriminant);
      double const inv_2A = 1.0 / (2.0 * A);

      bool found_hit = false;
      double t_root  = (-B - sqrt_d) * inv_2A;
      if (t_root > t_min and check_root_validity(r_in, t_root, t_max, rec)) {
        found_hit = true;
      }

      t_root = (-B + sqrt_d) * inv_2A;
      if (t_root > t_min and check_root_validity(r_in, t_root, t_max, rec)) {
        found_hit = true;
      }

      return found_hit;
    }

    bool hit_caps(render::Ray const & r_in, double t_min, double & t_max,
                  render::InfoColision & rec) const {
      bool hit_anything = false;

      // Iterar: 0 = base superior, 1 = base inferior
      for (int i = 0; i < 2; ++i) {
        render::vector const P  = (i == 0) ? p_base_sup : p_base_inf;
        render::vector const pn = (i == 0) ? a_hat : (a_hat * -1.0);

        double const denominator = render::vector::dot(r_in.direccion, pn);

        // Volvemos si el rayo es paralelo al plano de la base
        if (std::abs(denominator) < CYLINDER_EPSILON) {
          continue;
        }

        // t = (P - Or) · ⃗pn / (⃗rd · ⃗pn)
        double const t = render::vector::dot(P - r_in.origen, pn) / denominator;

        // Comprobar rango (t_max es el más cercano hasta ahora)
        if (t < t_min or t > t_max) {
          continue;
        }

        render::vector const I = r_in.at(t);

        // Comprobar si el impacto está DENTRO del radio
        if ((I - P).squared_magnitude() > r_squared) {
          continue;
        }

        // Golpe válido en la base
        hit_anything = true;
        t_max        = t;
        rec.t        = t;
        rec.p        = I;
        rec.set_face_normal(r_in, pn);
        rec.mat_ptr = mat_ptr.get();
      }
      return hit_anything;
    }
  };

}  // namespace render

#endif
