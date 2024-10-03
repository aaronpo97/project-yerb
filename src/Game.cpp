#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include "../includes/CollisionHelpers.h"
#include "../includes/EntityManager.h"
#include "../includes/Game.h"
#include "../includes/Tags.h"

Game::Game() {}

Game::~Game() {
  if (renderer != nullptr) {
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
  }
  if (window != nullptr) {
    SDL_DestroyWindow(window);
    window = nullptr;
  }
  SDL_Quit();
  std::cout << "Cleanup completed, SDL exited." << std::endl;
}

void Game::init() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL video system is not ready to go: " << SDL_GetError() << std::endl;
    return;
  }
  std::cout << "SDL video system is ready to go" << std::endl;

  window = SDL_CreateWindow("C++ SDL2 Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            1366, 768, SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    std::cerr << "Window could not be created: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return;
  }
  std::cout << "Window created successfully!" << std::endl;

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    std::cerr << "Renderer could not be created: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return;
  }
  std::cout << "Renderer created successfully" << std::endl;

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  isRunning = true; // Set isRunning to true after successful initialization

  spawnPlayer();
}
void Game::mainLoop(void *arg) {
  Game *game = static_cast<Game *>(arg);
  game->sInput();
  game->sRender();
  game->sMovement();
  game->sCollision();
}
void Game::spawnPlayer() {

  auto player        = entities.addEntity(EntityTags::Player);
  player->cTransform = std::make_shared<CTransform>(Vec2(100, 100), Vec2(0, 0), 0);
  player->cShape =
      std::make_shared<CShape>(renderer, ShapeConfig{50.0f, 50.0f, {255, 0, 0, 255}});
  player->cCollision = std::make_shared<CCollision>(25);
  player->cInput     = std::make_shared<CInput>();

  std::cout << "Player entity created" << std::endl;

  entities.update();
}

void Game::run() {
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg(Game::mainLoop, this, 0, 1);
#else
  while (isRunning) {
    mainLoop(this);
  }
#endif
  std::cout << "Game loop exited" << std::endl;
}

void Game::sInput() {
  SDL_Event event;
  auto      player = entities.getEntities(EntityTags::Player)[0];
  while (SDL_PollEvent(&event)) {

    if (event.type == SDL_QUIT) {
      std::cout << "Quit event detected - exiting game loop" << std::endl;
      isRunning = false; // Stop the game loop
      return;
    }

    if (event.type == SDL_KEYDOWN) {
      std::cout << "Key pressed" << std::endl;
      switch (event.key.keysym.sym) {
        case SDLK_w: {
          std::cout << "W key pressed" << std::endl;
          player->cInput->forward = true;
          break;
        }
        case SDLK_s: {
          std::cout << "S key pressed" << std::endl;
          player->cInput->backward = true;
          break;
        }

        case SDLK_a: {
          std::cout << "A key pressed" << std::endl;
          player->cInput->left = true;
          break;
        }

        case SDLK_d: {
          std::cout << "D key pressed" << std::endl;
          player->cInput->right = true;
          break;
        }
      }
    }

    if (event.type == SDL_KEYUP) {
      std::cout << "Key released" << std::endl;
      switch (event.key.keysym.sym) {
        case SDLK_w: {
          std::cout << "W key released" << std::endl;
          player->cInput->forward = false;
          break;
        }
        case SDLK_s: {
          std::cout << "S key released" << std::endl;
          player->cInput->backward = false;
          break;
        }

        case SDLK_a: {
          std::cout << "A key released" << std::endl;
          player->cInput->left = false;
          break;
        }

        case SDLK_d: {
          std::cout << "D key released" << std::endl;
          player->cInput->right = false;
          break;
        }
      }
    }
  }
}

void Game::sRender() {
  // Clear the screen
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Set background to black
  SDL_RenderClear(renderer);

  // // Draw a red rectangle
  // SDL_Rect rect = {10, 10, 100, 100};
  // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  // SDL_RenderFillRect(renderer, &rect);

  // Draw all entities
  for (auto &entity : entities.getEntities()) {
    if (entity->cShape == nullptr) {
      continue;
    }

    SDL_Rect &rect = entity->cShape->rect;
    Vec2     &pos  = entity->cTransform->position;
    rect.x         = static_cast<int>(pos.x);
    rect.y         = static_cast<int>(pos.y);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
  }

  // Update the screen
  SDL_RenderPresent(renderer);
}

struct PlayerConfig {
  float speed;
};

void Game::sCollision() {
  const auto player       = entities.getEntities(EntityTags::Player)[0];
  const auto playerPos    = player->cTransform->position;
  const auto playerRadius = player->cCollision->radius;

  const auto windowSize = Vec2(1366, 768);

  auto collides = CollisionHelpers::detectOutOfBounds(player, windowSize);
  CollisionHelpers::enforcePlayerBounds(player, collides, windowSize);
}

void Game::sMovement() {
  auto player         = entities.getEntities(EntityTags::Player)[0];
  Vec2 playerVelocity = {0, 0};
  if (player->cInput->forward) {
    playerVelocity.y = -1;
  }
  if (player->cInput->backward) {
    playerVelocity.y = 1;
  }
  if (player->cInput->left) {
    playerVelocity.x = -1;
  }
  if (player->cInput->right) {
    playerVelocity.x = 1;
  }
  for (auto e : entities.getEntities()) {
    if (e->tag() == EntityTags::Player) {
      player->cTransform->position += playerVelocity * 4;
    }
  }
}