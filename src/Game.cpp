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

  if (!game->m_paused) {
    game->sRender();
    game->sMovement();
    game->sCollision();
    game->sSpawner();
  }
}
void Game::spawnPlayer() {

  m_player = m_entities.addEntity(EntityTags::Player);

  auto &cTransform = m_player->cTransform;
  auto &cShape     = m_player->cShape;
  auto &cCollision = m_player->cCollision;
  auto &cInput     = m_player->cInput;

  cTransform = std::make_shared<CTransform>(Vec2(0, 0), Vec2(0, 0), 0);
  cShape     = std::make_shared<CShape>(m_renderer, m_playerConfig.shape);

  const auto radius = cShape->rect.w / 2;
  cCollision        = std::make_shared<CCollision>(radius);
  cInput            = std::make_shared<CInput>();

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
  while (SDL_PollEvent(&event)) {

    if (event.type == SDL_QUIT) {
      m_isRunning = false; // Stop the game loop
      return;
    }

    if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
        case SDLK_w: {
          m_player->cInput->forward = true;
          break;
        }
        case SDLK_s: {
          m_player->cInput->backward = true;
          break;
        }

        case SDLK_a: {
          m_player->cInput->left = true;
          break;
        }

        case SDLK_d: {
          m_player->cInput->right = true;
          break;
        }

        case SDLK_p: {
          setPaused(!m_paused);
          break;
        }
      }
    }

    if (event.type == SDL_KEYUP) {
      switch (event.key.keysym.sym) {
        case SDLK_w: {
          m_player->cInput->forward = false;
          break;
        }
        case SDLK_s: {
          m_player->cInput->backward = false;
          break;
        }
        case SDLK_a: {
          m_player->cInput->left = false;
          break;
        }
        case SDLK_d: {
          m_player->cInput->right = false;
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
    Vec2     &pos  = entity->cTransform->topLeftCornerPos;

    auto  centerPositionY = entity->cTransform->topLeftCornerPos.y + (rect.h / 2);
    auto  centerPositionX = entity->cTransform->topLeftCornerPos.x + (rect.w / 2);
    Vec2 &centerPos       = entity->cTransform->centerPos;

    centerPos.x = centerPositionX;
    centerPos.y = centerPositionY;

    rect.x = static_cast<int>(pos.x);
    rect.y = static_cast<int>(pos.y);

    const RGBA &color = entity->cShape->color;
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(m_renderer, &rect);
  }

  // Update the screen
  SDL_RenderPresent(m_renderer);
}

void Game::sCollision() {
  const Vec2     windowSize = Vec2(1366, 768);
  std::bitset<4> collides   = CollisionHelpers::detectOutOfBounds(m_player, windowSize);
  CollisionHelpers::enforcePlayerBounds(m_player, collides, windowSize);

  for (auto &entity : m_entities.getEntities()) {
    if (entity->tag() == EntityTags::Player) {
      continue;
    }

    if (CollisionHelpers::calculateCollisionBetweenEntities(m_player, entity)) {
      std::cout << "Player collided with " << entity->tag() << std::endl;
    }
  }
}

void Game::sMovement() {
  Vec2 playerVelocity = {0, 0};

  if (m_player->cInput == nullptr) {
    throw std::runtime_error("Player entity lacks an input component.");
  }

  if (m_player->cInput->forward) {
    playerVelocity.y = -1;
  }
  if (m_player->cInput->backward) {
    playerVelocity.y = 1;
  }
  if (m_player->cInput->left) {
    playerVelocity.x = -1;
  }
  if (m_player->cInput->right) {
    playerVelocity.x = 1;
  }

  for (auto e : m_entities.getEntities()) {
    if (e->cTransform == nullptr) {
      throw std::runtime_error("Entity " + e->tag() + ", with ID " + std::to_string(e->id()) +
                               " lacks a transform component.");
    }

    Vec2 &position = e->cTransform->topLeftCornerPos;

    if (e->tag() == EntityTags::Player) {
      position += playerVelocity * 2;
    }
  }
}
void Game::sSpawner() {
  const Uint32 ticks = SDL_GetTicks();
  if (ticks - m_lastEnemySpawnTime < 2500) {
    return;
  }
  m_lastEnemySpawnTime = ticks;
  spawnEnemy();
}

void Game::spawnEnemy() {
  std::shared_ptr<Entity>      enemy      = m_entities.addEntity("Enemy");
  std::shared_ptr<CTransform> &cTransform = enemy->cTransform;
  std::shared_ptr<CShape>     &cShape     = enemy->cShape;
  std::shared_ptr<CCollision> &cCollision = enemy->cCollision;

  const Vec2 &windowSize = m_gameConfig.windowSize;

  const int x = rand() % static_cast<int>(windowSize.x);
  const int y = rand() % static_cast<int>(windowSize.y);

  cTransform = std::make_shared<CTransform>(Vec2(x, y), Vec2(0, 0), 0);
  cShape     = std::make_shared<CShape>(m_renderer, ShapeConfig({40, 40, {255, 0, 0, 255}}));

  float centerPositionX = cTransform->topLeftCornerPos.x + cShape->rect.w / 2;
  float centerPositionY = cTransform->topLeftCornerPos.y + cShape->rect.h / 2;

  cTransform->centerPos = Vec2(centerPositionX, centerPositionY);

  auto radius = cShape->rect.w / 2;

  cCollision = std::make_shared<CCollision>(radius);

  std::cout << "Enemy entity created" << std::endl;

  m_entities.update();
}