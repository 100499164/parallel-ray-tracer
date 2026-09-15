#include "color.hpp"
#include "figures/sphere.hpp"
#include "image.hpp"
#include "infocolision.hpp"
#include "materials/metal.hpp"
#include "materials_render/matte_render.hpp"
#include "materials_render/metal_render.hpp"
#include "materials_render/refractive_render.hpp"
#include "ray.hpp"
#include "rayTracer.hpp"
#include "sceneRender.hpp"
#include "vector.hpp"
#include <cmath>
#include <cstddef>
#include <gtest/gtest.h>
#include <memory>
#include <vector>

using namespace render;

// TestableRayTracer: exposes protected methods for testing
class TestableRayTracer : public RayTracer {
public:
  using RayTracer::RayTracer;  // Inherit constructors

  // Expose protected methods as public for testing
  using RayTracer::backgroundColor;
  using RayTracer::handleMaterialScatter;
  using RayTracer::traceRay;
  using RayTracer::writeColor;
};

// Simple Image implementation for testing
class TestImage : public Image {
public:
  TestImage(size_t w, size_t h) : width(w), height(h) {
    data = std::vector<Color>(w * h, Color(0, 0, 0));
  }

  void setPixel(size_t x, size_t y, Color const & color) override { data[y * width + x] = color; }

  [[nodiscard]] Color getPixel(size_t x, size_t y) const override { return data[y * width + x]; }

  [[nodiscard]] size_t getWidth() const override { return width; }

  [[nodiscard]] size_t getHeight() const override { return height; }

private:
  size_t width;
  size_t height;
  std::vector<Color> data;
};

TEST(RayTracerTest, LightBackgroundColor) {
  // Create RayTracer with known background colors
  render::Color const light(1.0, 1.0, 1.0);  // White
  render::Color const dark(0.0, 0.0, 0.0);   // Black
  TestableRayTracer const rayTracer(10, 1, light, dark, 12'345, 67'890);

  // Create ray pointing downward (y = -1)
  render::vector const origin(0, 0, 0);
  render::vector const direction(0, -1, 0);
  render::Ray const ray(origin, direction);

  // Get background color
  render::Color const result = rayTracer.backgroundColor(ray);

  // When ray points down (y = -1), t = 0, so we expect BACKGROUND_LIGHT
  EXPECT_DOUBLE_EQ(result.r, light.r);
  EXPECT_DOUBLE_EQ(result.g, light.g);
  EXPECT_DOUBLE_EQ(result.b, light.b);
}

TEST(RayTracerTest, DarkBackgroundColor) {
  // Create RayTracer with known background colors
  render::Color const light(1.0, 1.0, 1.0);  // White
  render::Color const dark(0.0, 0.0, 0.0);   // Black
  TestableRayTracer const rayTracer(10, 1, light, dark, 12'345, 67'890);

  // Create ray pointing upward (y = 1)
  render::vector const origin(0, 0, 0);
  render::vector const direction(0, 1, 0);
  render::Ray const ray(origin, direction);

  // Get background color
  render::Color const result = rayTracer.backgroundColor(ray);

  // When ray points down (y = -1), t = 0, so we expect BACKGROUND_LIGHT
  EXPECT_DOUBLE_EQ(result.r, dark.r);
  EXPECT_DOUBLE_EQ(result.g, dark.g);
  EXPECT_DOUBLE_EQ(result.b, dark.b);
}

TEST(RayTracerTest, GradientgroundColor) {
  // Create RayTracer with known background colors
  render::Color const light(1.0, 1.0, 1.0);  // White
  render::Color const dark(0.0, 0.0, 0.0);   // Black
  TestableRayTracer const rayTracer(10, 1, light, dark, 12'345, 67'890);

  // Create ray pointing upward (y = 1)
  render::vector const origin(0, 0, 0);
  render::vector const direction(0, 0, 1);
  render::Ray const ray(origin, direction);

  // Get background color
  render::Color const result = rayTracer.backgroundColor(ray);

  // When ray points down (y = -1), t = 0, so we expect BACKGROUND_LIGHT
  EXPECT_DOUBLE_EQ(result.r, 0.5);
  EXPECT_DOUBLE_EQ(result.g, 0.5);
  EXPECT_DOUBLE_EQ(result.b, 0.5);
}

