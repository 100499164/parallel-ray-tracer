#include "camera.hpp"
#include "gtest/gtest.h"
#include <cmath>
#include <numbers>

TEST(CameraTest, BasicInitialization) {
  render::camera_settings const cfg{.position     = render::vector(0.0, 0.0, -10.0),
                                    .target       = render::vector(0.0, 0.0, 0.0),
                                    .north        = render::vector(0.0, 1.0, 0.0),
                                    .fov_deg      = 90.0,
                                    .image_width  = 4,
                                    .image_height = 2};

  render::camera const cam(cfg);

  EXPECT_NEAR(cam.fov(), 90.0, 1e-9);
  EXPECT_EQ(cam.width(), 4);
  EXPECT_EQ(cam.height(), 2);

  render::vector const pos = cam.position();
  EXPECT_NEAR(pos.x, 0.0, 1e-9);
  EXPECT_NEAR(pos.y, 0.0, 1e-9);
  EXPECT_NEAR(pos.z, -10.0, 1e-9);
}

TEST(CameraTest, FocalVectorEasyCase) {
  render::camera_settings const cfg{render::vector(0.0, 0.0, -10.0),
                                    render::vector(0.0, 0.0, 0.0),
                                    render::vector(0.0, 1.0, 0.0),
                                    90.0,
                                    4,
                                    2};

  render::camera const cam(cfg);
  render::vector const vf_expected(0.0, 0.0, -10.0);
  render::vector const vf = cam.focal();

  EXPECT_NEAR(vf.x, vf_expected.x, 1e-9);
  EXPECT_NEAR(vf.y, vf_expected.y, 1e-9);
  EXPECT_NEAR(vf.z, vf_expected.z, 1e-9);

  // Magnitud
  EXPECT_NEAR(vf.magnitude(), 10.0, 1e-9);
}

TEST(CameraTest, FocalVectorGeneralCase) {
  render::camera_settings const cfg{render::vector(3.0, 4.0, -2.0),
                                    render::vector(-1.0, 2.0, 5.0),
                                    render::vector(0.0, 1.0, 0.0),
                                    60.0,
                                    8,
                                    6};

  render::camera const cam(cfg);
  render::vector const vf_expected(4.0, 2.0, -7.0);
  render::vector const vf = cam.focal();

  EXPECT_NEAR(vf.x, vf_expected.x, 1e-9);
  EXPECT_NEAR(vf.y, vf_expected.y, 1e-9);
  EXPECT_NEAR(vf.z, vf_expected.z, 1e-9);

  double const expected_magnitude = std::sqrt(69.0);
  EXPECT_NEAR(vf.magnitude(), expected_magnitude, 1e-9);
}

TEST(CameraTest, ProjectionWindowHeightIsCorrect) {
  render::camera_settings const cfg{render::vector(0.0, 0.0, -10.0),
                                    render::vector(0.0, 0.0, 0.0),
                                    render::vector(0.0, 1.0, 0.0),
                                    60.0,
                                    4,
                                    2};

  render::camera const cam(cfg);

  double const df          = cam.focal().magnitude();
  double const alpha_rad   = 60.0 * (std::numbers::pi / 180.0);
  double const expected_hp = 2.0 * std::tan(alpha_rad / 2.0) * df;
  double const hp          = cam.window_height();

  EXPECT_NEAR(hp, expected_hp, 1e-9);
}

TEST(CameraTest, ProjectionWindowWidthIsCorrect) {
  render::camera_settings const cfg{render::vector(0.0, 0.0, -10.0),
                                    render::vector(0.0, 0.0, 0.0),
                                    render::vector(0.0, 1.0, 0.0),
                                    90.0,
                                    16,
                                    9};

  render::camera const cam(cfg);

  double const df          = cam.focal().magnitude();
  double const alpha_rad   = 90.0 * (std::numbers::pi / 180.0);
  double const expected_hp = 2.0 * std::tan(alpha_rad / 2.0) * df;
  double const expected_wp =
      expected_hp * static_cast<double>(cam.width()) / static_cast<double>(cam.height());
  double const wp = cam.window_width();

  EXPECT_NEAR(wp, expected_wp, 1e-9);
}

TEST(CameraTest, ProjectionBasisVectorsAreCorrect) {
  render::camera_settings const cfg{render::vector(0.0, 0.0, -10.0),
                                    render::vector(0.0, 0.0, 0.0),
                                    render::vector(0.0, 1.0, 0.0),
                                    90.0,
                                    4,
                                    2};

  render::camera const cam(cfg);
  render::vector const u_expected(-1.0, 0.0, 0.0);
  render::vector const v_expected(0.0, 1.0, 0.0);
  render::vector const u = cam.u();
  render::vector const v = cam.v();

  EXPECT_NEAR(u.x, u_expected.x, 1e-9);
  EXPECT_NEAR(u.y, u_expected.y, 1e-9);
  EXPECT_NEAR(u.z, u_expected.z, 1e-9);

  EXPECT_NEAR(v.x, v_expected.x, 1e-9);
  EXPECT_NEAR(v.y, v_expected.y, 1e-9);
  EXPECT_NEAR(v.z, v_expected.z, 1e-9);

  EXPECT_NEAR(u.magnitude(), 1.0, 1e-9);
  EXPECT_NEAR(v.magnitude(), 1.0, 1e-9);
}

