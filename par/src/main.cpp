#include <span>
#include <string>
#include <iostream>

#include "camera.hpp"
#include "config_parser.hpp"
#include "par.hpp"
#include "rayTracer.hpp"
#include "sceneParser.hpp"
#include "sceneRender.hpp"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <oneapi/tbb/global_control.h>

namespace {

  struct RenderConfig {
    std::string configFile;
    std::string sceneFile;
    std::string outputFile;
    // This is the best default configuration for s5 scene
    size_t numThreads             = 256;
    render::PartitionerType pType = render::PartitionerType::SIMPLE;
    size_t grainSize              = 1;
  };

  // Intenta leer los hilos. Devuelve false si hay un error fatal.
  bool parseThreadArg(char const * arg, size_t & outThreads) {
    try {
      const int val = std::stoi(arg);
      if (val > 0) {
        outThreads = static_cast<size_t>(val);
      } else {
        std::cerr << "Warning: Threads must be > 0. Using default.\n";
      }
    } catch (std::exception const & e) {
      std::cerr << "Error parsing threads: " << e.what() << "\n";
      return false;
    }
    return true;
  }

  // Intenta leer el partitioner. Nunca falla fatalmente (usa default si hay error).
  void parsePartitionerArg(char const * arg, render::PartitionerType & outType) {
    try {
      const int val = std::stoi(arg);
      if (val >= 0 and val <= 3) {
        outType = static_cast<render::PartitionerType>(val);
      } else {
        std::cerr << "Warning: Invalid partitioner ID. Using AUTO.\n";
      }
    } catch (...) {
      std::cerr << "Error parsing partitioner. Using AUTO.\n";
    }
  }

  // Intenta leer el tamaño de grano.
  void parseGrainArg(char const * arg, size_t & outGrain) {
    try {
      const int val = std::stoi(arg);
      if (val > 0) {
        outGrain = static_cast<size_t>(val);
      } else {
        std::cerr << "Warning: Grain size must be > 0. Using default (16).\n";
      }
    } catch (...) {
      std::cerr << "Error parsing grain size. Using default (16).\n";
    }
  }

  bool parseArguments(std::span<char *> args, RenderConfig & config) {
    // 1. Validar tamaño del span
    if (args.size() < 4 or args.size() > 7) {
      std::cerr
          << "Usage: ./render-soa <config> <scene> <output> [threads] [partitioner] [grain]\n";
      return false;
    }

    // 2. Asignar obligatorios
    config.configFile = args[1];
    config.sceneFile  = args[2];
    config.outputFile = args[3];

    // 3. Opcional: Hilos (si el span tiene al menos 5 elementos)
    if (args.size() >= 5) {
      if (!parseThreadArg(args[4], config.numThreads)) {
        return false;  // Falló el parseo de hilos
      }
    }

    // 4. Opcional: Partitioner (si el span tiene 6 elementos)
    if (args.size() >= 6) {
      parsePartitionerArg(args[5], config.pType);
    }

    // 5. Opcional: Grain Size
    if (args.size() == 7) {
      parseGrainArg(args[6], config.grainSize);
    }

    return true;
  }

  /**
   * @brief Crea la cámara a partir de los datos de configuración.
   */
  render::camera createCamera(render::config_data const & cfg) {
    return render::camera(
        {.position     = render::vector(cfg.cam_pos[0], cfg.cam_pos[1], cfg.cam_pos[2]),
         .target       = render::vector(cfg.cam_tgt[0], cfg.cam_tgt[1], cfg.cam_tgt[2]),
         .north        = render::vector(cfg.cam_up[0], cfg.cam_up[1], cfg.cam_up[2]),
         .fov_deg      = cfg.fov,
         .image_width  = cfg.image_width,
         .image_height = static_cast<int>(
             std::trunc(cfg.image_width * static_cast<double>(cfg.ar_h) / cfg.ar_w)),
         .ray_rng_seed = static_cast<unsigned long long>(cfg.ray_rng_seed)});
  }

