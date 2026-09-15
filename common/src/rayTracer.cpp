#include "rayTracer.hpp"
#include "camera.hpp"
#include "color.hpp"
#include "image.hpp"
#include "infocolision.hpp"
#include "materials_render/material_render.hpp"
#include "ray.hpp"
#include "scatter_context.hpp"
#include "sceneRender.hpp"
#include "vector.hpp"

#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>

// --- TBB ---
#include <oneapi/tbb/blocked_range2d.h>
#include <oneapi/tbb/parallel_for.h>

namespace render {

  /**
   * Main render loop.
   * Iterates through pixels, samples each pixel, and writes to the image.
   * Versión paralela con TBB: paralelizamos la imagen en 2D (filas y columnas).
   */
  void RayTracer::render(camera const & cam, SceneRender const & escena, Image & imagen,
                         ParallelConfig config) {
    size_t const alto  = imagen.getHeight();
    size_t const ancho = imagen.getWidth();

    origen_ventana = cam.origin();
    delta_vec_x    = cam.delta_x();
    delta_vec_y    = cam.delta_y();
    punto_vista    = cam.position();

    // 1. We define the 2D range (tiles)
    tbb::blocked_range2d<std::size_t> const range(0, alto, config.grain, 0, ancho, config.grain);
    std::cout << "Grainsize: " << config.grain << "x" << config.grain << "\n";

    // 2. We define the body of the work
    auto renderingLoop = [&](tbb::blocked_range2d<std::size_t> const & r) {
      processTile(r, escena, imagen);
    };
    // 3. Switch with the Partitioners
    switch (config.pType) {
      case PartitionerType::STATIC:
        std::cout << "Using STATIC partitioner (2D)\n";
        tbb::parallel_for(range, renderingLoop, tbb::static_partitioner());
        break;

      case PartitionerType::SIMPLE:
        std::cout << "Using SIMPLE partitioner (2D)\n";
        tbb::parallel_for(range, renderingLoop, tbb::simple_partitioner());
        break;

      case PartitionerType::AUTO:
      default:
        std::cout << "Using AUTO partitioner (2D)\n";
        tbb::parallel_for(range, renderingLoop, tbb::auto_partitioner());
        break;
    }

    std::cout << "\nRendering complete.\n";
  }

  /**
   *  Process a tile (2D block) of the image
   * Iterates through pixels in the tile, samples each pixel, and writes to the image.
   **/
  void RayTracer::processTile(tbb::blocked_range2d<std::size_t> const & r,
                              SceneRender const & escena, Image & imagen) {
    // r.rows() gives the vertical range of the current block
    for (std::size_t y = r.rows().begin(); y != r.rows().end(); ++y) {
      // r.cols() gives the horizontal range of the current block
      for (std::size_t x = r.cols().begin(); x != r.cols().end(); ++x) {
        Color const color_acumulado = samplePixel(x, y, escena);
        writeColor(imagen, x, y, color_acumulado);
      }
    }
  }

  /**
   * Traces a single ray iteratively.
   * Follows the ray through X bounces.
   */
  render::Color RayTracer::traceRay(render::Ray rayo_inicial, SceneRender const & escena) {
    render::Color atenuacion_acumulada(1.0, 1.0, 1.0);
    render::Ray rayo_actual = rayo_inicial;

    for (int i = 0; i < MAX_DEPTH; ++i) {
      InfoColision info;

      if (escena.hit(rayo_actual, 0.001, std::numeric_limits<double>::infinity(), info)) {
        // 1. HIT: Handle material scattering
        if (!handleMaterialScatter(rayo_actual, info, atenuacion_acumulada, rayo_actual)) {
          // Ray was absorbed
          return {0, 0, 0};
        }
        // Ray continues in loop...
      } else {
        // 2. BACKGROUND: No hit, return background color
        return atenuacion_acumulada * backgroundColor(rayo_actual);
      }
    }

    // 3. MAX DEPTH REACHED
    return {0, 0, 0};
  }

  // Private helper (Pixel Sampling):
  render::Color RayTracer::samplePixel(size_t x, size_t y, SceneRender const & escena) {
    Color color_acumulado(0, 0, 0);

    // Pixel center
    render::vector const q_centro = origen_ventana +
                                    (delta_vec_x * static_cast<double>(x)) +
                                    (delta_vec_y * static_cast<double>(y));

    render::vector const dir_centro = (q_centro - punto_vista);

    // thread-local RNG for rays
    auto & rrng = ray_rng_pool.local();

    // --- Sample Loop ("SAMPLES_PIXEL" rays per pixel) ---
    for (int s = 0; s < SAMPLES_PIXEL; ++s) {
      // 1. Generate random point in pixel (Antialiasing)
      double const delta_x = dist_antialiasing(rrng);
      double const delta_y = dist_antialiasing(rrng);

      // 2. Generate ray
      render::vector const dir =
          (dir_centro + (delta_vec_x * delta_x) + (delta_vec_y * delta_y)).normalized();

      render::Ray const rayo_aleatorio(punto_vista, dir);

      // 3. Trace ray
      color_acumulado += traceRay(rayo_aleatorio, escena);
    }

    return color_acumulado;
  }

  // Private helper (Background Color):
  render::Color RayTracer::backgroundColor(render::Ray const & r) const {
    double const t = 0.5 * (r.direccion.normalized().y + 1.0);
    return Color::lerp(BACKGROUND_LIGHT, BACKGROUND_DARK, t);
  }

  // Private helper (Material Scattering):
  bool RayTracer::handleMaterialScatter(render::Ray const & r_in, InfoColision const & info,
                                        render::Color & atenuacion_acumulada,
                                        render::Ray & ray_out) {
    render::Ray rayo_rebotado;
    render::Color atenuacion_material{};

    // RNG local del hilo para materiales
    auto & mrng = mat_rng_pool.local();

    // Ask material how to scatter
    render::ScatterContext const ctx{&r_in, &info, &mrng, &dist_aleatoria};
    if (info.mat_ptr->scatter(ctx, atenuacion_material, rayo_rebotado)) {
      atenuacion_acumulada *= atenuacion_material;
      ray_out = rayo_rebotado;  // Update ray for next iteration
      return true;
    }

    return false;  // Ray dies
  }

  /**
   * Private helper (Post-processing):
   * Applies averaging, gamma, and scaling to the final pixel.
   */
  void RayTracer::writeColor(Image & imagen, size_t x, size_t y, Color color_acumulado) const {
    // 1. Average color
    static double const escala = 1.0 / SAMPLES_PIXEL;
    double r                   = color_acumulado.r * escala;
    double g                   = color_acumulado.g * escala;
    double b                   = color_acumulado.b * escala;

    // 2. Gamma correction
    r = std::pow(r, INV_GAMMA);
    g = std::pow(g, INV_GAMMA);
    b = std::pow(b, INV_GAMMA);

    // 3. Write to Image interface
    imagen.setPixel(x, y, Color(r, g, b));
  }

}  // namespace render
