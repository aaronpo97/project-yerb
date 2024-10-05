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
  SDL_Window   *m_window    = nullptr;
  SDL_Renderer *m_renderer  = nullptr;
  bool          m_isRunning = false;
  EntityManager m_entities;
  float         m_deltaTime = 0.0f;

  GameConfig   m_gameConfig   = {{1366, 768}, "C++ SDL2 Window"};
  PlayerConfig m_playerConfig = {5.0f, {50, 50}};

public:
  Game();
  ~Game();

  void run();
  void sInput();
  void sRender();
  void sCollision();
  void sMovement();

  static void mainLoop(void *arg);
  void        spawnPlayer();
};

#endif // GAME_H
