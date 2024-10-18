#include "../includes/Game.h"
#include <time.h>
int main(int argc, char *argv[]) {
  srand(time(0));
  Game game = Game();
  game.run();
}