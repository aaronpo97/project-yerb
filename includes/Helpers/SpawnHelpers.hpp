#pragma once

#include "../Configuration/ConfigManager.hpp"
#include "../EntityManagement/Entity.hpp"
#include "../EntityManagement/EntityManager.hpp"
#include "../Helpers/Vec2.hpp"

#include <SDL2/SDL.h>
#include <iostream>
#include <memory>
#include <random>

namespace SpawnHelpers {
  std::shared_ptr<Entity> spawnPlayer(SDL_Renderer  *renderer,
                                      ConfigManager &configManager,
                                      EntityManager &entityManager);

  void spawnEnemy(SDL_Renderer  *renderer,
                  ConfigManager &configManager,
                  std::mt19937  &randomGenerator,
                  EntityManager &entityManager);

  void spawnSpeedBoostEntity(SDL_Renderer  *renderer,
                             ConfigManager &configManager,
                             std::mt19937  &randomGenerator,
                             EntityManager &entityManager);

  void spawnSlownessEntity(SDL_Renderer        *renderer,
                           const ConfigManager &configManager,
                           std::mt19937        &randomGenerator,
                           EntityManager       &entityManager);

  void spawnWalls(SDL_Renderer        *renderer,
                  const ConfigManager &configManager,
                  std::mt19937        &randomGenerator,
                  EntityManager       &entityManager);

  void spawnBullets(SDL_Renderer                  *renderer,
                    ConfigManager                 &configManager,
                    EntityManager                 &entityManager,
                    const std::shared_ptr<Entity> &player,
                    const Vec2                    &mousePosition);
} // namespace SpawnHelpers
