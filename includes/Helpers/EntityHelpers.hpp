#pragma once

#include "../EntityManagement/Entity.hpp"
#include "../EntityManagement/EntityManager.hpp"
#include "../Helpers/MathHelpers.hpp"
#include <algorithm>
#include <memory>
#include <vector>

namespace EntityHelpers {
  EntityVector findClosestEntities(const std::shared_ptr<Entity> &entity,
                                   const EntityVector            &candidates,
                                   const size_t                  &limit);

  EntityVector getEntitiesInRadius(const std::shared_ptr<Entity> &entity,
                                   const EntityVector            &candidates,
                                   const float                   &radius);
} // namespace EntityHelpers
