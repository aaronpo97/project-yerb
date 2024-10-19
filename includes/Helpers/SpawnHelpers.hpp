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
