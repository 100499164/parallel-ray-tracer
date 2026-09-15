#ifndef SCATTER_CONTEXT_HPP
#define SCATTER_CONTEXT_HPP

#include "infocolision.hpp"
#include "ray.hpp"
#include <random>

namespace render {

  /**
   * @brief Contexto de dispersión (scatter) unificado.
   *
   * Contiene toda la información necesaria para calcular la reflexión o refracción
   * de un rayo en un material, evitando copias costosas.
   *
   * Nota: Se usan punteros no nulos en lugar de referencias para cumplir con clang-tidy.
   */
  struct ScatterContext {
    Ray const * r_in          = nullptr;  ///< Puntero al rayo incidente (no nulo)
    InfoColision const * info = nullptr;  ///< Puntero a la información de colisión (no nulo)
    std::mt19937_64 * rng     = nullptr;  ///< Puntero al generador aleatorio
    std::uniform_real_distribution<double> * dist =
        nullptr;  ///< Puntero a la distribución uniforme

    ScatterContext(Ray const * r_in_, InfoColision const * info_, std::mt19937_64 * rng_,
                   std::uniform_real_distribution<double> * dist_) noexcept
        : r_in(r_in_), info(info_), rng(rng_), dist(dist_) { }

    [[nodiscard]] double next() const noexcept { return (*dist)(*rng); }

    [[nodiscard]] Ray const & ray() const noexcept { return *r_in; }

    [[nodiscard]] InfoColision const & collision() const noexcept { return *info; }
  };

}  // namespace render

#endif  // SCATTER_CONTEXT_HPP
