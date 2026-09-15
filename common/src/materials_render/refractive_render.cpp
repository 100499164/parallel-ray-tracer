#include "materials_render/refractive_render.hpp"
#include "color.hpp"  // Para render::Color
#include "ray.hpp"
#include "scatter_context.hpp"  // Para render::ScatterContext
#include "vector.hpp"
#include <cmath>  // Para std::fabs, std::sqrt

namespace render {
  namespace {

    vector reflect(vector const & v, vector const & n) noexcept {
      return v - n * 2.0 * render::vector::dot(v, n);
    }

    vector refract(vector const & uv, vector const & n, double etai_over_etat) noexcept {
      // Calcular cos_theta
      double const cos_theta = std::fmin(render::vector::dot(uv * -1, n), 1.0);

      // Calcular r_out_perp
      vector const r_out_perp = (uv + n * cos_theta) * etai_over_etat;

      double const sin_theta_sq = 1.0 - cos_theta * cos_theta;
      double const cos_prime_sq = 1.0 - etai_over_etat * etai_over_etat * sin_theta_sq;

      // Calcular r_out_parallel (ahora usa el valor rápido)
      vector const r_out_parallel = n * -std::sqrt(std::fabs(cos_prime_sq));

      return r_out_perp + r_out_parallel;
    }

  }  // namespace

  // -----------------------------------------------------------------------------
  // Constructor
  // -----------------------------------------------------------------------------
  Refractive_render::Refractive_render(double index_of_refraction) noexcept
      : ir_(index_of_refraction) { }

  // -----------------------------------------------------------------------------
  // Método scatter()
  // -----------------------------------------------------------------------------
  bool Refractive_render::scatter(ScatterContext const & ctx, Color & atenuacion,
                                  Ray & r_out) const {
    // Recuperar datos del contexto
    auto const & r_in = ctx.ray();
    auto const & info = ctx.collision();

    atenuacion = Color(1.0, 1.0, 1.0);  // Los materiales refractivos no atenúan el color

    // Determinar índice relativo (de dentro a fuera o viceversa)
    double const refraction_ratio = info.front_face ? (1.0 / ir_) : ir_;

    vector const unit_dir = r_in.direccion.normalized();

    // Calcular el coseno del ángulo de incidencia
    double const cos_theta = std::fmin(render::vector::dot(unit_dir * -1, info.normal), 1.0);
    double const sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

    // Comprobar si hay reflexión total interna
    bool const cannot_refract = refraction_ratio * sin_theta > 1.0;

    vector direction;
    if (cannot_refract) {
      // Reflexión total
      direction = reflect(unit_dir, info.normal);
    } else {
      // Aplicar ley de Snell
      direction = refract(unit_dir, info.normal, refraction_ratio);
    }

    // Crear nuevo rayo refractado/reflejado
    r_out = Ray(info.p, direction);

    return true;
  }

}  // namespace render
