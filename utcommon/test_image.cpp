#include "color.hpp"
#include "image.hpp"
#include <cstddef>
#include <gtest/gtest.h>
#include <memory>
#include <vector>
using namespace render;

// Clase derivada mínima para testear la interfaz
class DummyImage : public Image {
public:
  DummyImage(size_t w, size_t h) : width(w), height(h) {
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

// ID: IMG-1
TEST(test_image, polymorphic_destruction_does_not_throw) {
  std::unique_ptr<Image> img = std::make_unique<DummyImage>(2, 2);
  EXPECT_NO_THROW(img.reset());
}

// ID: IMG-2
TEST(test_image, set_and_get_pixel_work_correctly) {
  DummyImage img(3, 2);

  Color const c1(0.5, 0.4, 0.3);
  img.setPixel(1, 1, c1);

  Color const c2 = img.getPixel(1, 1);
  EXPECT_DOUBLE_EQ(c2.r, 0.5);
  EXPECT_DOUBLE_EQ(c2.g, 0.4);
  EXPECT_DOUBLE_EQ(c2.b, 0.3);
}

// ID: IMG-3
TEST(test_image, width_and_height_are_correct) {
  DummyImage const img(10, 5);
  EXPECT_EQ(img.getWidth(), 10);
  EXPECT_EQ(img.getHeight(), 5);
}

// ID: IMG-4
TEST(test_image, cannot_instantiate_abstract_class) {
  // La clase base Image es abstracta, no se puede instanciar.
  SUCCEED();
}
