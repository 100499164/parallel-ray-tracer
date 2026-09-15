#include "materials_render/metal_render.hpp"
#include "color.hpp"  // Para render::Color
#include "ray.hpp"
#include "scatter_context.hpp"  // Para render::ScatterContext
#include "vector.hpp"           // Para operaciones vectoriales
#include <cmath>                // Para std::fabs

namespace render {

  // -----------------------------------------------------------------------------
  // Constructor
  // -----------------------------------------------------------------------------
  Metal_render::Metal_render(Color const & albedo, double fuzz) noexcept
      : albedo_(albedo), fuzz_(std::abs(fuzz)) { }

  // -----------------------------------------------------------------------------
  // Método scatter()
  // -----------------------------------------------------------------------------
  bool Metal_render::scatter(ScatterContext const & ctx, Color & atenuacion, Ray & r_out) const {
    // 1. Recuperar datos del contexto
    auto const & r_in = ctx.ray();
    auto const & info = ctx.collision();

    // Calcular la dirección reflejada ideal
    vector const reflected =
        r_in.direccion - info.normal * 2.0 * render::vector::dot(r_in.direccion, info.normal);

    // Generar el vector aleatorio φ (Método de la Esfera)
    vector random_vec;
    while (true) {
      // Genera un vector entre [-1, 1]
      random_vec.x = ctx.next();
      random_vec.y = ctx.next();
      random_vec.z = ctx.next();

      // Comprueba si está dentro de la Esfera
      if (random_vec.squared_magnitude() < 1.0) {
        break;
      }
      // Rechazado: el bucle se repite.
    }

    // Ahora usa el vector aceptado para el fuzz
    vector const fuzz_vec = random_vec.normalized() * fuzz_;
    // Calcular la dirección de reflexión final
    vector const reflected_dir = (reflected.normalized() + fuzz_vec);
    // Crear el nuevo rayo
    r_out = Ray(info.p, reflected_dir);

    // La atenuación
    atenuacion = albedo_;

    return true;
  }

}  // namespace render
