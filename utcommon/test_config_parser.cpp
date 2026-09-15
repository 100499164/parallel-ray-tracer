#include "config_parser.hpp"
#include <cstdlib>
#include <gtest/gtest.h>
#include <sstream>
#include <string>

TEST(test_configParser, parse_valid_file) {
  // Simulamos el contenido del archivo directamente en memoria
  std::string const config_text = R"(
aspect_ratio: 16 9
image_width: 1920
gamma: 2.2
camera_position: 0 0 -10
camera_target: 0 0 0
camera_north: 0 1 0
field_of_view: 90
samples_per_pixel: 20
max_depth: 5
material_rng_seed: 13
ray_rng_seed: 19
background_dark_color: 0.25 0.5 1.0
background_light_color: 1.0 1.0 1.0
)";

  std::istringstream fake_file(config_text);

  render::config_parser cfg(fake_file);
  cfg.parse();

  auto const & data = cfg.data();

  // --- Validaciones ---
  EXPECT_EQ(data.ar_w, 16);
  EXPECT_EQ(data.ar_h, 9);
  EXPECT_EQ(data.image_width, 1'920);
  EXPECT_NEAR(data.gamma, 2.2, 1e-6);

  EXPECT_DOUBLE_EQ(data.cam_pos[0], 0.0);
  EXPECT_DOUBLE_EQ(data.cam_pos[1], 0.0);
  EXPECT_DOUBLE_EQ(data.cam_pos[2], -10.0);

  EXPECT_DOUBLE_EQ(data.cam_tgt[0], 0.0);
  EXPECT_DOUBLE_EQ(data.cam_tgt[1], 0.0);
  EXPECT_DOUBLE_EQ(data.cam_tgt[2], 0.0);

  EXPECT_DOUBLE_EQ(data.cam_up[0], 0.0);
  EXPECT_DOUBLE_EQ(data.cam_up[1], 1.0);
  EXPECT_DOUBLE_EQ(data.cam_up[2], 0.0);

  EXPECT_NEAR(data.fov, 90.0, 1e-6);
  EXPECT_EQ(data.samples_per_pixel, 20);
  EXPECT_EQ(data.max_depth, 5);
  EXPECT_EQ(data.material_rng_seed, 13);
  EXPECT_EQ(data.ray_rng_seed, 19);

  EXPECT_NEAR(data.bg_dark[0], 0.25, 1e-6);
  EXPECT_NEAR(data.bg_dark[1], 0.5, 1e-6);
  EXPECT_NEAR(data.bg_dark[2], 1.0, 1e-6);

  EXPECT_NEAR(data.bg_light[0], 1.0, 1e-6);
  EXPECT_NEAR(data.bg_light[1], 1.0, 1e-6);
  EXPECT_NEAR(data.bg_light[2], 1.0, 1e-6);
}

