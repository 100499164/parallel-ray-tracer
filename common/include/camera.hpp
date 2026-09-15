#ifndef RENDER_CAMERA_HPP
#define RENDER_CAMERA_HPP

#include "vector.hpp"
#include <cmath>
#include <random>

namespace render {

  // --- Configuración de la cámara ---
  struct camera_settings {
    vector position;
    vector target;
    vector north{0.0, 1.0, 0.0};
    double fov_deg{90.0};
    int image_width{1'920};
    int image_height{1'080};
    unsigned long long ray_rng_seed{19};
  };

  // --- Clase principal de cámara ---
  class camera {
  public:
    explicit camera(camera_settings const & settings);

    // --- Getters de lectura ---
    [[nodiscard]] double fov() const noexcept { return fov_deg_; }

    [[nodiscard]] int width() const noexcept { return width_; }

    [[nodiscard]] int height() const noexcept { return height_; }

    [[nodiscard]] vector position() const noexcept { return position_; }

    [[nodiscard]] vector const & origin() const noexcept { return origin_; }

    [[nodiscard]] vector const & horizontal() const noexcept { return p_h_; }

    [[nodiscard]] vector const & vertical() const noexcept { return p_v_; }

    [[nodiscard]] vector const & delta_x() const noexcept { return delta_x_; }

    [[nodiscard]] vector const & delta_y() const noexcept { return delta_y_; }

    [[nodiscard]] vector const & focal() const noexcept { return v_f_; }

    [[nodiscard]] double window_height() const noexcept { return h_p_; }

    [[nodiscard]] double window_width() const noexcept { return w_p_; }

    [[nodiscard]] vector const & u() const noexcept { return u_; }

    [[nodiscard]] vector const & v() const noexcept { return v_; }

    [[nodiscard]] vector const & p_h() const noexcept { return p_h_; }

    [[nodiscard]] vector const & p_v() const noexcept { return p_v_; }

    [[nodiscard]] vector pixel_center(int row, int col) const noexcept;

  private:
    // --- Parámetros base ---
    vector position_;
    vector target_;
    vector north_{0.0, 1.0, 0.0};
    double fov_deg_{90.0};
    int width_{0};
    int height_{0};

    // --- Variables geométricas ---
    vector v_f_;  // vector focal
    double d_f_{0.0};
    double h_p_{0.0};
    double w_p_{0.0};
    vector u_;  // eje horizontal del plano de imagen
    vector v_;  // eje vertical del plano de imagen

    // --- Plano de imagen ---
    vector p_h_;      // vector horizontal del plano
    vector p_v_;      // vector vertical del plano
    vector delta_x_;  // desplazamiento por píxel en X
    vector delta_y_;  // desplazamiento por píxel en Y
    vector origin_;   // esquina superior izquierda del plano

    // --- Generador de números aleatorios ---

    std::mt19937_64 rng_;
    std::uniform_real_distribution<double> dist_{-0.5, 0.5};

    // --- Métodos auxiliares ---
    void compute_geometry();
    void compute_focal();
    void compute_window();
    void compute_basis();
    void compute_deltas();

    [[nodiscard]] double random_offset() noexcept { return dist_(rng_); }
  };

}  // namespace render

#endif  // RENDER_CAMERA_HPP
