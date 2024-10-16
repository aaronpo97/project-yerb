#ifndef GAME_H
#define GAME_H

#include "../includes/ConfigManager.h"
#include "../includes/EntityManager.h"
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <iostream>

class Game {
private:
  bool                    m_isRunning = false;
  bool                    m_paused    = false;
  EntityManager           m_entities;
  float                   m_deltaTime = 0;
  int                     m_score     = 0;
  SDL_Renderer           *m_renderer  = nullptr;
  SDL_Window             *m_window    = nullptr;
  std::shared_ptr<Entity> m_player;
  Uint64                  m_lastEnemySpawnTime = 0;
  Uint64                  m_lastFrameTime      = 0;
  TTF_Font               *m_font_big;
  TTF_Font               *m_font_small;
  Uint64                  m_timeRemaining = 60 * 1000; // 60 seconds
  ConfigManager           m_configManager;
  bool                    m_gameOver = false;

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
  void sTimer();

  void renderText();

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
  void spawnEnemy();
  void spawnSpeedBoost();
  void setGameOver() {

    if (m_gameOver) {
      return;
    }
    m_gameOver = true;

    std::cout << "Game over! 😭" << std::endl;
  }
};

#endif // GAME_H
