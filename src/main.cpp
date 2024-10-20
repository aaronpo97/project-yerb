#include <SDL_main.h>

#include "../includes/GameEngine/GameEngine.hpp"
int main(int argc, char *argv[]) {
  GameEngine gameEngine = GameEngine();
  gameEngine.run();

  return 0;
}