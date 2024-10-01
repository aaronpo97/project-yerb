#include "../includes/Game.h"

int main(int argc, char *argv[]) {
  Game game = Game();

  game.init();
  game.run();
}