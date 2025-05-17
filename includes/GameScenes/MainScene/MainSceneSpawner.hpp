#pragma once
#include "../../AssetManagement/TextureManager.hpp"
#include "../../Configuration/ConfigManager.hpp"
#include "../../EntityManagement/EntityManager.hpp"
#include <random>

class MainSceneSpawner {
  std::mt19937   m_randomGenerator;
  ConfigManager  m_configManager;
  TextureManager m_textureManager;
  EntityManager  m_entityManager;
  SDL_Renderer  *m_renderer;

public:
  MainSceneSpawner(std::mt19937   &randomGenerator,
                    ConfigManager  &configManager,
                    TextureManager &textureManager,
                    EntityManager  &entityManager,
                   SDL_Renderer         *renderer);

  std::shared_ptr<Entity> spawnPlayer();

  void spawnEnemy(const std::shared_ptr<Entity> &m_player);
  void spawnSpeedBoostEntity(const std::shared_ptr<Entity> &m_player);
  void spawnSlownessEntity(const std::shared_ptr<Entity> &m_player);
  void spawnWalls();
  void spawnBullets(const std::shared_ptr<Entity> &m_player,const Vec2 &mousePosition);
  void spawnItem(const std::shared_ptr<Entity> &m_player);
};