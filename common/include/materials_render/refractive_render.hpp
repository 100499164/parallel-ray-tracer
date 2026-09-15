#ifndef REFRACTIVE_RENDER_HPP
#define REFRACTIVE_RENDER_HPP

#include "materials_render/material_render.hpp"

namespace render {

  //Aplica la ley de Snell para calcular el rayo refractado y determinar si ocurre reflexión total interna.
  class Refractive_render : public Material_render {
  public:
    explicit Refractive_render(double index_of_refraction) noexcept;

    ~Refractive_render() override                                = default;
    Refractive_render(Refractive_render const &)                 = default;
    Refractive_render & operator=(Refractive_render const &)     = default;
    Refractive_render(Refractive_render &&) noexcept             = default;
    Refractive_render & operator=(Refractive_render &&) noexcept = default;

    bool scatter(ScatterContext const & ctx, Color & atenuacion, Ray & r_out) const override;

  private:
    double ir_;  ///< Índice de refracción del material (n)
  };

}  // namespace render

#endif  // REFRACTIVE_RENDER_HPP
