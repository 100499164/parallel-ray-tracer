#include "materials_render/matte_render.hpp"
#include "color.hpp"  // Para render::Color
#include "ray.hpp"
#include "scatter_context.hpp"  // Para render::ScatterContext
#include "vector.hpp"           // Para operaciones con render::vector
#include <cmath>                // Para std::fabs

namespace render {

  // -----------------------------------------------------------------------------
  // Constructor
  // -----------------------------------------------------------------------------
  Matte_render::Matte_render(Color const & albedo) noexcept : albedo_(albedo) { }

  // -----------------------------------------------------------------------------
  // Método scatter()
  // -----------------------------------------------------------------------------
  bool Matte_render::scatter(ScatterContext const & ctx, Color & atenuacion, Ray & r_out) const {
    // Recuperar datos del contexto
    auto const & info = ctx.collision();

    // Calcular un vector de dirección aleatorio φ (entre -1 y 1)
    vector random_vec;
    while (true) {
      // Genera un vector entre [-1, 1]
      random_vec.x = ctx.next();
      random_vec.y = ctx.next();
      random_vec.z = ctx.next();

      // Comprueba si está dentro de la Esfera
      if (random_vec.squared_magnitude() < 1.0) {
        break;  // Aceptado
      }
      // Rechazado: el bucle se repite.
    }
    // Calcular la nueva dirección: normal + vector aleatorio
    vector scatter_dir = info.normal + random_vec.normalized();

    // Comprobar si el vector resultante es "demasiado pequeño"
    constexpr double EPS = 1e-8;
    if (std::fabs(scatter_dir.x) < EPS and
        std::fabs(scatter_dir.y) < EPS and
        std::fabs(scatter_dir.z) < EPS)
    {
      scatter_dir = info.normal;
    }

    // Definir el nuevo rayo reflejado
    r_out = Ray(info.p, scatter_dir.normalized());

    // La atenuación (reflectancia) es el color base del material
    atenuacion = albedo_;
    return true;
  }

}  // namespace render