  /**
   * @brief Crea el trazador de rayos a partir de los datos de configuración.
   */
  render::RayTracer createTracer(render::config_data const & cfg) {
    return {cfg.max_depth,
            cfg.samples_per_pixel,
            render::Color(cfg.bg_light[0], cfg.bg_light[1], cfg.bg_light[2]),
            render::Color(cfg.bg_dark[0], cfg.bg_dark[1], cfg.bg_dark[2]),
            static_cast<uint64_t>(cfg.ray_rng_seed),
            static_cast<uint64_t>(cfg.material_rng_seed),
            cfg.gamma};
  }

  /**
   * @brief Construye la escena renderizable a partir de los datos parseados.
   * @return true si se construye con éxito, false si falta un material.
   */
  bool buildScene(render::SceneRender & scene, render::sceneParser const & parser) {
    for (auto const & fig : parser.figures) {
      auto it = parser.materials.find(fig->material_name);
      if (it != parser.materials.end()) {
        scene.add_render_figure(fig, it->second);
      } else {
        // Mantenemos el mensaje de error exacto y el flujo de retorno.
        std::cerr << "Error: Undefined material for figure '" << fig->material_name << "'\n";
        return false;
      }
    }
    return true;
  }

  // --- Main Application Logic (< 40 lines each) ---

  /**
   * @brief Ejecuta los pasos secuenciales del renderizado (el "trabajo").
   * Esta función asume que será llamada desde un bloque try...catch.
   * @return true si se ejecuta con éxito, false si ocurre un error.
   */
  bool executeRenderPipeline(RenderConfig const & config) {
    // 1. Parsear archivo de configuración
    render::config_parser cfgp(config.configFile);
    cfgp.parse();  // llena cfgp.data()
    render::config_data const & cfg = cfgp.data();

    // 2. Parsear archivo de escena
    render::sceneParser parser;
    if (!parser.parse(config.sceneFile)) {
      std::cerr << "Error: Failed to parse scene file\n";
      return false;  // Indica fallo
    }
    // 3. Crear la cámara
    render::camera const cam = createCamera(cfg);

    // 4. Construir escena renderizable
    render::SceneRender scene;
    if (!buildScene(scene, parser)) {
      return false;  // Indica fallo
    }
    scene.build_bvh();

    // 5. Crear imagen AOS y trazador
    render::AOS image(static_cast<size_t>(cam.width()), static_cast<size_t>(cam.height()));
    render::RayTracer tracer = createTracer(cfg);

    // 6. Renderizar
    render::ParallelConfig par_config;
    par_config.pType = config.pType;
    par_config.grain = config.grainSize;
    tracer.render(cam, scene, image, par_config);

    // 7. Guardar la imagen
    if (image.saveAsPPM(config.outputFile)) {
      std::cout << "Image saved successfully in " << config.outputFile << '\n';
    } else {
      std::cerr << "Error: Could not write output file " << config.outputFile << '\n';
      return false;
    }
    return true;
  }

  /**
   * @brief Orquesta la ejecución del pipeline y maneja las excepciones.
   * @return true si se ejecuta con éxito, false si ocurre algún error.
   */
  bool runRenderer(RenderConfig const & config) {
    try {
      if (!executeRenderPipeline(config)) {
        return false;  // Propaga el fallo
      }
    } catch (std::exception const & e) {
      std::cerr << "Exception: " << e.what() << '\n';
      return false;  // Indica fallo
    }
    return true;  // Indica éxito
  }

}  // namespace

// --- Main Function (< 40 lines) ---

int main(int argc, char * argv[]) {
  // 1. Parseo de argumentos
  if (argc < 4 or argc > 7) {
    std::cerr << "Error: Invalid number of arguments: " << argc - 1 << '\n';
    std::cerr << "Usage: ./render-par <configFile> <sceneFile> <outputFile> [numThreads] "
                 "[partitioner] [grain]\n";
    return EXIT_FAILURE;
  }

  auto const args = std::span(argv, static_cast<size_t>(argc));
  RenderConfig config;

  if (!parseArguments(args, config)) {
    return EXIT_FAILURE;
  }

  std::cout << "Using " << config.numThreads << " threads.\n";

  // 2. Establecer el número de hilos que se va a usar a nivel global
  const tbb::global_control global_limit(tbb::global_control::max_allowed_parallelism, config.numThreads);

  // 3. Ejecutar la lógica principal de renderizado
  if (!runRenderer(config)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
