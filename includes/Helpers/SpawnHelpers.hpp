#pragma once

#include "../../includes/AssetManagement/TextureManager.hpp"
#include "../Configuration/ConfigManager.hpp"
#include "../EntityManagement/Entity.hpp"
#include "../EntityManagement/EntityManager.hpp"
#include "../Helpers/Vec2.hpp"

#include <SDL2/SDL.h>
#include <iostream>
#include <memory>
#include <random>

namespace SpawnHelpers {
  Vec2 createRandomPosition(std::mt19937 &randomGenerator, const Vec2 &windowSize);
  Vec2 createValidVelocity(std::mt19937 &randomGenerator, int attempts = 5);
  bool validateSpawnPosition(const std::shared_ptr<Entity> &entity,
                             const std::shared_ptr<Entity> &player,
                             EntityManager                 &entityManager,
                             const Vec2                    &windowSize);
} // namespace SpawnHelpers