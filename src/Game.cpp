#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include "../includes/CollisionHelpers.h"
#include "../includes/EntityManager.h"
#include "../includes/EntityTags.h"
#include "../includes/Game.h"
#include "../includes/MovementHelpers.h"

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
  SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
  SDL_RenderClear(m_renderer);
  SDL_RenderPresent(m_renderer);

  m_isRunning = true; // Set isRunning to true after successful initialization

  std::cout << "Game initialized successfully! 🥰" << std::endl;
  std::cout << "Spawning player entity..." << std::endl;

  spawnPlayer();
  std::cout << "You just spawned in the game! 🎉" << std::endl;
  std::cout << "Press W to move forward, S to move backward, A to move left, D to move right, "
               "and P to pause/unpause the game."
            << std::endl;
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

  const Uint32 currentTime = SDL_GetTicks();
  game->m_deltaTime        = (currentTime - game->m_lastTime);

#ifdef __EMSCRIPTEN__
  if (!game->m_isRunning) {
    emscripten_cancel_main_loop();
  }
#endif

  game->sInput();

  if (!game->m_paused) {
    game->sMovement();
    game->sCollision();
    game->sSpawner();
    game->sLifespan();
    game->sRender();
    game->sEffects();
  }
  // Update the lastTime for the next loop iteration
  game->m_lastTime = currentTime;
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
  const Vec2 windowSize = Vec2(1366, 768);

  for (auto &entity : m_entities.getEntities()) {

    if (entity->tag() == EntityTags::SpeedBoost) {
      std::bitset<4> speedBoostCollides =
          CollisionHelpers::detectOutOfBounds(entity, windowSize);
      CollisionHelpers::enforceNonPlayerBounds(entity, speedBoostCollides, windowSize);
    }

    if (entity->tag() == EntityTags::Player) {
      std::bitset<4> playerCollides = CollisionHelpers::detectOutOfBounds(entity, windowSize);
      CollisionHelpers::enforcePlayerBounds(entity, playerCollides, windowSize);
    }

    if (entity->tag() == EntityTags::Enemy) {
      std::bitset<4> enemyCollides = CollisionHelpers::detectOutOfBounds(entity, windowSize);
      CollisionHelpers::enforceNonPlayerBounds(entity, enemyCollides, windowSize);
    }

    for (auto &otherEntity : m_entities.getEntities()) {
      if (entity == otherEntity) {
        continue;
      }

      if (CollisionHelpers::calculateCollisionBetweenEntities(entity, otherEntity)) {
        if (entity->tag() == EntityTags::Player && otherEntity->tag() == EntityTags::Enemy) {
          std::cout << "Player caught an enemy!" << std::endl;
          setScore(m_score + 1);
          otherEntity->destroy();
        }

        if (entity->tag() == EntityTags::Player &&
            otherEntity->tag() == EntityTags::SpeedBoost) {
          std::cout << "Player collided with speed boost!" << std::endl;

          const Uint32 startTime = SDL_GetTicks();
          const Uint32 duration  = 7000 + (rand() % 5000);
          entity->cEffects->addEffect({startTime, duration, EffectTypes::Speed});

          const EntityVector &speedBoosts = m_entities.getEntities("SpeedBoost");

          for (auto &speedBoost : speedBoosts) {
            speedBoost->destroy();
          }
        }
      }
    }
  }

  m_entities.update();
}

void Game::sMovement() {
  for (std::shared_ptr<Entity> entity : m_entities.getEntities()) {
    MovementHelpers::moveEnemies(entity, m_deltaTime);
    MovementHelpers::moveSpeedBoosts(entity, m_deltaTime);
    MovementHelpers::movePlayer(entity, m_playerConfig, m_deltaTime);
  }
}

void Game::sSpawner() {
  const Uint32 ticks = SDL_GetTicks();
  if (ticks - m_lastEnemySpawnTime < 2500) {
    return;
  }
  m_lastEnemySpawnTime = ticks;
  spawnEnemy();

  const auto &playerEffects = m_player->cEffects->getEffects();

  const bool hasSpeedBoost =
      std::find_if(playerEffects.begin(), playerEffects.end(), [](const Effect &effect) {
        return effect.type == EffectTypes::Speed;
      }) != playerEffects.end();

  // Spawns a speed boost with a 10% chance and while speed boost is not active
  const bool willSpawnSpeedBoost = rand() % 100 < 10 && !hasSpeedBoost;
  if (willSpawnSpeedBoost) {
    spawnSpeedBoost();
  }
}

