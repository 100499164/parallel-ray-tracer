#ifndef RAYTRACER_HPP
#define RAYTRACER_HPP

#include "camera.hpp"
#include "color.hpp"
#include "image.hpp"
#include "infocolision.hpp"
#include "ray.hpp"
#include "sceneRender.hpp"

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <oneapi/tbb/blocked_range2d.h>
#include <random>
#include <thread>
#include <vector>

namespace render {

  // Partitioner types for TBB
  enum class PartitionerType { AUTO = 0, STATIC = 1, SIMPLE = 2 };

  // Configuration for parallel rendering
  struct ParallelConfig {
    PartitionerType pType = PartitionerType::SIMPLE;
    std::size_t grain     = 1;
  };

  // ==========================
  //  Helpers RNG por hilo
  // ==========================

  // Genera N semillas distintas a partir de una semilla maestra
  inline std::vector<std::uint64_t> make_seeds(std::uint64_t seed0, std::size_t N) {
    std::vector<std::uint64_t> seeds(N);
    std::mt19937_64 const gen(seed0);
    std::ranges::generate(seeds, gen);  // cada llamada produce una semilla distinta
    return seeds;
  }

  // ID de hilo único (0,1,2,...) asignado la primera vez que cada hilo entra aquí
  inline std::size_t thread_id() {
    static std::atomic<std::size_t> counter{0};
    thread_local std::size_t const id = counter++;
    return id;
  }

  // Pool de RNGs: cada hilo tiene su propio std::mt19937_64
  class RNGPool {
  public:
    RNGPool() = default;

    explicit RNGPool(std::vector<std::uint64_t> seeds) : seeds_(std::move(seeds)) { }

    std::mt19937_64 & local() {
      // cada hilo inicializa su RNG una vez con una de las semillas
      thread_local std::mt19937_64 rng(seeds_[thread_id() % seeds_.size()]);
      return rng;
    }

  private:
    std::vector<std::uint64_t> seeds_;
  };

  // ==========================
  //       RayTracer
  // ==========================

  class RayTracer {
  public:
    // --- Constructor ---
    RayTracer(int max_rebounds, int samples, render::Color background_light,
              render::Color background_dark, std::uint64_t ray_seed, std::uint64_t mat_seed,
              double gamma = 2.2)
        : MAX_DEPTH(max_rebounds), SAMPLES_PIXEL(samples), BACKGROUND_LIGHT(background_light),
          BACKGROUND_DARK(background_dark),
          ray_rng_pool(make_seeds(ray_seed, std::max(1U, std::thread::hardware_concurrency()))),
          mat_rng_pool(make_seeds(mat_seed, std::max(1U, std::thread::hardware_concurrency()))),
          INV_GAMMA(1.0 / gamma) { }

    // --- Main public method ---
    void render(camera const & cam, SceneRender const & escena, Image & imagen,
                ParallelConfig config);

  private:
    // --- Render Conf ---
    int MAX_DEPTH;                   // Max rebounds per ray
    int SAMPLES_PIXEL;               // Samples per pixel for AA
    render::Color BACKGROUND_LIGHT;  // Light color of the background
    render::Color BACKGROUND_DARK;   // Dark color of the background

    // --- Random Generators (por hilo) ---
    RNGPool ray_rng_pool;  // para AA / rayos primarios
    RNGPool mat_rng_pool;  // para materiales / scatter
    std::uniform_real_distribution<double> dist_aleatoria{-1.0, 1.0};
    std::uniform_real_distribution<double> dist_antialiasing{-0.5, 0.5};

    double INV_GAMMA;  // Inverse of gamma for correction

    // Cached camera geometry for current render
    render::vector origen_ventana;
    render::vector delta_vec_x;
    render::vector delta_vec_y;
    render::vector punto_vista;

  protected:
    // Calculates the color seen along a ray by bouncing it through the scene
    render::Color traceRay(render::Ray rayo_inicial, SceneRender const & escena);

    // Writes the final pixel to the image
    void writeColor(Image & imagen, size_t x, size_t y, render::Color color_acumulado) const;

    // Calculates the color of a single pixel by sampling multiple rays
    render::Color samplePixel(size_t x, size_t y, SceneRender const & escena);

    // Returns the background color based on ray direction
    [[nodiscard]] render::Color backgroundColor(render::Ray const & r) const;

    // Handles material scattering and updates attenuation and output ray
    bool handleMaterialScatter(render::Ray const & r_in, InfoColision const & info,
                               render::Color & atenuacion_acumulada, render::Ray & ray_out);

    // Processes a tile of the image
    void processTile(tbb::blocked_range2d<std::size_t> const & r, SceneRender const & escena,
                     Image & imagen);
  };

}  // namespace render
#endif  // RAYTRACER_HPP
