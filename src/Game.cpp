#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include "../includes/CollisionHelpers.h"
#include "../includes/EntityManager.h"
#include "../includes/Game.h"
#include "../includes/Tags.h"

Game::Game() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL video system is not ready to go: " << SDL_GetError() << std::endl;
    return;
  }
  std::cout << "SDL video system is ready to go" << std::endl;

  const std::string &WINDOW_TITLE = m_gameConfig.windowTitle;

  m_window = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, 1366, 768, SDL_WINDOW_SHOWN);
  if (m_window == nullptr) {
    std::cerr << "Window could not be created: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return;
  }
  std::cout << "Window created successfully!" << std::endl;

  m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
  if (m_renderer == nullptr) {
    std::cerr << "Renderer could not be created: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(m_window);
    SDL_Quit();
    return;
  }
  std::cout << "Renderer created successfully!" << std::endl;

  SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
  SDL_RenderClear(m_renderer);
  SDL_RenderPresent(m_renderer);

  m_isRunning = true; // Set isRunning to true after successful initialization

  spawnPlayer();
}
Game::~Game() {
  if (m_renderer != nullptr) {
    SDL_DestroyRenderer(m_renderer);
    m_renderer = nullptr;
  }
  if (m_window != nullptr) {
    SDL_DestroyWindow(m_window);
    m_window = nullptr;
  }
  SDL_Quit();
  std::cout << "Cleanup completed, SDL exited." << std::endl;
}

void Game::mainLoop(void *arg) {
  Game *game = static_cast<Game *>(arg);
  game->sInput();
  game->sRender();
  game->sMovement();
  game->sCollision();
}
void Game::spawnPlayer() {

  auto player = m_entities.addEntity(EntityTags::Player);

  auto &cTransform = player->cTransform;
  auto &cShape     = player->cShape;
  auto &cCollision = player->cCollision;
  auto &cInput     = player->cInput;

  cTransform = std::make_shared<CTransform>(Vec2(0, 0), Vec2(0, 0), 0);
  cShape     = std::make_shared<CShape>(m_renderer, m_playerConfig.shape);
  cCollision = std::make_shared<CCollision>(0);
  cInput     = std::make_shared<CInput>();

  std::cout << "Player entity created" << std::endl;

  m_entities.update();
}

void Game::run() {
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg(Game::mainLoop, this, 0, 1);
#else
  while (m_isRunning) {
    mainLoop(this);
  }
#endif
  std::cout << "Game loop exited" << std::endl;
}

void Game::sInput() {
  SDL_Event event;
  auto      player = m_entities.getEntities(EntityTags::Player)[0];
  while (SDL_PollEvent(&event)) {

    if (event.type == SDL_QUIT) {
      m_isRunning = false; // Stop the game loop
      return;
    }

    if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
        case SDLK_w: {
          player->cInput->forward = true;
          break;
        }
        case SDLK_s: {
          player->cInput->backward = true;
          break;
        }

        case SDLK_a: {
          player->cInput->left = true;
          break;
        }

        case SDLK_d: {
          player->cInput->right = true;
          break;
        }
      }
    }

    if (event.type == SDL_KEYUP) {
      switch (event.key.keysym.sym) {
        case SDLK_w: {
          player->cInput->forward = false;
          break;
        }
        case SDLK_s: {
          player->cInput->backward = false;
          break;
        }
        case SDLK_a: {
          player->cInput->left = false;
          break;
        }
        case SDLK_d: {
          player->cInput->right = false;
          break;
        }
      }
    }
  }
}

void Game::sRender() {
  SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
  SDL_RenderClear(m_renderer);

  for (auto &entity : m_entities.getEntities()) {
    if (entity->cShape == nullptr) {
      continue;
    }

    SDL_Rect &rect = entity->cShape->rect;
    Vec2     &pos  = entity->cTransform->position;
    rect.x         = static_cast<int>(pos.x);
    rect.y         = static_cast<int>(pos.y);

    SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(m_renderer, &rect);
  }

  // Update the screen
  SDL_RenderPresent(m_renderer);
}

void Game::sCollision() {
  const auto    &player       = m_entities.getEntities(EntityTags::Player)[0];
  const Vec2    &playerPos    = player->cTransform->position;
  const float    playerRadius = player->cCollision->radius;
  const Vec2     windowSize   = Vec2(1366, 768);
  std::bitset<4> collides     = CollisionHelpers::detectOutOfBounds(player, windowSize);
  CollisionHelpers::enforcePlayerBounds(player, collides, windowSize);
}

void Game::sMovement() {
  auto &player         = m_entities.getEntities(EntityTags::Player)[0];
  Vec2  playerVelocity = {0, 0};

  if (player->cInput == nullptr) {
    throw std::runtime_error("Player entity lacks an input component.");
  }

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

  for (auto e : m_entities.getEntities()) {
    if (e->cTransform == nullptr) {
      throw std::runtime_error("Entity " + e->tag() + ", with ID " + std::to_string(e->id()) +
                               " lacks a transform component.");
    }

    Vec2 &position = e->cTransform->position;

    if (e->tag() == EntityTags::Player) {
      std::cout << "Player position: " << position.x << ", " << position.y << std::endl;
      position += playerVelocity * 2;
    }
  }
}