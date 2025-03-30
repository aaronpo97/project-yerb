#pragma once

#include "../../includes/AssetManagement/SurfaceManager.hpp"
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
namespace SpawnHelpers::MainScene {
  std::shared_ptr<Entity> spawnPlayer(SDL_Renderer        *renderer,
                                      const ConfigManager &configManager,
                                      EntityManager       &entityManager,
                                      SurfaceManager      &imageManager);

  void spawnEnemy(SDL_Renderer                  *renderer,
                  const ConfigManager           &configManager,
                  std::mt19937                  &randomGenerator,
                  EntityManager                 &entityManager,
                  const std::shared_ptr<Entity> &player);

  void spawnSpeedBoostEntity(SDL_Renderer                  *renderer,
                             const ConfigManager           &configManager,
                             std::mt19937                  &randomGenerator,
                             EntityManager                 &entityManager,
                             const std::shared_ptr<Entity> &player);

  void spawnSlownessEntity(SDL_Renderer                  *renderer,
                           const ConfigManager           &configManager,
                           std::mt19937                  &randomGenerator,
                           EntityManager                 &entityManager,
                           const std::shared_ptr<Entity> &player);

  void spawnWalls(SDL_Renderer        *renderer,
                  const ConfigManager &configManager,
                  EntityManager       &entityManager);

  void spawnBullets(SDL_Renderer                  *renderer,
                    const ConfigManager           &configManager,
                    EntityManager                 &entityManager,
                    const std::shared_ptr<Entity> &player,
                    const Vec2                    &mousePosition);

  void spawnItem(SDL_Renderer                  *renderer,
                 const ConfigManager           &configManager,
                 std::mt19937                  &randomGenerator,
                 EntityManager                 &entityManager,
                 const std::shared_ptr<Entity> &player);
} // namespace SpawnHelpers::MainScene