TEST(test_configParser, not_specified_aspect_ratio) {
  // Simulamos un archivo de configuración SIN la clave 'aspect_ratio'
  std::string const config_text = R"(
image_width: 1920
gamma: 2.2
camera_position: 0 0 -10
camera_target: 0 0 0
camera_north: 0 1 0
field_of_view: 90
samples_per_pixel: 20
max_depth: 5
material_rng_seed: 13
ray_rng_seed: 19
background_dark_color: 0.25 0.5 1.0
background_light_color: 1.0 1.0 1.0
)";

  std::istringstream fake_file(config_text);

  render::config_parser cfg(fake_file);
  cfg.parse();

  auto const & data = cfg.data();

  // --- Validaciones ---
  // Debe asignarse el aspect_ratio por defecto (16:9)
  EXPECT_EQ(data.ar_w, 16);
  EXPECT_EQ(data.ar_h, 9);

  EXPECT_EQ(data.image_width, 1'920);
  EXPECT_NEAR(data.gamma, 2.2, 1e-6);

  EXPECT_DOUBLE_EQ(data.cam_pos[0], 0.0);
  EXPECT_DOUBLE_EQ(data.cam_pos[1], 0.0);
  EXPECT_DOUBLE_EQ(data.cam_pos[2], -10.0);

  EXPECT_DOUBLE_EQ(data.cam_tgt[0], 0.0);
  EXPECT_DOUBLE_EQ(data.cam_tgt[1], 0.0);
  EXPECT_DOUBLE_EQ(data.cam_tgt[2], 0.0);

  EXPECT_DOUBLE_EQ(data.cam_up[0], 0.0);
  EXPECT_DOUBLE_EQ(data.cam_up[1], 1.0);
  EXPECT_DOUBLE_EQ(data.cam_up[2], 0.0);

  EXPECT_NEAR(data.fov, 90.0, 1e-6);
  EXPECT_EQ(data.samples_per_pixel, 20);
  EXPECT_EQ(data.max_depth, 5);
  EXPECT_EQ(data.material_rng_seed, 13);
  EXPECT_EQ(data.ray_rng_seed, 19);

  EXPECT_NEAR(data.bg_dark[0], 0.25, 1e-6);
  EXPECT_NEAR(data.bg_dark[1], 0.5, 1e-6);
  EXPECT_NEAR(data.bg_dark[2], 1.0, 1e-6);

  EXPECT_NEAR(data.bg_light[0], 1.0, 1e-6);
  EXPECT_NEAR(data.bg_light[1], 1.0, 1e-6);
  EXPECT_NEAR(data.bg_light[2], 1.0, 1e-6);
}

TEST(test_configParser, only_one_key_specified) {
  // Archivo simulado con una sola clave especificada (aspect_ratio)
  std::string const config_text = R"(
aspect_ratio: 16 9
)";

  std::istringstream fake_file(config_text);

  render::config_parser cfg(fake_file);
  cfg.parse();

  auto const & data = cfg.data();

  // --- Validaciones ---
  EXPECT_EQ(data.ar_w, 16);
  EXPECT_EQ(data.ar_h, 9);

  // El resto debe tomar valores por defecto
  EXPECT_EQ(data.image_width, 1'920);
  EXPECT_NEAR(data.gamma, 2.2, 1e-6);

  EXPECT_DOUBLE_EQ(data.cam_pos[0], 0.0);
  EXPECT_DOUBLE_EQ(data.cam_pos[1], 0.0);
  EXPECT_DOUBLE_EQ(data.cam_pos[2], -10.0);

  EXPECT_DOUBLE_EQ(data.cam_tgt[0], 0.0);
  EXPECT_DOUBLE_EQ(data.cam_tgt[1], 0.0);
  EXPECT_DOUBLE_EQ(data.cam_tgt[2], 0.0);

  EXPECT_DOUBLE_EQ(data.cam_up[0], 0.0);
  EXPECT_DOUBLE_EQ(data.cam_up[1], 1.0);
  EXPECT_DOUBLE_EQ(data.cam_up[2], 0.0);

  EXPECT_NEAR(data.fov, 90.0, 1e-6);
  EXPECT_EQ(data.samples_per_pixel, 20);
  EXPECT_EQ(data.max_depth, 5);
  EXPECT_EQ(data.material_rng_seed, 13);
  EXPECT_EQ(data.ray_rng_seed, 19);

  EXPECT_NEAR(data.bg_dark[0], 0.25, 1e-6);
  EXPECT_NEAR(data.bg_dark[1], 0.5, 1e-6);
  EXPECT_NEAR(data.bg_dark[2], 1.0, 1e-6);

  EXPECT_NEAR(data.bg_light[0], 1.0, 1e-6);
  EXPECT_NEAR(data.bg_light[1], 1.0, 1e-6);
  EXPECT_NEAR(data.bg_light[2], 1.0, 1e-6);
}