TEST(RayTracerTest, WriteColorMethod) {
  // Create RayTracer with SAMPLES_PIXEL=100, INV_GAMMA=0.5 (gamma=2.0)
  render::Color const light(1.0, 1.0, 1.0);
  render::Color const dark(0.0, 0.0, 0.0);
  TestableRayTracer const rayTracer(10, 100, light, dark, 12'345, 67'890, 2.0);

  // Create a test image
  TestImage imagen(10, 10);

  // Accumulated color before averaging and gamma correction
  render::Color const color_acumulado(50.0, 100.0, 150.0);

  // Call writeColor
  rayTracer.writeColor(imagen, 5, 5, color_acumulado);

  // Get the written pixel
  render::Color const result = imagen.getPixel(5, 5);

  // Expected values
  EXPECT_NEAR(result.r, std::sqrt(0.5), 1e-10);
  EXPECT_NEAR(result.g, 1.0, 1e-10);
  EXPECT_NEAR(result.b, std::sqrt(1.5), 1e-10);
}

TEST(RayTracerTest, MatteMaterialScatter) {
  // Create RayTracer
  render::Color const light(1.0, 1.0, 1.0);
  render::Color const dark(0.0, 0.0, 0.0);
  TestableRayTracer rayTracer(10, 100, light, dark, 12'345, 67'890);

  // Create Matte material with albedo (red color)
  render::Color const albedo(0.8, 0.2, 0.2);
  Matte_render matte(albedo);

  // Create collision info
  InfoColision info;
  info.t          = 1.0;
  info.p          = render::vector(0, 0, 0);  // Collision point
  info.normal     = render::vector(0, 1, 0);  // Normal pointing up
  info.front_face = true;
  info.mat_ptr    = &matte;

  // Create incoming ray
  render::vector const ray_origin(0, -1, 0);
  render::vector const ray_direction(0, 1, 0);
  render::Ray const r_in(ray_origin, ray_direction);

  // Initial accumulated attenuation (white)
  render::Color atenuacion_acumulada(1.0, 1.0, 1.0);

  // Output ray
  render::Ray ray_out;

  // Call handleMaterialScatter
  bool const result = rayTracer.handleMaterialScatter(r_in, info, atenuacion_acumulada, ray_out);

  // 1. Check that it returns true (matte always scatters)
  EXPECT_TRUE(result);

  // 2. Check ray direction: should be normalized and pointing in hemisphere of normal
  EXPECT_NEAR(ray_out.direccion.magnitude(), 1.0, 1e-10);  // Normalized
  double const dot_product = render::vector::dot(ray_out.direccion, info.normal);
  EXPECT_GT(dot_product, 0.0);  // Scattered ray should point in same hemisphere as normal

  // 3. Verify ray origin is at collision point
  EXPECT_DOUBLE_EQ(ray_out.origen.x, info.p.x);
  EXPECT_DOUBLE_EQ(ray_out.origen.y, info.p.y);
  EXPECT_DOUBLE_EQ(ray_out.origen.z, info.p.z);

  // 4. Check accumulated attenuation
  EXPECT_DOUBLE_EQ(atenuacion_acumulada.r, 0.8);
  EXPECT_DOUBLE_EQ(atenuacion_acumulada.g, 0.2);
  EXPECT_DOUBLE_EQ(atenuacion_acumulada.b, 0.2);
}

TEST(RayTracerTest, MetalMaterialDoesScatter) {
  // Create RayTracer
  render::Color const light(1.0, 1.0, 1.0);
  render::Color const dark(0.0, 0.0, 0.0);
  TestableRayTracer rayTracer(10, 100, light, dark, 12'345, 67'890);

  // Create Metal material with albedo (silver color) and small fuzz
  render::Color const albedo(0.9, 0.9, 0.9);
  double const fuzz = 0.1;
  Metal_render metal(albedo, fuzz);

  // Create collision info
  // Ray coming from above hitting a horizontal surface
  InfoColision info;
  info.t          = 1.0;
  info.p          = render::vector(0, 0, 0);  // Collision point at origin
  info.normal     = render::vector(0, 1, 0);  // Normal pointing up
  info.front_face = true;
  info.mat_ptr    = &metal;

  // Create incoming ray: coming from above at 45 degrees
  // Origin at (1, 1, 0), pointing toward origin (slightly downward)
  render::vector const ray_origin(1, 1, 0);
  render::vector const ray_direction =
      (info.p - ray_origin).normalized();  // Direction toward collision point
  render::Ray const r_in(ray_origin, ray_direction);

  // Initial accumulated attenuation (white)
  render::Color atenuacion_acumulada(1.0, 1.0, 1.0);

  // Output ray
  render::Ray ray_out;

  // Call handleMaterialScatter
  bool const result = rayTracer.handleMaterialScatter(r_in, info, atenuacion_acumulada, ray_out);

  // 1. Check that it returns true (metal scatters when reflected ray points outward)
  EXPECT_TRUE(result);

  // 2. Check ray direction: should be normalized
  EXPECT_NEAR(ray_out.direccion.magnitude(), 1.0, 1);  // Normalized

  // 3. Check that scattered ray points in same hemisphere as normal
  double const dot_product = render::vector::dot(ray_out.direccion, info.normal);
  EXPECT_GT(dot_product, 0.0);  // Reflected ray should point upward (same hemisphere as normal)

  // 4. Verify ray origin is at collision point
  EXPECT_DOUBLE_EQ(ray_out.origen.x, info.p.x);
  EXPECT_DOUBLE_EQ(ray_out.origen.y, info.p.y);
  EXPECT_DOUBLE_EQ(ray_out.origen.z, info.p.z);

  // 5. Check accumulated attenuation: should be multiplied by albedo
  EXPECT_DOUBLE_EQ(atenuacion_acumulada.r, 0.9);
  EXPECT_DOUBLE_EQ(atenuacion_acumulada.g, 0.9);
  EXPECT_DOUBLE_EQ(atenuacion_acumulada.b, 0.9);
}