void Game::sEffects() {
  const auto effects = m_player->cEffects->getEffects();
  if (effects.empty()) {
    return;
  }

  const Uint32 currentTime = SDL_GetTicks();
  for (auto &effect : effects) {
    const bool effectExpired = currentTime - effect.startTime > effect.duration;
    if (!effectExpired) {
      if (effect.type == EffectTypes::Speed) {
        m_playerConfig.speed = 8.0f;
      }

      return;
    }

    m_player->cEffects->removeEffect(effect.type);
    if (effect.type == EffectTypes::Speed) {
      std::cout << "Your speed boost expired. 😔" << std::endl;
      m_playerConfig.speed = 4.0f;
    }
  }
}

void Game::sLifespan() {
  for (auto &entity : m_entities.getEntities()) {
    if (entity->tag() == EntityTags::Player) {
      continue;
    }
    if (entity->cLifespan == nullptr) {
      std::cerr << "Entity with ID " << entity->id() << " lacks a lifespan component"
                << std::endl;
      continue;
    }

    const Uint32 currentTime = SDL_GetTicks();
    Uint32       elapsedTime = currentTime - entity->cLifespan->birthTime;
    // Calculate the lifespan percentage, ensuring it's clamped between 0 and 1
    float lifespanPercentage =
        std::min(1.0f, static_cast<float>(elapsedTime) / entity->cLifespan->lifespan);

    // Check if the entity's lifespan has expired
    if (elapsedTime > entity->cLifespan->lifespan) {
      if (entity->tag() == EntityTags::Enemy) {
        std::cout << "You missed an enemy! 😢" << std::endl;
        setScore(m_score - 1);
      }
      entity->destroy();
      continue;
    }

    const float MAX_COLOR_VALUE = 255.0f;
    float       colorValue =
        MAX_COLOR_VALUE * (1.0f - lifespanPercentage); // Fade out as time progresses
    // Ensure the alpha value is clamped between 0 and 255
    colorValue = std::max(0.0f, std::min(colorValue, MAX_COLOR_VALUE));
    // Update the entity's alpha value
    auto &color           = entity->cShape->color;
    entity->cShape->color = {color.r, color.g, color.b, colorValue};
  }
}

void Game::spawnPlayer() {
  m_player = m_entities.addEntity(EntityTags::Player);

  std::shared_ptr<CTransform> &playerCTransform = m_player->cTransform;
  std::shared_ptr<CShape>     &playerCShape     = m_player->cShape;
  std::shared_ptr<CInput>     &playerCInput     = m_player->cInput;
  std::shared_ptr<CEffects>   &playerCEffects   = m_player->cEffects;

  playerCTransform = std::make_shared<CTransform>(Vec2(0, 0), Vec2(0, 0), 0);
  playerCShape     = std::make_shared<CShape>(m_renderer, m_playerConfig.shape);
  playerCInput     = std::make_shared<CInput>();
  playerCEffects   = std::make_shared<CEffects>();

  std::cout << "Player entity created!" << std::endl;

  m_entities.update();
}

