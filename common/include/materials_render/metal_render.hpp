#ifndef METAL_RENDER_HPP
#define METAL_RENDER_HPP

#include "materials_render/material_render.hpp"

namespace render {

  // Calcula la reflexión del rayo en función del ángulo de incidencia y un factor de rugosidad
  // (`fuzz`).

  class Metal_render : public Material_render {
  public:
    Metal_render(Color const & albedo, double fuzz) noexcept;

    ~Metal_render() override                           = default;
    Metal_render(Metal_render const &)                 = default;
    Metal_render & operator=(Metal_render const &)     = default;
    Metal_render(Metal_render &&) noexcept             = default;
    Metal_render & operator=(Metal_render &&) noexcept = default;
    bool scatter(ScatterContext const & ctx, Color & atenuacion, Ray & r_out) const override;

  private:
    Color albedo_;  // Color base del material (reflectancia)
    double fuzz_;   // Factor de difusión
  };

}  // namespace render

#endif  // METAL_RENDER_HPP