TEST(test_configParser, white_lines_ignored_extra_spaces) {
  // Archivo de configuración con líneas en blanco y espacios adicionales
  std::string const config_text = R"(

  aspect_ratio:     16    9
  image_width:    1920

  gamma:     2.2


  camera_position:     0   0   -10
  camera_target: 0 0 0
  camera_north:   0 1 0

  field_of_view:   90
  samples_per_pixel: 20
  max_depth:   5
  material_rng_seed:   13
  ray_rng_seed:   19

  background_dark_color:  0.25    0.5   1.0
  background_light_color:   1.0   1.0   1.0


)";

  std::istringstream fake_file(config_text);

  render::config_parser cfg(fake_file);
  cfg.parse();

  auto const & data = cfg.data();

  // --- Validaciones ---
  EXPECT_EQ(data.ar_w, 16);
  EXPECT_EQ(data.ar_h, 9);
  EXPECT_EQ(data.image_width, 1'920);
  EXPECT_NEAR(data.gamma, 2.2, 1e-6);

  EXPECT_DOUBLE_EQ(data.cam_pos[0], 0.0);
  EXPECT_DOUBLE_EQ(data.cam_pos[1], 0.0);
  EXPECT_DOUBLE_EQ(data.cam_pos[2], -10.0);

  EXPECT_DOUBLE_EQ(data.cam_tgt[0], 0.0);
  EXPECT_DOUBLE_EQ(data.cam_tgt[1], 0.0);
  EXPECT_DOUBLE_EQ(data.cam_tgt[2], 0.0);

  EXPECT_DOUBLE_EQ(data.cam_up[0], 0.0);
  EXPECT_DOUBLE_EQ(data.cam_up[1], 1.0);
  EXPECT_DOUBLE_EQ(data.cam_up[2], 0.0);

  EXPECT_NEAR(data.fov, 90.0, 1e-6);
  EXPECT_EQ(data.samples_per_pixel, 20);
  EXPECT_EQ(data.max_depth, 5);
  EXPECT_EQ(data.material_rng_seed, 13);
  EXPECT_EQ(data.ray_rng_seed, 19);

  EXPECT_NEAR(data.bg_dark[0], 0.25, 1e-6);
  EXPECT_NEAR(data.bg_dark[1], 0.5, 1e-6);
  EXPECT_NEAR(data.bg_dark[2], 1.0, 1e-6);

  EXPECT_NEAR(data.bg_light[0], 1.0, 1e-6);
  EXPECT_NEAR(data.bg_light[1], 1.0, 1e-6);
  EXPECT_NEAR(data.bg_light[2], 1.0, 1e-6);
}

