#pragma once

#include "../EntityManagement/EntityManager.hpp"
#include "../GameScenes/Scene.hpp"

#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <random>

class MainScene : public Scene {
private:
  Uint64                  m_lastEnemySpawnTime = 0;
  Uint64                  m_lastFrameTime      = 0;
  EntityManager           m_entities;
  float                   m_deltaTime = 0;
  bool                    m_paused    = false;
  int                     m_score     = 0;
  std::shared_ptr<Entity> m_player;
  Uint64                  m_timeRemaining = 60 * 1000; // 60 seconds
  bool                    m_gameOver      = false;
  std::random_device      m_rd;
  std::mt19937            m_randomGenerator = std::mt19937(m_rd());
  void                    renderText();

public:
  MainScene(GameEngine *gameEngine);

  void update() override;
  void onEnd() override;
  void sRender() override;
  void sDoAction(Action &action) override;

  void sCollision();
  void sMovement();
  void sSpawner();
  void sLifespan();
  void sEffects();
  void sTimer();

  void setScore(const int score);
  void setGameOver();
};