TEST(RayTracerTest, RefractiveMaterialScatter) {
  // Create RayTracer
  render::Color const light(1.0, 1.0, 1.0);
  render::Color const dark(0.0, 0.0, 0.0);
  TestableRayTracer rayTracer(10, 100, light, dark, 12'345, 67'890);

  // Create Refractive material (glass with index of refraction 1.5)
  double const index_of_refraction = 1.5;
  Refractive_render refractive(index_of_refraction);

  // Create collision info
  InfoColision info;
  info.t          = 1.0;
  info.p          = render::vector(0, 0, 0);  // Collision point at origin
  info.normal     = render::vector(0, 1, 0);  // Normal pointing up
  info.front_face = true;                     // Ray entering the material from outside
  info.mat_ptr    = &refractive;

  // Create incoming ray: coming from above at 45 degrees
  render::vector const ray_origin(1, 1, 0);
  render::vector const ray_direction = (info.p - ray_origin).normalized();
  render::Ray const r_in(ray_origin, ray_direction);

  // Initial accumulated attenuation (white)
  render::Color atenuacion_acumulada(1.0, 1.0, 1.0);

  // Output ray
  render::Ray ray_out;

  // Call handleMaterialScatter
  bool const result = rayTracer.handleMaterialScatter(r_in, info, atenuacion_acumulada, ray_out);

  // 1. Check that it returns true (refractive materials always scatter)
  EXPECT_TRUE(result);

  // 2. Check ray direction: should be normalized
  EXPECT_NEAR(ray_out.direccion.magnitude(), 1.0, 1e-10);  // Normalized

  // 3. Verify ray origin is at collision point
  EXPECT_DOUBLE_EQ(ray_out.origen.x, info.p.x);
  EXPECT_DOUBLE_EQ(ray_out.origen.y, info.p.y);
  EXPECT_DOUBLE_EQ(ray_out.origen.z, info.p.z);

  // 4. Check accumulated attenuation: refractive materials don't attenuate
  EXPECT_DOUBLE_EQ(atenuacion_acumulada.r, 1.0);
  EXPECT_DOUBLE_EQ(atenuacion_acumulada.g, 1.0);
  EXPECT_DOUBLE_EQ(atenuacion_acumulada.b, 1.0);
}

TEST(RayTracerTest, TraceRayNoHit) {
  // Create RayTracer with known background colors
  render::Color const light(1.0, 1.0, 1.0);
  render::Color const dark(0.0, 0.0, 0.0);
  TestableRayTracer rayTracer(10, 100, light, dark, 12'345, 67'890);

  // Create a scene with a metal sphere at origin with radius 2
  SceneRender escena;

  // Create metal material (scene material)
  auto metal_material            = std::make_shared<Metal>();
  metal_material->material_alias = "metal1";
  std::string metal_params       = "0.8 0.8 0.8 0.1";
  metal_material->validate_parameters(metal_params);

  // Create sphere figure
  auto sphere               = std::make_shared<Sphere>();
  std::string sphere_params = "0.0 0.0 0.0 2.0 metal1";
  sphere->validate_parameters(sphere_params);

  // Add sphere to scene using add_render_figure
  escena.add_render_figure(sphere, metal_material);

  // Create a ray that does not hit the sphere
  render::vector const ray_origin(0, 5, 0);
  render::vector const ray_direction(1, 0, 0);
  render::Ray const ray(ray_origin, ray_direction);

  // Call traceRay
  render::Color const result = rayTracer.traceRay(ray, escena);

  // Expected: background color at y=0 (horizontal ray)
  EXPECT_DOUBLE_EQ(result.r, 0.5);
  EXPECT_DOUBLE_EQ(result.g, 0.5);
  EXPECT_DOUBLE_EQ(result.b, 0.5);
}