void Game::spawnEnemy() {
  const Vec2 &windowSize = m_gameConfig.windowSize;
  const int   x          = rand() % static_cast<int>(windowSize.x);
  const int   y          = rand() % static_cast<int>(windowSize.y);

  std::shared_ptr<Entity>      enemy            = m_entities.addEntity(EntityTags::Enemy);
  std::shared_ptr<CTransform> &entityCTransform = enemy->cTransform;
  std::shared_ptr<CShape>     &entityCShape     = enemy->cShape;
  std::shared_ptr<CLifespan>  &entityLifespan   = enemy->cLifespan;

  entityCTransform = std::make_shared<CTransform>(Vec2(x, y), Vec2(0, 0), 0);

  ShapeConfig cShapeConfig  = {40, 40, {255, 0, 0, 255}};
  entityCShape              = std::make_shared<CShape>(m_renderer, cShapeConfig);
  entityLifespan            = std::make_shared<CLifespan>(30000);
  bool touchesBoundary      = CollisionHelpers::detectOutOfBounds(enemy, windowSize).any();
  bool touchesOtherEntities = false;

  for (auto &entity : m_entities.getEntities()) {
    if (CollisionHelpers::calculateCollisionBetweenEntities(entity, enemy)) {
      touchesOtherEntities = true;
      break;
    }
  }

  bool      isValidSpawn       = !touchesBoundary && !touchesOtherEntities;
  const int MAX_SPAWN_ATTEMPTS = 10;
  int       spawnAttempt       = 1;

  while (!isValidSpawn && spawnAttempt < MAX_SPAWN_ATTEMPTS) {
    const int randomX = rand() % static_cast<int>(windowSize.x);
    const int randomY = rand() % static_cast<int>(windowSize.y);

    enemy->cTransform->topLeftCornerPos = Vec2(randomX, randomY);
    touchesBoundary      = CollisionHelpers::detectOutOfBounds(enemy, windowSize).any();
    touchesOtherEntities = false;

    for (auto &entity : m_entities.getEntities()) {
      if (CollisionHelpers::calculateCollisionBetweenEntities(entity, enemy)) {
        touchesOtherEntities = true;
        break;
      }
    }

    spawnAttempt += 1;
    isValidSpawn = !touchesBoundary && !touchesOtherEntities;
  }

  if (!isValidSpawn) {
    enemy->destroy();
  }

  m_entities.update();
}

void Game::spawnSpeedBoost() {
  const Vec2 &windowSize = m_gameConfig.windowSize;
  const int   x          = rand() % static_cast<int>(windowSize.x);
  const int   y          = rand() % static_cast<int>(windowSize.y);

  std::shared_ptr<Entity>      speedBoost       = m_entities.addEntity(EntityTags::SpeedBoost);
  std::shared_ptr<CTransform> &entityCTransform = speedBoost->cTransform;
  std::shared_ptr<CShape>     &entityCShape     = speedBoost->cShape;
  std::shared_ptr<CLifespan>  &entityLifespan   = speedBoost->cLifespan;

  const ShapeConfig shapeConfig = {20, 20, {0, 255, 0, 255}};
  entityCTransform              = std::make_shared<CTransform>(Vec2(x, y), Vec2(0, 0), 0);
  entityCShape                  = std::make_shared<CShape>(m_renderer, shapeConfig);
  entityLifespan                = std::make_shared<CLifespan>(25000);

  bool touchesBoundary = CollisionHelpers::detectOutOfBounds(speedBoost, windowSize).any();
  bool touchesOtherEntities = false;

  for (auto &entity : m_entities.getEntities()) {
    if (CollisionHelpers::calculateCollisionBetweenEntities(entity, speedBoost)) {
      touchesOtherEntities = true;
      break;
    }
  }

  bool      isValidSpawn       = !touchesBoundary && !touchesOtherEntities;
  const int MAX_SPAWN_ATTEMPTS = 10;
  int       spawnAttempt       = 1;

  while (!isValidSpawn && spawnAttempt < MAX_SPAWN_ATTEMPTS) {
    const int randomX = rand() % static_cast<int>(windowSize.x);
    const int randomY = rand() % static_cast<int>(windowSize.y);

    speedBoost->cTransform->topLeftCornerPos = Vec2(randomX, randomY);
    touchesBoundary      = CollisionHelpers::detectOutOfBounds(speedBoost, windowSize).any();
    touchesOtherEntities = false;

    for (auto &entity : m_entities.getEntities()) {
      if (CollisionHelpers::calculateCollisionBetweenEntities(entity, speedBoost)) {
        touchesOtherEntities = true;
        break;
      }
    }

    spawnAttempt += 1;
    isValidSpawn = !touchesBoundary && !touchesOtherEntities;
  }

  if (!isValidSpawn) {
    speedBoost->destroy();
  }
  m_entities.update();
}
