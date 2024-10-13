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
  Uint32                  m_lastTime  = 0;
  Uint32                  m_deltaTime = 0;
  bool                    m_paused    = false;
  std::shared_ptr<Entity> m_player;
  Uint32                  m_lastEnemySpawnTime = 0;
  int                     m_score              = 0;

  GameConfig   m_gameConfig   = {{1366, 768}, "C++ SDL2 Window"};
  PlayerConfig m_playerConfig = {4.0f, {80, 80, {0, 0, 255, 255}}};

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

  void setScore(const int score) {
    const int previousScore = m_score;
    m_score                 = score;

    const int diff = m_score - previousScore;

    if (m_score < 0) {
      std::cout << "Your score dipped below 0! 😬" << std::endl;
      setGameOver();
      return;
    }
    const std::string scoreChange = diff > 0 ? "increased" : "decreased";
    const std::string emoji       = diff > 0 ? "😃" : "😔";

    std::cout << "Score " << scoreChange << " by " << std::abs(diff) << " point"
              << (diff > 1 ? "s" : "") << "! " << emoji << std::endl;

    std::cout << "Your score is now " << m_score << "." << std::endl;
  }
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
  void setGameOver() {
    if (!m_isRunning) {
      throw std::runtime_error("Game is already over.");
      return;
    }

    m_isRunning = false;
    std::cout << "Game over! 😭" << std::endl;
  }
};

#endif // GAME_H
