#ifndef SCENE_RENDER_HPP
#define SCENE_RENDER_HPP

#include "bvh/bvh_node.hpp"
#include "figures/figure.hpp"
#include "materials/material.hpp"
#include "render_figures/cylinderRender.hpp"
#include "render_figures/hittable.hpp"
#include "render_figures/sphereRender.hpp"
#include <cstddef>
#include <memory>
#include <vector>

namespace render {

  class SceneRender : public Hittable {
  public:
    // Constructor por defecto
    SceneRender() = default;

    // Rule of Five (heredamos de clase polimórfica)
    ~SceneRender() override          = default;
    SceneRender(SceneRender const &) = delete;  // No permitir copia (contenedor de recursos)
    SceneRender & operator=(SceneRender const &)     = delete;
    SceneRender(SceneRender &&) noexcept             = default;  // Permitir movimiento
    SceneRender & operator=(SceneRender &&) noexcept = default;

    // Método para añadir figuras a la escena
    // Convierte Figure (Sphere, Cylinder) a su versión renderizable (SphereRender, CylinderRender)
    void add_render_figure(std::shared_ptr<Figure> const & figure,
                           std::shared_ptr<Material> const & material);

    // Implementación del método virtual puro hit() heredado de Hittable
    // Itera sobre todas las figuras y encuentra la intersección más cercana
    bool hit(Ray const & ray, double t_min, double t_max, InfoColision & record) const override;

    // Método para limpiar todas las figuras de la escena
    void clear();

    // Método para obtener el número de figuras en la escena
    [[nodiscard]] size_t size() const;

    bool bounding_box(AABB & output_box) const override;

    void build_bvh();

  private:
    // Usar shared_ptr en lugar de punteros raw para gestión automática de memoria
    std::vector<std::unique_ptr<SphereRender>> render_spheres;
    std::vector<std::unique_ptr<CylinderRender>> render_cylinders;
    std::vector<std::shared_ptr<Hittable>> render_figures;
    std::shared_ptr<BvhNode> bvh_root;
  };

}  // namespace render

#endif
