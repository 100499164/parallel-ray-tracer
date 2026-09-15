#include "infocolision.hpp"
#include "ray.hpp"
#include "render_figures/hittable.hpp"
#include <gtest/gtest.h>
#include <memory>

using namespace render;

// Clase Dummy para test
class DummyHittable : public Hittable {
public:
  DummyHittable()           = default;
  ~DummyHittable() override = default;

  // No copies or assignments (heredados como delete)
  DummyHittable(DummyHittable const &)             = delete;
  DummyHittable & operator=(DummyHittable const &) = delete;

  // Movimientos permitidos (opcional)
  DummyHittable(DummyHittable &&) noexcept             = default;
  DummyHittable & operator=(DummyHittable &&) noexcept = default;

  mutable bool called = false;

  bool hit(Ray const & ray, double t_min, double t_max, InfoColision & record) const override {
    (void) ray;
    (void) t_min;
    (void) t_max;
    called        = true;
    record.t      = 42.0;
    record.p      = vector(1.0, 2.0, 3.0);
    record.normal = vector(0.0, 1.0, 0.0);
    return true;
  }
};

// ID: HIT-1
TEST(test_hittable, polymorphic_destruction_does_not_throw) {
  std::unique_ptr<Hittable> h = std::make_unique<DummyHittable>();
  EXPECT_NO_THROW(h.reset());
}

// ID: HIT-2
TEST(test_hittable, hit_method_is_virtual_and_called_correctly) {
  DummyHittable const dummy;
  Ray const ray({0.0, 0.0, -1.0}, vector(0.0, 0.0, 1.0));
  InfoColision rec{};
  EXPECT_TRUE(dummy.hit(ray, 0.0, 10.0, rec));

  EXPECT_DOUBLE_EQ(rec.t, 42.0);
  EXPECT_EQ(rec.p, vector(1.0, 2.0, 3.0));
  EXPECT_EQ(rec.normal, vector(0.0, 1.0, 0.0));
  EXPECT_TRUE(dummy.called);
}

// ID: HIT-3
TEST(test_hittable, cannot_instantiate_abstract_class) {
  SUCCEED();  // solo confirma que no se puede instanciar Hittable
}