TEST(test_configParser, duplicated_parameters) {
  // Configuración con parámetros duplicados (gamma aparece dos veces)
  std::string const config_text = R"(
aspect_ratio: 16 9
image_width: 1920
gamma: 2.2
gamma: 3.5
camera_position: 0 0 -10
camera_target: 0 0 0
camera_north: 0 1 0
field_of_view: 90
samples_per_pixel: 20
max_depth: 5
material_rng_seed: 13
ray_rng_seed: 19
background_dark_color: 0.25 0.5 1.0
background_light_color: 1.0 1.0 1.0
)";

  std::istringstream fake_file(config_text);

  render::config_parser cfg(fake_file);
  cfg.parse();

  auto const & data = cfg.data();

  // --- Validaciones ---
  // Se espera que prevalezca el último valor leído (3.5)
  EXPECT_EQ(data.ar_w, 16);
  EXPECT_EQ(data.ar_h, 9);
  EXPECT_EQ(data.image_width, 1'920);
  EXPECT_NEAR(data.gamma, 3.5, 1e-6);

  EXPECT_DOUBLE_EQ(data.cam_pos[0], 0.0);
  EXPECT_DOUBLE_EQ(data.cam_pos[1], 0.0);
  EXPECT_DOUBLE_EQ(data.cam_pos[2], -10.0);

  EXPECT_DOUBLE_EQ(data.cam_tgt[0], 0.0);
  EXPECT_DOUBLE_EQ(data.cam_tgt[1], 0.0);
  EXPECT_DOUBLE_EQ(data.cam_tgt[2], 0.0);

  EXPECT_DOUBLE_EQ(data.cam_up[0], 0.0);
  EXPECT_DOUBLE_EQ(data.cam_up[1], 1.0);
  EXPECT_DOUBLE_EQ(data.cam_up[2], 0.0);

  EXPECT_NEAR(data.fov, 90.0, 1e-6);
  EXPECT_EQ(data.samples_per_pixel, 20);
  EXPECT_EQ(data.max_depth, 5);
  EXPECT_EQ(data.material_rng_seed, 13);
  EXPECT_EQ(data.ray_rng_seed, 19);

  EXPECT_NEAR(data.bg_dark[0], 0.25, 1e-6);
  EXPECT_NEAR(data.bg_dark[1], 0.5, 1e-6);
  EXPECT_NEAR(data.bg_dark[2], 1.0, 1e-6);

  EXPECT_NEAR(data.bg_light[0], 1.0, 1e-6);
  EXPECT_NEAR(data.bg_light[1], 1.0, 1e-6);
  EXPECT_NEAR(data.bg_light[2], 1.0, 1e-6);
}

TEST(test_configParser, unknown_configuration_key) {
  // Clave desconocida: "image_xwidth"
  std::string const config_text = R"(
aspect_ratio: 16 9
image_xwidth: 1920
gamma: 2.2
camera_position: 0 0 -10
camera_target: 0 0 0
camera_north: 0 1 0
field_of_view: 90
samples_per_pixel: 20
max_depth: 5
material_rng_seed: 13
ray_rng_seed: 19
background_dark_color: 0.25 0.5 1.0
background_light_color: 1.0 1.0 1.0
)";

  std::istringstream fake_file(config_text);

  render::config_parser cfg(fake_file);

  // Esperamos que el programa termine con exit(EXIT_FAILURE)
  EXPECT_EXIT(cfg.parse(), ::testing::ExitedWithCode(EXIT_FAILURE),
              "Error: Unknown configuration key: \\[image_xwidth:\\]");
}

TEST(test_configParser, unknown_configuration_key_missing_colon) {
  // Clave válida sin el carácter ':'
  std::string const config_text = R"(
aspect_ratio: 16 9
image_width 1920
gamma: 2.2
camera_position: 0 0 -10
camera_target: 0 0 0
camera_north: 0 1 0
field_of_view: 90
samples_per_pixel: 20
max_depth: 5
material_rng_seed: 13
ray_rng_seed: 19
background_dark_color: 0.25 0.5 1.0
background_light_color: 1.0 1.0 1.0

)";

  std::istringstream fake_file(config_text);

  render::config_parser cfg(fake_file);

  // Esperamos que el parser falle porque falta el ':'
  EXPECT_EXIT(cfg.parse(), ::testing::ExitedWithCode(EXIT_FAILURE),
              "Error: Unknown configuration key: \\[image_width:\\]");
}

TEST(test_configParser, invalid_key_value) {
  // Clave válida con valor no numérico
  std::string const config_text = R"(
aspect_ratio: nulo
image_width: 1920
gamma: 2.2
camera_position: 0 0 -10
camera_target: 0 0 0
camera_north: 0 1 0
field_of_view: 90
samples_per_pixel: 20
max_depth: 5
material_rng_seed: 13
ray_rng_seed: 19
background_dark_color: 0.25 0.5 1.0
background_light_color: 1.0 1.0 1.0
)";

  std::istringstream fake_file(config_text);

  render::config_parser cfg(fake_file);

  // Esperamos que el parser falle por valor inválido en aspect_ratio
  EXPECT_EXIT(cfg.parse(), ::testing::ExitedWithCode(EXIT_FAILURE),
              "Error: Invalid value for key: \\[aspect_ratio:\\]");
}

