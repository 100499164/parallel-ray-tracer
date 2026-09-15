#ifndef IMAGE_HPP
#define IMAGE_HPP

/**
 * @brief Abstract class representing an image. (SOA or AOS)
 *
 */
#include "color.hpp"
#include <cstddef>

namespace render {

  class Image {
  public:
    Image()          = default;
    virtual ~Image() = default;

    Image(Image const &)             = delete;
    Image & operator=(Image const &) = delete;

    Image(Image &&)             = delete;
    Image & operator=(Image &&) = delete;

    virtual void setPixel(size_t x, size_t y, Color const & color) = 0;

    [[nodiscard]] virtual Color getPixel(size_t x, size_t y) const = 0;

    [[nodiscard]] virtual size_t getWidth() const = 0;

    [[nodiscard]] virtual size_t getHeight() const = 0;
  };

}  // namespace render

#endif  // IMAGE_HPP
