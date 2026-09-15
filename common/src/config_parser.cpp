#include "config_parser.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <charconv>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <unordered_map>

namespace {

  bool parse_int(std::string_view & values, int & result) {
    char const * start = values.data();
    char const * end   = values.data() + values.size();

    auto [ptr, ec] = std::from_chars(start, end, result);
    if (ec != std::errc()) {
      return false;
    }

    values.remove_prefix(static_cast<std::size_t>(ptr - start));

    while (!values.empty() and std::isspace(static_cast<unsigned char>(values.front())) != 0) {
      values.remove_prefix(1);
    }

    return true;
  }

  bool parse_real(std::string_view & values, double & result) {
    char const * start = values.data();
    char const * end   = values.data() + values.size();

    auto [ptr, ec] = std::from_chars(start, end, result);
    if (ec != std::errc()) {
      return false;
    }

    values.remove_prefix(static_cast<std::size_t>(ptr - start));

    while (!values.empty() and std::isspace(static_cast<unsigned char>(values.front())) != 0) {
      values.remove_prefix(1);
    }

    return true;
  }

  enum class Vec3ParseResult { OK, INVALID, EXTRA };

  Vec3ParseResult parse_vec3(std::string_view values, std::array<double, 3> & out) {
    double x = 0.0, y = 0.0, z = 0.0;

    if (!parse_real(values, x)) {
      return Vec3ParseResult::INVALID;
    }
    if (!parse_real(values, y)) {
      return Vec3ParseResult::INVALID;
    }
    if (!parse_real(values, z)) {
      return Vec3ParseResult::INVALID;
    }

    while (!values.empty() and std::isspace(static_cast<unsigned char>(values.front())) != 0) {
      values.remove_prefix(1);
    }

    if (!values.empty()) {
      return Vec3ParseResult::EXTRA;
    }

    out = {x, y, z};
    return Vec3ParseResult::OK;
  }

  void print_invalid_value_error(std::string_view key, std::string_view line) {
    std::cerr << "Error: Invalid value for key: [" << key << ":]\n"
              << "Line: \"" << line << "\"\n";
    std::exit(EXIT_FAILURE);
  }

  void print_extra_data_error(std::string_view key, std::string_view extra) {
    std::cerr << "Error: Extra data after configuration value for key: [" << key << ":]\n"
              << "Extra: \"" << extra << "\"\n";
    std::exit(EXIT_FAILURE);
  }

}  // namespace

namespace render {

  std::unordered_map<std::string_view, config_parser::setter_ptr> const
      config_parser::setter_table = {
        {          "aspect_ratio", &config_parser::set_aspect_ratio},
        {           "image_width",  &config_parser::set_image_width},
        {                 "gamma",        &config_parser::set_gamma},
        {       "camera_position", &config_parser::set_cam_position},
        {         "camera_target",   &config_parser::set_cam_target},
        {          "camera_north",    &config_parser::set_cam_north},
        {         "field_of_view",          &config_parser::set_fov},
        {     "samples_per_pixel",          &config_parser::set_spp},
        {             "max_depth",    &config_parser::set_max_depth},
        {     "material_rng_seed",     &config_parser::set_mat_seed},
        {          "ray_rng_seed",     &config_parser::set_ray_seed},
        { "background_dark_color",      &config_parser::set_bg_dark},
        {"background_light_color",     &config_parser::set_bg_light}
  };

  void config_parser::parse() {
    std::ifstream file_stream;
    std::istream * stream_to_use = input_stream_;

    if (stream_to_use == nullptr) {
      file_stream.open(config_path_);
      if (!file_stream.is_open()) {
        std::cerr << "ERROR: Unable to open configuration file: " << config_path_ << "\n";
        std::exit(EXIT_FAILURE);
      }
      stream_to_use = &file_stream;
    }

    std::string line;

    while (std::getline(*stream_to_use, line)) {
      if (!line.empty() and line.back() == '\r') {
        line.pop_back();
      }

      bool const only_ws =
          std::ranges::all_of(line, [](unsigned char c) { return std::isspace(c) != 0; });
      if (only_ws or line.empty()) {
        continue;
      }

      process_line(line);
    }

    if (!any_key_parsed_) {
      std::cerr << "ERROR: Configuration is empty or contains no valid keys.\n";
      if (!config_path_.empty()) {
        std::cerr << "Path: " << config_path_ << "\n";
      }
      std::exit(EXIT_FAILURE);
    }
  }

  void config_parser::process_line(std::string_view raw) {
    std::string_view key, values;
    split_key_values(raw, key, values);

    auto it = setter_table.find(key);
    if (it == setter_table.end()) {
      std::cerr << "Error: Unknown configuration key: [" << key << ":]\n";
      std::exit(EXIT_FAILURE);
    }

    auto fn         = it->second;
    any_key_parsed_ = true;
    (this->*fn)(values);
  }

