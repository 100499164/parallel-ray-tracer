#ifndef BVH_NODE_HPP
#define BVH_NODE_HPP

#include "aabb.hpp"
#include "infocolision.hpp"
#include "render_figures/hittable.hpp"
#include <memory>
#include <vector>

namespace render {

  class BvhNode : public render::Hittable {
  public:
    std::shared_ptr<render::Hittable> left;
    std::shared_ptr<render::Hittable> right;
    AABB box;

    BvhNode(std::vector<std::shared_ptr<render::Hittable>> & objects, size_t start, size_t end);

    bool hit(render::Ray const & r, double t_min, double t_max,
             render::InfoColision & record) const override;
    bool bounding_box(AABB & output_box) const override;

    void assign_two_objects(std::vector<std::shared_ptr<Hittable>> & objects, size_t start,
                            bool (*comparator)(std::shared_ptr<Hittable> const &,
                                               std::shared_ptr<Hittable> const &));
    void compute_bounding_box();
  };

}  // namespace render

#endif
