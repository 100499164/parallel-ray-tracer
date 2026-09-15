#include "bvh/bvh_node.hpp"
#include "bvh/aabb.hpp"
#include "infocolision.hpp"
#include "ray.hpp"
#include "render_figures/hittable.hpp"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

// --- Funciones Auxiliares para la generación de números aleatorios ---
namespace {

  std::mt19937 & get_random_engine() {
    static constexpr unsigned int SEED = 150;
    static std::mt19937 gen(SEED);
    return gen;
  }

  // Función para crear un número random (entero) entre min y max (con una semilla aleatoria)
  int random_int(int min, int max) {
    static auto & gen = get_random_engine();
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
  }

  // --- Funciones Auxiliares para Ordenar ---

  // Función genérica para comparar dos objetos en un eje específico (0=x, 1=y, 2=z)
  inline bool box_compare(std::shared_ptr<render::Hittable> const & a,
                          std::shared_ptr<render::Hittable> const & b, int axis) {
    render::AABB box_a;
    render::AABB box_b;

    // Obtenemos la caja de cada objeto.
    // Si un objeto no tiene caja (ej. plano infinito sin límites), devolvemos error/falso.
    if (!a->bounding_box(box_a) or !b->bounding_box(box_b)) {
      std::cerr << "No bounding box in bvh_node constructor.\n";
    }

    // Comparamos la coordenada mínima del eje seleccionado
    if (axis == 0) {
      return box_a.min.x < box_b.min.x;
    }
    if (axis == 1) {
      return box_a.min.y < box_b.min.y;
    }
    return box_a.min.z < box_b.min.z;
  }

  // Wrappers específicos para std::sort
  bool box_x_compare(std::shared_ptr<render::Hittable> const & a,
                     std::shared_ptr<render::Hittable> const & b) {
    return box_compare(a, b, 0);
  }

  bool box_y_compare(std::shared_ptr<render::Hittable> const & a,
                     std::shared_ptr<render::Hittable> const & b) {
    return box_compare(a, b, 1);
  }

  bool box_z_compare(std::shared_ptr<render::Hittable> const & a,
                     std::shared_ptr<render::Hittable> const & b) {
    return box_compare(a, b, 2);
  }

}  // namespace

namespace render {

  // --- Implementación del Constructor ---

  BvhNode::BvhNode(std::vector<std::shared_ptr<Hittable>> & src_objects, size_t start, size_t end) {
    auto & objects = src_objects;

    // 1. Elegir un eje al azar para dividir (0=x, 1=y, 2=z)
    int const axis = random_int(0, 2);

    // Seleccionamos la función comparadora según el eje
    auto comparator = [axis]() {
      if (axis == 0) {
        return box_x_compare;
      }
      if (axis == 1) {
        return box_y_compare;
      }
      return box_z_compare;
    }();

    size_t const object_span = end - start;

    if (object_span == 1) {
      // Caso Base: Solo hay 1 objeto. Ambos hijos apuntan a él.
      left = right = objects[start];
    } else if (object_span == 2) {
      // Caso Base: Hay 2 objetos. Los ordenamos y asignamos uno a cada lado.
      assign_two_objects(objects, start, comparator);
    } else {
      // Caso Recursivo: Hay muchos objetos.
      std::sort(objects.begin() + static_cast<long>(start),
                objects.begin() + static_cast<long>(end), comparator);

      // Dividimos por la mitad
      size_t const mid = start + object_span / 2;

      // Creamos nodos hijos recursivamente
      left  = std::make_shared<BvhNode>(objects, start, mid);
      right = std::make_shared<BvhNode>(objects, mid, end);
    }
    // 2. Calcular la caja envolvente de ESTE nodo
    compute_bounding_box();
  }

  void BvhNode::assign_two_objects(std::vector<std::shared_ptr<Hittable>> & objects, size_t start,
                                   bool (*comparator)(std::shared_ptr<Hittable> const &,
                                                      std::shared_ptr<Hittable> const &)) {
    left  = comparator(objects[start], objects[start + 1]) ? objects[start] : objects[start + 1];
    right = (left == objects[start]) ? objects[start + 1] : objects[start];
  }

  void BvhNode::compute_bounding_box() {
    AABB box_left, box_right;
    if (!left->bounding_box(box_left) or !right->bounding_box(box_right)) {
      std::cerr << "No bounding box in bvh_node constructor.\n";
    }
    box = surrounding_box(box_left, box_right);
  }

  // --- Implementación de bounding_box ---

  bool BvhNode::bounding_box(AABB & output_box) const {
    output_box = box;
    return true;
  }

  // --- Implementación de hit ---

  bool BvhNode::hit(Ray const & r, double t_min, double t_max, InfoColision & record) const {
    // Si no toca la caja, no comprobamos
    if (!box.hit(r, t_min, t_max)) {
      return false;
    }

    // 2. Comprobar hijos
    bool const hit_left = left->hit(r, t_min, t_max, record);

    bool const hit_right = right->hit(r, t_min, hit_left ? record.t : t_max, record);

    return hit_left or hit_right;
  }

}  // namespace render