  void config_parser::split_key_values(std::string_view line, std::string_view & key_out,
                                       std::string_view & values_out) {
    while (!line.empty() and std::isspace(static_cast<unsigned char>(line.front())) != 0) {
      line.remove_prefix(1);
    }
    std::size_t const colon_pos = line.find(':');
    if (colon_pos == std::string_view::npos) {
      // Obtener solo la primera palabra (etiqueta)
      std::string_view key_only = line;
      std::size_t const ws      = line.find_first_of(" \t");
      if (ws != std::string_view::npos) {
        key_only = line.substr(0, ws);
      }
      std::cerr << "Error: Unknown configuration key: [" << key_only << ":]\n";
      std::exit(EXIT_FAILURE);
    }
    if (colon_pos == 0) {
      std::cerr << "ERROR: Empty key before ':' in line -> '" << line << "'\n";
      std::exit(EXIT_FAILURE);
    }
    auto before = static_cast<unsigned char>(line[colon_pos - 1]);
    if (before == ' ' or before == '\t') {
      std::cerr << "ERROR: Whitespace between key and ':' is not allowed in line -> '" << line
                << "'\n";
      std::exit(EXIT_FAILURE);
    }
    key_out    = line.substr(0, colon_pos);
    values_out = line.substr(colon_pos + 1);
    while (!values_out.empty() and
           std::isspace(static_cast<unsigned char>(values_out.front())) != 0)
    {
      values_out.remove_prefix(1);
    }
    while (!values_out.empty() and std::isspace(static_cast<unsigned char>(values_out.back())) != 0)
    {
      values_out.remove_suffix(1);
    }
    if (values_out.empty()) {
      std::cerr << "ERROR: Missing value after ':' in line -> '" << line << "'\n";
      std::exit(EXIT_FAILURE);
    }
  }

  void config_parser::set_aspect_ratio(std::string_view values) {
    int w = 0, h = 0;

    if (!parse_int(values, w)) {
      print_invalid_value_error("aspect_ratio", "aspect_ratio: " + std::string(values));
    }

    if (!parse_int(values, h)) {
      print_invalid_value_error("aspect_ratio", "aspect_ratio: " + std::string(values));
    }

    if (w <= 0 or h <= 0) {
      print_invalid_value_error("aspect_ratio", "aspect_ratio: " + std::string(values));
    }

    if (!values.empty()) {
      print_extra_data_error("aspect_ratio", values);
    }

    data_.ar_w = w;
    data_.ar_h = h;
  }

  void config_parser::set_image_width(std::string_view values) {
    int width = 0;
    if (!parse_int(values, width) or width <= 0) {
      print_invalid_value_error("image_width", "image_width: " + std::string(values));
    }
    if (!values.empty()) {
      print_extra_data_error("image_width", values);
    }
    data_.image_width = width;
  }

  void config_parser::set_gamma(std::string_view values) {
    double g = 0.0;

    if (!parse_real(values, g) or g <= 0.0) {
      print_invalid_value_error("gamma", "gamma: " + std::string(values));
    }
    if (!values.empty()) {
      print_extra_data_error("gamma", values);
    }
    data_.gamma = g;
  }

  void config_parser::set_cam_position(std::string_view values) {
    std::array<double, 3> tmp{};
    auto result = parse_vec3(values, tmp);

    switch (result) {
      case Vec3ParseResult::OK: data_.cam_pos = tmp; break;
      case Vec3ParseResult::INVALID:
        print_invalid_value_error("camera_position", "camera_position: " + std::string(values));
        break;
      case Vec3ParseResult::EXTRA: print_extra_data_error("camera_position", values); break;
    }
  }

  void config_parser::set_cam_target(std::string_view values) {
    std::array<double, 3> tmp{};
    auto result = parse_vec3(values, tmp);

    switch (result) {
      case Vec3ParseResult::OK: data_.cam_tgt = tmp; break;
      case Vec3ParseResult::INVALID:
        print_invalid_value_error("camera_target", "camera_target: " + std::string(values));
        break;
      case Vec3ParseResult::EXTRA: print_extra_data_error("camera_target", values); break;
    }
  }

  void config_parser::set_cam_north(std::string_view values) {
    std::array<double, 3> tmp{};
    auto result = parse_vec3(values, tmp);

    switch (result) {
      case Vec3ParseResult::OK: data_.cam_up = tmp; break;
      case Vec3ParseResult::INVALID:
        print_invalid_value_error("camera_north", "camera_north: " + std::string(values));
        break;
      case Vec3ParseResult::EXTRA: print_extra_data_error("camera_north", values); break;
    }
  }

