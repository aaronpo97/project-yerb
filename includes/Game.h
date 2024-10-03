#ifndef GAME_H
#define GAME_H

#include "../includes/EntityManager.h"
#include <SDL2/SDL.h>
#include <iostream>

class Game {
private:
  SDL_Window   *window    = nullptr;
  SDL_Renderer *renderer  = nullptr;
  bool          isRunning = false;
  EntityManager entities;

public:
  Game();
  ~Game();

  void init();     // Initialize SDL and create window/renderer
  void run();      // Run the game loop
  void sInput();   // Handle input events
  void sRender();  // Render the game scene
  void sCleanup(); // Cleanup and shutdown SDL

  void sMovement();

  static void mainLoop(void *arg);
  void        spawnPlayer();
};

#endif // GAME_H
