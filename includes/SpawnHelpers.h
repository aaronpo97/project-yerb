#ifndef SPAWN_HELPERS_H
#define SPAWN_HELPERS_H

#include "./CollisionHelpers.h"
#include "./ConfigManager.h"
#include "./EntityManager.h"
#include "./EntityTags.h"
#include "./Vec2.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <memory>
#include <random>

namespace SpawnHelpers {
  std::shared_ptr<Entity> spawnPlayer(SDL_Renderer  *m_renderer,
                                      ConfigManager &m_configManager,
                                      EntityManager &m_entities);

  void spawnEnemy(SDL_Renderer  *m_renderer,
                  ConfigManager &m_configManager,
                  std::mt19937  &m_randomGenerator,
                  EntityManager &m_entities);

  void spawnSpeedBoostEntity(SDL_Renderer  *m_renderer,
                             ConfigManager &m_configManager,
                             std::mt19937  &m_randomGenerator,
                             EntityManager &m_entities);

  void spawnSlownessEntity(SDL_Renderer        *m_renderer,
                           const ConfigManager &m_configManager,
                           std::mt19937        &m_randomGenerator,
                           EntityManager       &m_entities);
} // namespace SpawnHelpers

#endif // SPAWN_HELPERS_H
