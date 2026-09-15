#include "camera.hpp"
#include "vector.hpp"
#include <cmath>
#include <numbers>

namespace render {

  camera::camera(camera_settings const & settings)
      : position_{settings.position}, target_{settings.target}, north_{settings.north.normalized()},
        fov_deg_{settings.fov_deg}, width_{settings.image_width}, height_{settings.image_height},
        rng_{settings.ray_rng_seed}, dist_{-0.5, 0.5} {
    compute_geometry();
  }

  void camera::compute_geometry() {
    compute_focal();
    compute_window();
    compute_basis();
    compute_deltas();
  }

  void camera::compute_focal() {
    v_f_ = position_ - target_;
    d_f_ = v_f_.magnitude();
  }

  void camera::compute_window() {
    double const alpha_rad = fov_deg_ * (std::numbers::pi / 180.0);
    h_p_                   = 2.0 * std::tan(alpha_rad / 2.0) * d_f_;
    w_p_                   = h_p_ * static_cast<double>(width_) / static_cast<double>(height_);
  }

  void camera::compute_basis() {
    vector const v_f_norm = v_f_.normalized();
    u_                    = vector::cross(north_, v_f_norm).normalized();
    v_                    = vector::cross(v_f_norm, u_);
  }

  void camera::compute_deltas() {
    p_h_     = u_ * w_p_;
    p_v_     = v_ * (-h_p_);
    delta_x_ = p_h_ / static_cast<double>(width_);
    delta_y_ = p_v_ / static_cast<double>(height_);
    origin_  = position_ - v_f_ - (p_h_ + p_v_) * 0.5 + (delta_x_ + delta_y_) * 0.5;
  }

  vector camera::pixel_center(int row, int col) const noexcept {
    return origin_ + delta_x_ * static_cast<double>(col) + delta_y_ * static_cast<double>(row);
  }

}  // namespace render