TEST(test_configParser, unexpected_info) {
  // Valor extra después de una configuración válida de vector3
  std::string const config_text = R"(
aspect_ratio: 16 9
image_width: 1920
gamma: 2.2
camera_position: 0 0 -10 
camera_target: 0 0 0
camera_north: 0 1 0 23
field_of_view: 90
samples_per_pixel: 20
max_depth: 5
material_rng_seed: 13
ray_rng_seed: 19
background_dark_color: 0.25 0.5 1.0
background_light_color: 1.0 1.0 1.0
)";
  std::istringstream fake_file(config_text);
  render::config_parser cfg(fake_file);

  // Esperamos que el parser finalice con exit(EXIT_FAILURE)
  EXPECT_EXIT(cfg.parse(), ::testing::ExitedWithCode(EXIT_FAILURE),
              "Error: Extra data after configuration value for key: "
              "\\[camera_north:\\][[:space:]]*Extra: \".*\"");
}

TEST(test_configParser, insufficient_information) {
  // Faltan componentes en un valor de tipo vector3
  std::string const config_text = R"(
aspect_ratio: 16 9
image_width: 1920
gamma: 2.2
camera_position: 0 0
camera_target: 0 0 0
camera_north: 0 1 0
field_of_view: 90
samples_per_pixel: 20
max_depth: 5
material_rng_seed: 13
ray_rng_seed: 19
background_dark_color: 0.25 0.5 1.0
background_light_color: 1.0 1.0 1.0
)";
  std::istringstream fake_file(config_text);
  render::config_parser cfg(fake_file);

  // Esperamos que el parser finalice con exit(EXIT_FAILURE)
  EXPECT_EXIT(cfg.parse(), ::testing::ExitedWithCode(EXIT_FAILURE),
              "Error: Invalid value for key: \\[camera_position:\\][[:space:]]*Line: \".*\"");
}

TEST(test_configParser, empty_file) {
  // Simulación de un archivo de configuración vacío
  std::string const config_text = " ";
  std::istringstream fake_file(config_text);
  render::config_parser cfg(fake_file);

  // Esperamos que el parser finalice con exit(EXIT_FAILURE)
  EXPECT_EXIT(cfg.parse(), ::testing::ExitedWithCode(EXIT_FAILURE),
              "ERROR: Configuration is empty or contains no valid keys.");
}

TEST(test_configParser, invalid_aspect_ratio) {
  // Un valor negativo en aspect_ratio
  std::string const config_text = R"(
aspect_ratio: -16 9
image_width: 1920
gamma: 2.2
camera_position: 0 0
camera_target: 0 0 0
camera_north: 0 1 0
field_of_view: 90
samples_per_pixel: 20
max_depth: 5
material_rng_seed: 13
ray_rng_seed: 19
background_dark_color: 0.25 0.5 1.0
background_light_color: 1.0 1.0 1.0
)";
  std::istringstream fake_file(config_text);
  render::config_parser cfg(fake_file);

  EXPECT_EXIT(cfg.parse(), ::testing::ExitedWithCode(EXIT_FAILURE),
              "Error: Invalid value for key: \\[aspect_ratio:\\][[:space:]]*Line: \".*\"");
}

TEST(test_configParser, invalid_image_width) {
  // Un valor igual a 0 para image_width
  std::string const config_text = R"(
aspect_ratio: 16 9
image_width: 0
gamma: 2.2
camera_position: 0 0 -10
camera_target: 0 0 0
camera_north: 0 1 0
field_of_view: 90
samples_per_pixel: 20
max_depth: 5
material_rng_seed: 13
ray_rng_seed: 19
background_dark_color: 0.25 0.5 1.0
background_light_color: 1.0 1.0 1.0
)";
  std::istringstream fake_file(config_text);
  render::config_parser cfg(fake_file);

  // Esperamos que el parser finalice con exit(EXIT_FAILURE)
  EXPECT_EXIT(cfg.parse(), ::testing::ExitedWithCode(EXIT_FAILURE),
              "Error: Invalid value for key: \\[image_width:\\][[:space:]]*Line: \".*\"");
}

