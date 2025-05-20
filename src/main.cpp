#include "../includes/GameEngine/GameEngine.hpp"
#include <SDL2/SDL_main.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

int main(int argc, char *argv[]) {
#ifndef __EMSCRIPTEN__
  SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);
#endif
  auto gameEngine = GameEngine();
  gameEngine.run();

  return 0;
}