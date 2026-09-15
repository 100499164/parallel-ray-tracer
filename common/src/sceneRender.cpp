#include "sceneRender.hpp"
#include "bvh/aabb.hpp"
#include "bvh/bvh_node.hpp"
#include "figures/cylinder.hpp"
#include "figures/figure.hpp"
#include "figures/sphere.hpp"
#include "infocolision.hpp"
#include "materials/material.hpp"
#include "materials_render/material_render.hpp"
#include "ray.hpp"
#include "render_figures/cylinderRender.hpp"
#include "render_figures/sphereRender.hpp"
#include "vector.hpp"
#include <cstddef>
#include <iostream>
#include <memory>
#include <utility>

namespace render {

  void SceneRender::add_render_figure(std::shared_ptr<Figure> const & figure,
                                      std::shared_ptr<Material> const & material) {
    auto mat_render = Material_render::fromMaterial(material);
    if (!mat_render) {
      std::cerr << "Error: No se pudo convertir material '"
                << (material ? material->material_alias : "(null)") << "' a Material_render\n";
      return;
    }

    // Verificar si la figura es una Sphere
    if (auto sphere = std::dynamic_pointer_cast<Sphere>(figure)) {
      // Crear SphereRender mapeando los atributos de Sphere
      auto sphere_render = std::make_shared<SphereRender>(sphere->position,  // center
                                                          sphere->radius,    // radius
                                                          mat_render         // material
      );
      render_figures.push_back(std::move(sphere_render));

    } else if (auto cylinder = std::dynamic_pointer_cast<Cylinder>(figure)) {
      // Crear CylinderRender mapeando los atributos de Cylinder
      render::vector const position(cylinder->position[0], cylinder->position[1],
                                    cylinder->position[2]);
      CylinderContext const ctx{position, cylinder->axis, cylinder->radius, cylinder->height};
      auto cylinder_render = std::make_shared<CylinderRender>(ctx, mat_render);
      render_figures.push_back(std::move(cylinder_render));
    } else {
      std::cout << "Unsupported figure type" << "\n";
    }
  }

  void SceneRender::build_bvh() {
    if (render_figures.empty()) {
      return;
    }

    std::cout << "Construyendo BVH con " << render_figures.size() << " objetos...\n";

    bvh_root = std::make_shared<BvhNode>(render_figures, 0, render_figures.size());

    std::cout << "BVH Construido.\n";
  }

  bool SceneRender::hit(Ray const & ray, double t_min, double t_max, InfoColision & record) const {
    // Si el BVH está construido, úsalo
    if (bvh_root) {
      return bvh_root->hit(ray, t_min, t_max, record);
    }
    // En otro caso, busca de forma iterativa
    bool hit_anything     = false;
    double closest_so_far = t_max;

    for (auto const & object : render_figures) {
      if (object->hit(ray, t_min, closest_so_far, record)) {
        hit_anything   = true;
        closest_so_far = record.t;
      }
    }
    return hit_anything;
  }

  void SceneRender::clear() {
    render_figures.clear();
    bvh_root.reset();
  }

  size_t SceneRender::size() const {
    return render_figures.size();
  }

  bool SceneRender::bounding_box(AABB & output_box) const {
    if (render_figures.empty() and !bvh_root) {
      return false;
    }

    // Si ya está el BVH construido
    if (bvh_root) {
      return bvh_root->bounding_box(output_box);
    }

    // Si no lo está
    AABB temp_box;
    bool first_box = true;

    for (auto const & object : render_figures) {
      if (!object->bounding_box(temp_box)) {
        return false;
      }
      output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
      first_box  = false;
    }

    return true;
  }

}  // namespace render
