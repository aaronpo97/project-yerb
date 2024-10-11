#ifndef GAME_H
#define GAME_H

#include "../includes/EntityManager.h"
#include <SDL2/SDL.h>
#include <iostream>

struct GameConfig {
  Vec2        windowSize;
  std::string windowTitle;
};

struct PlayerConfig {
  float       speed;
  ShapeConfig shape;
};

class Game {
private:
  SDL_Window             *m_window    = nullptr;
  SDL_Renderer           *m_renderer  = nullptr;
  bool                    m_isRunning = false;
  EntityManager           m_entities;
  float                   m_deltaTime = 0.0f;
  bool                    m_paused    = false;
  std::shared_ptr<Entity> m_player;
  Uint32                  m_lastEnemySpawnTime = 0;
  int                     m_score              = 0;

  GameConfig   m_gameConfig   = {{1366, 768}, "C++ SDL2 Window"};
  PlayerConfig m_playerConfig = {2.0f, {80, 80, {0, 0, 255, 255}}};

public:
  Game();
  ~Game();

  void run();
  void sInput();
  void sRender();
  void sCollision();
  void sMovement();
  void sSpawner();
  void sLifespan();
  void sEffects();

  void setPaused(const bool paused) {
    std::cout << "Game is " << (paused ? "paused" : "unpaused") << std::endl;
    m_paused = paused;
  }

  static void mainLoop(void *arg);

  void spawnPlayer();
  void setPlayerSpeed(const float speed) {
    m_playerConfig.speed = speed;
    std::cout << "Player speed set to " << speed << std::endl;
  }
  void spawnEnemy();
  void spawnSpeedBoost();
};

#endif // GAME_H
