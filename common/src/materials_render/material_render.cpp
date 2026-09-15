#include "materials_render/material_render.hpp"

// Incluimos las clases base del parser
#include "materials/matte.hpp"
#include "materials/metal.hpp"
#include "materials/refractive.hpp"

// Incluimos las clases del renderizador
#include "color.hpp"
#include "materials/material.hpp"  // Para render::Material (Línea 19)
#include "materials_render/matte_render.hpp"
#include "materials_render/metal_render.hpp"
#include "materials_render/refractive_render.hpp"
#include <iostream>
#include <memory>

namespace render {

  std::shared_ptr<Material_render> Material_render::fromMaterial(
      std::shared_ptr<Material> const & material) {
    if (!material) {
      std::cerr << "[Material_render::fromMaterial] Error: material nulo\n";
      return nullptr;
    }

    // Matte
    if (auto matte = std::dynamic_pointer_cast<Matte>(material)) {
      Color const color(matte->reflectance[0], matte->reflectance[1], matte->reflectance[2]);
      return std::make_shared<Matte_render>(color);
    }

    // Metal
    if (auto metal = std::dynamic_pointer_cast<Metal>(material)) {
      Color const color(metal->reflectance[0], metal->reflectance[1], metal->reflectance[2]);
      return std::make_shared<Metal_render>(color, metal->diffusiveness);
    }

    // Refractive
    if (auto refr = std::dynamic_pointer_cast<Refractive>(material)) {
      return std::make_shared<Refractive_render>(refr->refraction_index);
    }

    std::cerr << "[Material_render::fromMaterial] Tipo de material desconocido: "
              << material->material_alias << "\n";
    return nullptr;
  }

}  // namespace render
