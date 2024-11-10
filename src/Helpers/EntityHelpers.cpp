
#include "../../includes/Helpers/EntityHelpers.hpp"
#include <algorithm>
#include <vector>

namespace EntityHelpers {
  EntityVector findClosestEntities(const std::shared_ptr<Entity> &entity,
                                   const EntityVector            &candidates,
                                   const size_t                  &limit) {

    std::vector<std::pair<std::shared_ptr<Entity>, float>> distances;
    const Vec2                                            &center = entity->getCenterPos();

    for (const auto &candidate : candidates) {
      if (candidate == entity)
        continue;

      const Vec2 &candidateCenter = candidate->getCenterPos();

      float distance = MathHelpers::pythagorasSquared(center.x - candidateCenter.x,
                                                      center.y - candidateCenter.y);
      distances.emplace_back(candidate, distance);
    }

    std::ranges::sort(distances,
                      [](const auto &a, const auto &b) { return a.second < b.second; });

    EntityVector result;
    const size_t numToReturn = std::min(limit, distances.size());
    for (size_t i = 0; i < numToReturn; i++) {
      result.push_back(distances[i].first);
    }

    return result;
  }

  EntityVector getEntitiesInRadius(const std::shared_ptr<Entity> &entity,
                                   const EntityVector            &candidates,
                                   const float                   &radius) {

    EntityVector result;
    const Vec2  &center        = entity->getCenterPos();
    const float  radiusSquared = radius * radius;

    for (const auto &candidate : candidates) {
      if (candidate == entity)
        continue;

      const Vec2 &candidateCenter = candidate->getCenterPos();

      const float deltaX          = center.x - candidateCenter.x;
      const float deltaY          = center.y - candidateCenter.y;
      const float distanceSquared = MathHelpers::pythagorasSquared(deltaX, deltaY);

      if (distanceSquared >= radiusSquared) {
        continue;
      }
      result.push_back(candidate);
    }

    return result;
  }
} // namespace EntityHelpers
