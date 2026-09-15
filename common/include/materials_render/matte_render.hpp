#ifndef MATTE_RENDER_HPP
#define MATTE_RENDER_HPP

#include "materials_render/material_render.hpp"

namespace render {

  /**
   * @brief Material mate (difuso).
   *
   * Refleja los rayos en direcciones aleatorias, simulando una superficie rugosa.
   */
  class Matte_render : public Material_render {
  public:
    explicit Matte_render(Color const & albedo) noexcept;

    ~Matte_render() override                           = default;
    Matte_render(Matte_render const &)                 = default;
    Matte_render & operator=(Matte_render const &)     = default;
    Matte_render(Matte_render &&) noexcept             = default;
    Matte_render & operator=(Matte_render &&) noexcept = default;

    bool scatter(ScatterContext const & ctx, Color & atenuacion, Ray & r_out) const override;

  private:
    Color albedo_;  ///< Color base del material (reflectancia difusa)
  };

}  // namespace render

#endif  // MATTE_RENDER_HPP