  void config_parser::set_fov(std::string_view values) {
    double fov = 0.0;
    if (!parse_real(values, fov) or fov <= 0.0 or fov >= 180.0) {
      print_invalid_value_error("field_of_view", "field_of_view: " + std::string(values));
    }
    if (!values.empty()) {
      print_extra_data_error("field_of_view", values);
    }
    data_.fov = fov;
  }

  void config_parser::set_spp(std::string_view values) {
    int spp = 0;
    if (!parse_int(values, spp) or spp <= 0) {
      print_invalid_value_error("samples_per_pixel", "samples_per_pixel: " + std::string(values));
    }
    if (!values.empty()) {
      print_extra_data_error("samples_per_pixel", values);
    }
    data_.samples_per_pixel = spp;
  }

  void config_parser::set_max_depth(std::string_view values) {
    int depth = 0;
    if (!parse_int(values, depth) or depth <= 0) {
      print_invalid_value_error("max_depth", "max_depth: " + std::string(values));
    }
    if (!values.empty()) {
      print_extra_data_error("max_depth", values);
    }
    data_.max_depth = depth;
  }

  void config_parser::set_mat_seed(std::string_view values) {
    int seed = 0;
    if (!parse_int(values, seed) or seed <= 0) {
      print_invalid_value_error("material_rng_seed", "material_rng_seed: " + std::string(values));
    }
    if (!values.empty()) {
      print_extra_data_error("material_rng_seed", values);
    }
    data_.material_rng_seed = seed;
  }

  void config_parser::set_ray_seed(std::string_view values) {
    int seed = 0;
    if (!parse_int(values, seed) or seed <= 0) {
      print_invalid_value_error("ray_rng_seed", "ray_rng_seed: " + std::string(values));
    }
    if (!values.empty()) {
      print_extra_data_error("ray_rng_seed", values);
    }
    data_.ray_rng_seed = seed;
  }

  void config_parser::set_bg_dark(std::string_view values) {
    std::array<double, 3> v{};
    auto result = parse_vec3(values, v);

    switch (result) {
      case Vec3ParseResult::OK:
        if (std::ranges::any_of(v, [](double c) { return c < 0.0 or c > 1.0; })) {
          print_invalid_value_error("background_dark_color",
                                    "background_dark_color: " + std::string(values));
        }
        data_.bg_dark = v;
        break;

      case Vec3ParseResult::INVALID:
        print_invalid_value_error("background_dark_color",
                                  "background_dark_color: " + std::string(values));
        break;

      case Vec3ParseResult::EXTRA: print_extra_data_error("background_dark_color", values); break;
    }
  }

  void config_parser::set_bg_light(std::string_view values) {
    std::array<double, 3> v{};
    auto result = parse_vec3(values, v);

    switch (result) {
      case Vec3ParseResult::OK:
        if (std::ranges::any_of(v, [](double c) { return c < 0.0 or c > 1.0; })) {
          print_invalid_value_error("background_light_color",
                                    "background_light_color: " + std::string(values));
        }
        data_.bg_light = v;
        break;

      case Vec3ParseResult::INVALID:
        print_invalid_value_error("background_light_color",
                                  "background_light_color: " + std::string(values));
        break;

      case Vec3ParseResult::EXTRA: print_extra_data_error("background_light_color", values); break;
    }
  }

  std::optional<cfg_key> to_key(std::string_view name) {
    static std::unordered_map<std::string_view, cfg_key> const key_map = {
      {          "aspect_ratio",           cfg_key::ASPECT_RATIO},
      {           "image_width",            cfg_key::IMAGE_WIDTH},
      {                 "gamma",                  cfg_key::GAMMA},
      {       "camera_position",        cfg_key::CAMERA_POSITION},
      {         "camera_target",          cfg_key::CAMERA_TARGET},
      {          "camera_north",           cfg_key::CAMERA_NORTH},
      {         "field_of_view",          cfg_key::FIELD_OF_VIEW},
      {     "samples_per_pixel",      cfg_key::SAMPLES_PER_PIXEL},
      {             "max_depth",              cfg_key::MAX_DEPTH},
      {     "material_rng_seed",      cfg_key::MATERIAL_RNG_SEED},
      {          "ray_rng_seed",           cfg_key::RAY_RNG_SEED},
      { "background_dark_color",  cfg_key::BACKGROUND_DARK_COLOR},
      {"background_light_color", cfg_key::BACKGROUND_LIGHT_COLOR}
    };

    if (auto it = key_map.find(name); it != key_map.end()) {
      return it->second;
    }

    return std::nullopt;
  }

}  // namespace render
