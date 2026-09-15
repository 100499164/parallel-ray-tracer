#include "par.hpp"
#include "color.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace render {

  // Constructor that initializes the image with width and height
  AOS::AOS(size_t width, size_t height) : width(width), height(height), pixels(width * height) { }

  size_t AOS::getIndex(size_t x, size_t y) const noexcept {
    return y * width + x;
  }

  // Stablishes the full pixel at the given index
  void AOS::setPixel(size_t x, size_t y, Color const & pixel) {
    if (x >= width or y >= height) {
      throw std::out_of_range("Pixel index out of range.");
    }
    size_t const index = getIndex(x, y);

    double const r_clamped = std::clamp(pixel.r, 0.0, 1.0);
    double const g_clamped = std::clamp(pixel.g, 0.0, 1.0);
    double const b_clamped = std::clamp(pixel.b, 0.0, 1.0);

    // 2. Escala de [0.0, 1.0] a [0, 255] y LUEGO convierte a entero
    pixels[index].r = static_cast<uint8_t>(r_clamped * 255.0);
    pixels[index].g = static_cast<uint8_t>(g_clamped * 255.0);
    pixels[index].b = static_cast<uint8_t>(b_clamped * 255.0);
  }

  // Obtains the full pixel at the given index
  Color AOS::getPixel(size_t x, size_t y) const {
    if (x >= width or y >= height) {
      throw std::out_of_range("Pixel index out of range.");
    }
    size_t const index = getIndex(x, y);

    // Hacemos el cast a double y normalizamos (dividimos por 255.0)
    double const r = static_cast<double>(pixels[index].r) / 255.0;
    double const g = static_cast<double>(pixels[index].g) / 255.0;
    double const b = static_cast<double>(pixels[index].b) / 255.0;

    // Llamamos al constructor de Color (double, double, double)
    return {r, g, b};
  }

  // --- Channel Getters (inefficient in AOS) ---

  std::vector<uint8_t> AOS::getRedChannel() const {
    std::vector<uint8_t> R(width * height);
    for (size_t i = 0; i < pixels.size(); ++i) {
      R[i] = pixels[i].r;
    }
    return R;
  }

  std::vector<uint8_t> AOS::getGreenChannel() const {
    std::vector<uint8_t> G(width * height);
    for (size_t i = 0; i < pixels.size(); ++i) {
      G[i] = pixels[i].g;
    }
    return G;
  }

  std::vector<uint8_t> AOS::getBlueChannel() const {
    std::vector<uint8_t> B(width * height);
    for (size_t i = 0; i < pixels.size(); ++i) {
      B[i] = pixels[i].b;
    }
    return B;
  }

  // --- Getters ---

  size_t AOS::getWidth() const noexcept {
    return width;
  }

  size_t AOS::getHeight() const noexcept {
    return height;
  }

  // --- Other Methods ---

  std::string AOS::str() const {
    std::ostringstream oss;
    oss << "AOS image (" << width << "x" << height << ")\n";

    oss << "R: ";
    for (auto const & pixel : pixels) {
      oss << static_cast<int>(pixel.r) << '\t';
    }
    oss << '\n';

    oss << "G: ";
    for (auto const & pixel : pixels) {
      oss << static_cast<int>(pixel.g) << '\t';
    }
    oss << '\n';

    oss << "B: ";
    for (auto const & pixel : pixels) {
      oss << static_cast<int>(pixel.b) << '\t';
    }
    oss << '\n';

    return oss.str();
  }

  bool AOS::saveAsPPM(std::string const & filename) const {
    std::ofstream file(filename);

    if (!file.is_open()) {
      return false;
    }

    // PPM Header
    file << "P3\n"
         << width << " " << height << "\n"
         << "255\n";

    // Data buffer for writing pixel values instead of line by line in the file
    std::ostringstream data_buffer;

    // Pixel Data
    // Very efficient in AOS since we can write the whole pixel at once
    for (auto const & pixel : pixels) {
      data_buffer << (int) pixel.r << " " << (int) pixel.g << " " << (int) pixel.b << "\n";
    }

    // Write all pixel data at once (more efficient)
    file << data_buffer.str();

    file.close();
    return true;
  }

}  // namespace render
