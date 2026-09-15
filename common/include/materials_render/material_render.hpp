#ifndef MATERIAL_RENDER_HPP
#define MATERIAL_RENDER_HPP

#include "color.hpp"
#include "materials/material.hpp"
#include "scatter_context.hpp"

namespace render {

  // Cada material define cómo un rayo se dispersa (scatter) al impactar con una superficie.

  class Material_render {
  public:
    virtual ~Material_render()                               = default;
    Material_render()                                        = default;
    Material_render(Material_render const &)                 = default;
    Material_render & operator=(Material_render const &)     = default;
    Material_render(Material_render &&) noexcept             = default;
    Material_render & operator=(Material_render &&) noexcept = default;

    virtual bool scatter(ScatterContext const & ctx, Color & atenuacion, Ray & r_out) const = 0;

    static std::shared_ptr<Material_render> fromMaterial(
        std::shared_ptr<Material> const & material);
  };

}  // namespace render

#endif  // MATERIAL_RENDER_HPP