TEST(CameraTest, ProjectionPlaneVectorsAreCorrect) {
  render::camera_settings const cfg{render::vector(0.0, 0.0, -10.0),
                                    render::vector(0.0, 0.0, 0.0),
                                    render::vector(0.0, 1.0, 0.0),
                                    90.0,
                                    16,
                                    9};

  render::camera const cam(cfg);

  double const df        = cam.focal().magnitude();
  double const alpha_rad = 90.0 * (std::numbers::pi / 180.0);
  double const hp        = 2.0 * std::tan(alpha_rad / 2.0) * df;
  double const wp = hp * static_cast<double>(cam.width()) / static_cast<double>(cam.height());

  render::vector const u_expected(-1.0, 0.0, 0.0);
  render::vector const v_expected(0.0, 1.0, 0.0);
  render::vector const p_h_expected = u_expected * wp;
  render::vector const p_v_expected = v_expected * (-hp);
  render::vector const p_h          = cam.p_h();
  render::vector const p_v          = cam.p_v();

  EXPECT_NEAR(p_h.x, p_h_expected.x, 1e-9);
  EXPECT_NEAR(p_h.y, p_h_expected.y, 1e-9);
  EXPECT_NEAR(p_h.z, p_h_expected.z, 1e-9);

  EXPECT_NEAR(p_v.x, p_v_expected.x, 1e-9);
  EXPECT_NEAR(p_v.y, p_v_expected.y, 1e-9);
  EXPECT_NEAR(p_v.z, p_v_expected.z, 1e-9);

  EXPECT_NEAR(p_h.magnitude(), wp, 1e-9);
  EXPECT_NEAR(p_v.magnitude(), hp, 1e-9);

  EXPECT_NEAR(render::vector::dot(p_h, p_v), 0.0, 1e-9);
}

TEST(CameraTest, ProjectionWindowOriginIsCorrect) {
  render::camera_settings const cfg{render::vector(0.0, 0.0, -10.0),
                                    render::vector(0.0, 0.0, 0.0),
                                    render::vector(0.0, 1.0, 0.0),
                                    90.0,
                                    4,
                                    2};

  render::camera const cam(cfg);
  render::vector const P       = cam.position();
  render::vector const vf      = cam.focal();
  render::vector const p_h     = cam.p_h();
  render::vector const p_v     = cam.p_v();
  render::vector const delta_x = cam.delta_x();
  render::vector const delta_y = cam.delta_y();

  render::vector const O_expected = P - vf - (p_h + p_v) * 0.5 + (delta_x + delta_y) * 0.5;

  render::vector const O = cam.origin();

  EXPECT_NEAR(O.x, O_expected.x, 1e-9);
  EXPECT_NEAR(O.y, O_expected.y, 1e-9);
  EXPECT_NEAR(O.z, O_expected.z, 1e-9);
}

TEST(CameraTest, DeltaValuesAreComputed) {
  render::camera_settings const cfg{render::vector(0.0, 0.0, -10.0),
                                    render::vector(0.0, 0.0, 0.0),
                                    render::vector(0.0, 1.0, 0.0),
                                    90.0,
                                    4,
                                    2};

  render::camera const cam(cfg);

  render::vector const dx = cam.delta_x();
  render::vector const dy = cam.delta_y();

  double const dx_len = dx.magnitude();
  double const dy_len = dy.magnitude();

  EXPECT_GT(dx_len, 0.0);
  EXPECT_GT(dy_len, 0.0);
  EXPECT_NEAR(dx_len / dy_len, 1.0, 1e-9);
}

TEST(CameraTest, PixelCenterAtOrigin) {
  render::camera_settings const cfg{render::vector(0.0, 0.0, -10.0),
                                    render::vector(0.0, 0.0, 0.0),
                                    render::vector(0.0, 1.0, 0.0),
                                    90.0,
                                    4,
                                    2};

  render::camera const cam(cfg);

  render::vector const p = cam.pixel_center(0, 0);
  render::vector const o = cam.origin();

  EXPECT_NEAR(p.x, o.x, 1e-9);
  EXPECT_NEAR(p.y, o.y, 1e-9);
  EXPECT_NEAR(p.z, o.z, 1e-9);
}

TEST(CameraTest, PixelCenterMovesRightAndDown) {
  render::camera_settings const cfg{render::vector(0.0, 0.0, -10.0),
                                    render::vector(0.0, 0.0, 0.0),
                                    render::vector(0.0, 1.0, 0.0),
                                    90.0,
                                    4,
                                    2};

  render::camera const cam(cfg);

  render::vector const p00 = cam.pixel_center(0, 0);
  render::vector const p01 = cam.pixel_center(0, 1);
  render::vector const p10 = cam.pixel_center(1, 0);

  render::vector const dx = p01 - p00;
  render::vector const dy = p10 - p00;

  EXPECT_LT(dx.x, 0.0);
  EXPECT_LT(dy.y, 0.0);
}

TEST(CameraTest, DeltaXAndDeltaYAreOrthogonal) {
  render::camera_settings const cfg{render::vector(0.0, 0.0, -10.0),
                                    render::vector(0.0, 0.0, 0.0),
                                    render::vector(0.0, 1.0, 0.0),
                                    90.0,
                                    4,
                                    2};

  render::camera const cam(cfg);

  render::vector const dx = cam.delta_x();
  render::vector const dy = cam.delta_y();

  double const dot = render::vector::dot(dx, dy);
  EXPECT_NEAR(dot, 0.0, 1e-9);
}

TEST(CameraTest, CenterOfScreenIsFrontOfCamera) {
  render::camera_settings const cfg{render::vector(0.0, 0.0, -10.0),
                                    render::vector(0.0, 0.0, 0.0),
                                    render::vector(0.0, 1.0, 0.0),
                                    90.0,
                                    4,
                                    2};

  render::camera const cam(cfg);

  int const mid_row          = cam.height() / 2;
  int const mid_col          = cam.width() / 2;
  render::vector const pixel = cam.pixel_center(mid_row, mid_col);

  EXPECT_GT(pixel.z, -10.0);
}