TEST(test_configParser, invalid_gamma) {
  // Un valor no numérico para gamma
  std::string const config_text = R"(
aspect_ratio: 16 9
image_width: 1920
gamma: abc
camera_position: 0 0 -10
camera_target: 0 0 0
camera_north: 0 1 0
field_of_view: 90
samples_per_pixel: 20
max_depth: 5
material_rng_seed: 13
ray_rng_seed: 19
background_dark_color: 0.25 0.5 1.0
background_light_color: 1.0 1.0 1.0
)";
  std::istringstream fake_file(config_text);
  render::config_parser cfg(fake_file);

  EXPECT_EXIT(cfg.parse(), ::testing::ExitedWithCode(EXIT_FAILURE),
              "Error: Invalid value for key: \\[gamma:\\][[:space:]]*Line: \".*\"");
}

TEST(test_configParser, invalid_field_of_view) {
  // Un valor mayor a 180 para field_of_view
  std::string const config_text = R"(
aspect_ratio: 16 9
image_width: 1920
gamma: 2.2
camera_position: 0 0 -10
camera_target: 0 0 0
camera_north: 0 1 0
field_of_view: 181
samples_per_pixel: 20
max_depth: 5
material_rng_seed: 13
ray_rng_seed: 19
background_dark_color: 0.25 0.5 1.0
background_light_color: 1.0 1.0 1.0
)";
  std::istringstream fake_file(config_text);
  render::config_parser cfg(fake_file);

  // Esperamos que el parser finalice con exit(EXIT_FAILURE)
  EXPECT_EXIT(cfg.parse(), ::testing::ExitedWithCode(EXIT_FAILURE),
              "Error: Invalid value for key: \\[field_of_view:\\][[:space:]]*Line: \".*\"");
}

TEST(test_configParser, invalid_samples_per_pixel) {
  // Un valor negativo para samples_per_pixel
  std::string const config_text = R"(
aspect_ratio: 16 9
image_width: 1920
gamma: 2.2
camera_position: 0 0 -10
camera_target: 0 0 0
camera_north: 0 1 0
field_of_view: 90
samples_per_pixel: -50
max_depth: 5
material_rng_seed: 13
ray_rng_seed: 19
background_dark_color: 0.25 0.5 1.0
background_light_color: 1.0 1.0 1.0
)";
  std::istringstream fake_file(config_text);
  render::config_parser cfg(fake_file);

  // Esperamos que el parser finalice con exit(EXIT_FAILURE)
  EXPECT_EXIT(cfg.parse(), ::testing::ExitedWithCode(EXIT_FAILURE),
              "Error: Invalid value for key: \\[samples_per_pixel:\\][[:space:]]*Line: \".*\"");
}

TEST(test_configParser, invalid_max_depth) {
  // Un valor igual a 0 para max_depth
  std::string const config_text = R"(
aspect_ratio: 16 9
image_width: 1920
gamma: 2.2
camera_position: 0 0 -10
camera_target: 0 0 0
camera_north: 0 1 0
field_of_view: 90
samples_per_pixel: 20
max_depth: 0
material_rng_seed: 13
ray_rng_seed: 19
background_dark_color: 0.25 0.5 1.0
background_light_color: 1.0 1.0 1.0
)";
  std::istringstream fake_file(config_text);
  render::config_parser cfg(fake_file);

  // Esperamos que el parser finalice con exit(EXIT_FAILURE)
  EXPECT_EXIT(cfg.parse(), ::testing::ExitedWithCode(EXIT_FAILURE),
              "Error: Invalid value for key: \\[max_depth:\\][[:space:]]*Line: \".*\"");
}