TEST(RayTracerTest, TraceRayHitsSphereAndScatters) {
  // Create RayTracer with known background colors
  render::Color const light(1.0, 1.0, 1.0);
  render::Color const dark(0.0, 0.0, 0.0);
  TestableRayTracer rayTracer(10, 100, light, dark, 12'345, 67'890);

  // Create a scene with a metal sphere at origin with radius 2
  SceneRender escena;

  // Create metal material (scene material)
  auto metal_material            = std::make_shared<Metal>();
  metal_material->material_alias = "metal1";
  std::string metal_params       = "0.8 0.6 0.4 0.1";
  metal_material->validate_parameters(metal_params);

  // Create sphere figure
  auto sphere               = std::make_shared<Sphere>();
  std::string sphere_params = "0.0 0.0 0.0 2.0 metal1";
  sphere->validate_parameters(sphere_params);

  // Add sphere to scene using add_render_figure
  escena.add_render_figure(sphere, metal_material);

  // Create a ray that HITS the sphere directly from above
  // Ray origin at (0, 3, 0) pointing downward toward center
  render::vector const ray_origin(0, 3, 0);      // Above the sphere (sphere has radius 2)
  render::vector const ray_direction(0, -1, 0);  // Pointing straight down
  render::Ray const ray(ray_origin, ray_direction);

  // Call traceRay
  render::Color const result = rayTracer.traceRay(ray, escena);

  // Since the reflected ray will point upward and hit dark background,
  // the result should be close to black
  EXPECT_TRUE(result.r >= 0.0 and result.r <= 0.1);  // Nearly black due to dark background
  EXPECT_TRUE(result.g >= 0.0 and result.g <= 0.1);
  EXPECT_TRUE(result.b >= 0.0 and result.b <= 0.1);
}

TEST(RayTracerTest, TraceRayReachMaxDepth) {
  // Create RayTracer with known background colors and SMALL MAX_DEPTH for testing
  render::Color const light(1.0, 1.0, 1.0);
  render::Color const dark(0.0, 0.0, 0.0);
  // Use smaller MAX_DEPTH to make test faster (default is 50, we'll use 5)
  TestableRayTracer rayTracer(5, 100, light, dark, 12'345, 67'890);

  // Create a scene with TWO metal spheres positioned to create multiple bounces
  SceneRender escena;

  // Create metal material with NO fuzz (perfect reflection, always scatters)
  auto metal_material            = std::make_shared<Metal>();
  metal_material->material_alias = "metal1";
  std::string metal_params       = "0.9 0.9 0.9 0.0";  // High reflectivity, no fuzz
  metal_material->validate_parameters(metal_params);

  // Create first sphere at origin
  auto sphere1               = std::make_shared<Sphere>();
  std::string sphere1_params = "0.0 0.0 0.0 2.0 metal1";
  sphere1->validate_parameters(sphere1_params);

  // Create second sphere positioned to catch reflections from first sphere
  auto sphere2               = std::make_shared<Sphere>();
  std::string sphere2_params = "0.0 10.0 0.0 2.0 metal1";  // Above first sphere
  sphere2->validate_parameters(sphere2_params);

  // Add both spheres to scene
  escena.add_render_figure(sphere1, metal_material);
  escena.add_render_figure(sphere2, metal_material);

  // Create a ray that hits the first sphere and bounces between both
  // Ray from above pointing down toward first sphere
  render::vector const ray_origin(0, 5, 0);
  render::vector const ray_direction(0, -1, 0);  // Pointing straight down
  render::Ray const ray(ray_origin, ray_direction);

  // Call traceRay
  render::Color const result = rayTracer.traceRay(ray, escena);

  // Verify the result is exactly black (max depth reached)
  EXPECT_DOUBLE_EQ(result.r, 0.0);
  EXPECT_DOUBLE_EQ(result.g, 0.0);
  EXPECT_DOUBLE_EQ(result.b, 0.0);
}
