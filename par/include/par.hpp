#ifndef PAR_HPP
#define PAR_HPP

#include "color.hpp"
#include "image.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace render {

  class AOS : public Image {
  public:
    // Constructor that initializes the image with width and height
    AOS(size_t width, size_t height);

    // Calculates the 1D index in the pixel array for given (x, y) coordinates
    [[nodiscard]] size_t getIndex(size_t x, size_t y) const noexcept;

    // Sets the color of a specific pixel
    void setPixel(size_t x, size_t y, Color const & pixel) override;

    // Gets the color of a specific pixel
    [[nodiscard]] Color getPixel(size_t x, size_t y) const override;

    // Retrieves all red channel values (innefficient in AOS)
    [[nodiscard]] std::vector<uint8_t> getRedChannel() const;

    // Retrieves all green channel values (innefficient in AOS)
    [[nodiscard]] std::vector<uint8_t> getGreenChannel() const;

    // Retrieves all blue channel values (innefficient in AOS)
    [[nodiscard]] std::vector<uint8_t> getBlueChannel() const;

    [[nodiscard]] size_t getWidth() const noexcept override;
    [[nodiscard]] size_t getHeight() const noexcept override;

    // Converts the image to a string representation (for debugging)
    [[nodiscard]] std::string str() const;

    // Saves the image as a PPM file
    [[nodiscard]] bool saveAsPPM(std::string const & filename) const;

  private:
    size_t width;
    size_t height;

    struct PixelData {
      uint8_t r{}, g{}, b{};

      PixelData() = default;
    };

    // Array of Structs
    std::vector<PixelData> pixels;
  };

}  // namespace render

#endif  // PAR_HPP