TEST(test_configParser, invalid_material_rng_seed) {
  // Un valor igual a 0 para material_rng_seed
  std::string const config_text = R"(
aspect_ratio: 16 9
image_width: 1920
gamma: 2.2
camera_position: 0 0 -10
camera_target: 0 0 0
camera_north: 0 1 0
field_of_view: 90
samples_per_pixel: 20
max_depth: 5
material_rng_seed: 0
ray_rng_seed: 19
background_dark_color: 0.25 0.5 1.0
background_light_color: 1.0 1.0 1.0
)";
  std::istringstream fake_file(config_text);
  render::config_parser cfg(fake_file);

  // Esperamos que el parser finalice con exit(EXIT_FAILURE)
  EXPECT_EXIT(cfg.parse(), ::testing::ExitedWithCode(EXIT_FAILURE),
              "Error: Invalid value for key: \\[material_rng_seed:\\][[:space:]]*Line: \".*\"");
}

TEST(test_configParser, invalid_ray_rng_seed) {
  // Un valor negativo para ray_rng_seed
  std::string const config_text = R"(
aspect_ratio: 16 9
image_width: 1920
gamma: 2.2
camera_position: 0 0 -10
camera_target: 0 0 0
camera_north: 0 1 0
field_of_view: 90
samples_per_pixel: 20
max_depth: 5
material_rng_seed: 13
ray_rng_seed: -123
background_dark_color: 0.25 0.5 1.0
background_light_color: 1.0 1.0 1.0
)";
  std::istringstream fake_file(config_text);
  render::config_parser cfg(fake_file);

  // Esperamos que el parser finalice con exit(EXIT_FAILURE)
  EXPECT_EXIT(cfg.parse(), ::testing::ExitedWithCode(EXIT_FAILURE),
              "Error: Invalid value for key: \\[ray_rng_seed:\\][[:space:]]*Line: \".*\"");
}

TEST(test_configParser, invalid_background_dark_color) {
  // Un valor fuera del rango [0.0, 1.0] para background_dark_color
  std::string const config_text = R"(
aspect_ratio: 16 9
image_width: 1920
gamma: 2.2
camera_position: 0 0 -10
camera_target: 0 0 0
camera_north: 0 1 0
field_of_view: 90
samples_per_pixel: 20
max_depth: 5
material_rng_seed: 13
ray_rng_seed: 19
background_dark_color: 0.25 1.5 1.0
background_light_color: 1.0 1.0 1.0
)";
  std::istringstream fake_file(config_text);
  render::config_parser cfg(fake_file);

  // Esperamos que el parser finalice con exit(EXIT_FAILURE)
  EXPECT_EXIT(cfg.parse(), ::testing::ExitedWithCode(EXIT_FAILURE),
              "Error: Invalid value for key: \\[background_dark_color:\\][[:space:]]*Line: \".*\"");
}

TEST(test_configParser, invalid_background_light_color) {
  // Un valor fuera del rango [0.0, 1.0] para background_light_color
  std::string const config_text = R"(
aspect_ratio: 16 9
image_width: 1920
gamma: 2.2
camera_position: 0 0 -10
camera_target: 0 0 0
camera_north: 0 1 0
field_of_view: 90
samples_per_pixel: 20
max_depth: 5
material_rng_seed: 13
ray_rng_seed: 19
background_dark_color: 0.25 0.5 1.0
background_light_color: -1.0 1.0 1.0
)";
  std::istringstream fake_file(config_text);
  render::config_parser cfg(fake_file);

  // Esperamos que el parser finalice con exit(EXIT_FAILURE)
  EXPECT_EXIT(
      cfg.parse(), ::testing::ExitedWithCode(EXIT_FAILURE),
      "Error: Invalid value for key: \\[background_light_color:\\][[:space:]]*Line: \".*\"");
}
