#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace render {

  enum class cfg_key : std::uint8_t {
    ASPECT_RATIO,
    IMAGE_WIDTH,
    GAMMA,
    CAMERA_POSITION,
    CAMERA_TARGET,
    CAMERA_NORTH,
    FIELD_OF_VIEW,
    SAMPLES_PER_PIXEL,
    MAX_DEPTH,
    MATERIAL_RNG_SEED,
    RAY_RNG_SEED,
    BACKGROUND_DARK_COLOR,
    BACKGROUND_LIGHT_COLOR
  };

  std::optional<cfg_key> to_key(std::string_view name);

  struct config_data {
    static constexpr int DEFAULT_AR_W        = 16;
    static constexpr int DEFAULT_AR_H        = 9;
    static constexpr int DEFAULT_IMAGE_WIDTH = 1'920;
    static constexpr double DEFAULT_GAMMA    = 2.2;
    static constexpr std::array<double, 3> DEFAULT_CAM_POS{0.0, 0.0, -10.0};
    static constexpr std::array<double, 3> DEFAULT_CAM_TGT{0.0, 0.0, 0.0};
    static constexpr std::array<double, 3> DEFAULT_CAM_UP{0.0, 1.0, 0.0};
    static constexpr double DEFAULT_FOV    = 90.0;
    static constexpr int DEFAULT_SPP       = 20;
    static constexpr int DEFAULT_MAX_DEPTH = 5;
    static constexpr int DEFAULT_MAT_SEED  = 13;
    static constexpr int DEFAULT_RAY_SEED  = 19;
    static constexpr std::array<double, 3> DEFAULT_BG_DARK{0.25, 0.5, 1.0};
    static constexpr std::array<double, 3> DEFAULT_BG_LIGHT{1.0, 1.0, 1.0};

    int ar_w        = DEFAULT_AR_W;
    int ar_h        = DEFAULT_AR_H;
    int image_width = DEFAULT_IMAGE_WIDTH;
    double gamma    = DEFAULT_GAMMA;

    std::array<double, 3> cam_pos = DEFAULT_CAM_POS;
    std::array<double, 3> cam_tgt = DEFAULT_CAM_TGT;
    std::array<double, 3> cam_up  = DEFAULT_CAM_UP;

    double fov            = DEFAULT_FOV;
    int samples_per_pixel = DEFAULT_SPP;
    int max_depth         = DEFAULT_MAX_DEPTH;

    int material_rng_seed = DEFAULT_MAT_SEED;
    int ray_rng_seed      = DEFAULT_RAY_SEED;

    std::array<double, 3> bg_dark  = DEFAULT_BG_DARK;
    std::array<double, 3> bg_light = DEFAULT_BG_LIGHT;
  };

  class config_parser {
  public:
    explicit config_parser(std::string config_path)
        : config_path_(std::move(config_path)), input_stream_(nullptr) { }

    explicit config_parser(std::istream & stream) : input_stream_(&stream) { }

    ~config_parser()                                     = default;
    config_parser(config_parser const &)                 = delete;
    config_parser & operator=(config_parser const &)     = delete;
    config_parser(config_parser &&) noexcept             = default;
    config_parser & operator=(config_parser &&) noexcept = default;

    void parse();

    [[nodiscard]] config_data const & data() const noexcept { return data_; }

  private:
    std::string config_path_;
    std::istream * input_stream_;
    config_data data_{};

    using setter_ptr = void (config_parser::*)(std::string_view);

    static std::unordered_map<std::string_view, setter_ptr> const setter_table;

    void process_line(std::string_view raw);
    static void split_key_values(std::string_view line, std::string_view & key_out,
                                 std::string_view & values_out);

    void set_aspect_ratio(std::string_view values);
    void set_image_width(std::string_view values);
    void set_gamma(std::string_view values);
    void set_cam_position(std::string_view values);
    void set_cam_target(std::string_view values);
    void set_cam_north(std::string_view values);
    void set_fov(std::string_view values);
    void set_spp(std::string_view values);
    void set_max_depth(std::string_view values);
    void set_mat_seed(std::string_view values);
    void set_ray_seed(std::string_view values);
    void set_bg_dark(std::string_view values);
    void set_bg_light(std::string_view values);

    bool any_key_parsed_ = false;
  };

}  // namespace render

#endif  // CONFIG_PARSER_HPP
