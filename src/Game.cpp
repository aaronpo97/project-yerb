#include <filesystem>

#ifdef __EMSCRIPTEN__

#include <emscripten/emscripten.h>
#endif

#include "../includes/CollisionHelpers.h"
#include "../includes/EntityManager.h"
#include "../includes/EntityTags.h"
#include "../includes/Game.h"
#include "../includes/MovementHelpers.h"
#include "../includes/TextHelpers.h"

Game::Game() {
  if (!std::filesystem::exists("./assets")) {
    std::cerr << "Assets folder not found." << std::endl;
    return;
  }

  m_configManager = ConfigManager();

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL video system is not ready to go: " << SDL_GetError() << std::endl;
    return;
  }
  std::cout << "SDL video system is ready to go" << std::endl;

  if (TTF_Init() != 0) {
    std::cerr << "TTF_Init: " << TTF_GetError() << std::endl;
    return;
  }

  const std::string &fontPath = m_configManager.getGameConfig().fontPath;

  m_font_big   = TTF_OpenFont(fontPath.c_str(), 28);
  m_font_small = TTF_OpenFont(fontPath.c_str(), 14);

  if (!m_font_big || !m_font_small) {
    std::cerr << "Failed to load fonts: " << TTF_GetError() << std::endl;
    return;
  }
  std::cout << "Fonts loaded successfully!" << std::endl;

  const std::string &WINDOW_TITLE = m_configManager.getGameConfig().windowTitle;
  const Vec2        &WINDOW_SIZE  = m_configManager.getGameConfig().windowSize;

  m_window =
      SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       WINDOW_SIZE.x, WINDOW_SIZE.y, SDL_WINDOW_SHOWN);
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

  if (m_font_big != nullptr) {
    TTF_CloseFont(m_font_big);
    m_font_big = nullptr;
  }

  if (m_font_small != nullptr) {
    TTF_CloseFont(m_font_small);
    m_font_small = nullptr;
  }
  TTF_Quit();
  SDL_Quit();
  std::cout << "Cleanup completed, SDL exited." << std::endl;
}

void Game::mainLoop(void *arg) {
  Game *game = static_cast<Game *>(arg);

  const Uint64 currentTime = SDL_GetTicks64();
  game->m_deltaTime        = (currentTime - game->m_lastFrameTime) / 1000.0f;

#ifdef __EMSCRIPTEN__
  if (!game->m_isRunning) {
    emscripten_cancel_main_loop();
  }
#endif

  game->sInput();

  if (!game->m_paused) {
    if (!game->m_gameOver) {
      game->sMovement();
      game->sCollision();
      game->sSpawner();
      game->sLifespan();
      game->sEffects();
      game->sTimer();
    }
    game->sRender();
  }

  game->m_lastFrameTime = currentTime;
}

void Game::run() {
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop_arg(Game::mainLoop, this, 0, 1);
#else
  while (m_isRunning) {
    mainLoop(this);
  }
#endif
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

void Game::renderText() {
  const SDL_Color   scoreColor = {255, 255, 255, 255};
  const std::string scoreText  = "Score: " + std::to_string(m_score);
  const Vec2        scorePos   = {10, 10};
  TextHelpers::renderLineOfText(m_renderer, m_font_big, scoreText, scoreColor, scorePos);

  const Uint64     &timeRemaining = m_timeRemaining;
  const Uint64      minutes       = timeRemaining / 60000;
  const Uint64      seconds       = (timeRemaining % 60000) / 1000;
  const SDL_Color   timeColor     = {255, 255, 255, 255};
  const std::string timeText      = "Time: " + std::to_string(minutes) + ":" +
                               (seconds < 10 ? "0" : "") + std::to_string(seconds);
  const Vec2 timePos = {10, 40};
  TextHelpers::renderLineOfText(m_renderer, m_font_big, timeText, timeColor, timePos);

  if (m_player->cEffects->hasEffect(EffectTypes::Speed)) {
    const SDL_Color   speedBoostColor = {0, 255, 0, 255};
    const std::string speedBoostText  = "Speed Boost Active!";
    const Vec2        speedBoostPos   = {10, 90};
    TextHelpers::renderLineOfText(m_renderer, m_font_small, speedBoostText, speedBoostColor,
                                  speedBoostPos);
  };

  if (m_player->cEffects->hasEffect(EffectTypes::Slowness)) {
    const SDL_Color   slownessColor = {255, 0, 0, 255};
    const std::string slownessText  = "Slowness Active!";
    const Vec2        slownessPos   = {10, 90};
    TextHelpers::renderLineOfText(m_renderer, m_font_small, slownessText, slownessColor,
                                  slownessPos);
  };

  if (m_gameOver) {
    const SDL_Color   gameOverColor = {255, 0, 0, 255};
    const std::string gameOverText  = "Game Over!";
    const Vec2        gameOverPos   = {10, 100};
    TextHelpers::renderLineOfText(m_renderer, m_font_big, gameOverText, gameOverColor,
                                  gameOverPos);
  }
}

void Game::sRender() {
  // Clear the renderer with a black color
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

    const SDL_Color &color = entity->cShape->color;
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(m_renderer, &rect);
  }

  renderText();
  // Update the screen
  SDL_RenderPresent(m_renderer);
}

void Game::sCollision() {
  const Vec2 &windowSize = m_configManager.getGameConfig().windowSize;
  std::uniform_int_distribution<Uint64> randomSlownessDuration(5000, 10000);
  std::uniform_int_distribution<Uint64> randomSpeedBoostDuration(9000, 15000);

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

    if (entity->tag() == EntityTags::SlownessDebuff) {
      std::bitset<4> slownessCollides =
          CollisionHelpers::detectOutOfBounds(entity, windowSize);
      CollisionHelpers::enforceNonPlayerBounds(entity, slownessCollides, windowSize);
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
            otherEntity->tag() == EntityTags::SlownessDebuff) {
          std::cout << "Player collided with slowness debuff!" << std::endl;

          const Uint64 startTime = SDL_GetTicks64();
          const Uint64 duration  = randomSlownessDuration(m_randomGenerator);
          entity->cEffects->addEffect({startTime, duration, EffectTypes::Slowness});

          const EntityVector &slownessDebuffs = m_entities.getEntities("SlownessDebuff");
          const EntityVector &speedBoosts     = m_entities.getEntities("SpeedBoost");

          for (auto &slownessDebuff : slownessDebuffs) {
            slownessDebuff->destroy();
          }

          for (auto &speedBoost : speedBoosts) {
            speedBoost->destroy();
          }
        }

        if (entity->tag() == EntityTags::Player &&
            otherEntity->tag() == EntityTags::SpeedBoost) {
          std::cout << "Player collided with speed boost!" << std::endl;

          const Uint64 startTime = SDL_GetTicks64();
          const Uint64 duration  = randomSpeedBoostDuration(m_randomGenerator);
          entity->cEffects->addEffect({startTime, duration, EffectTypes::Speed});

          const EntityVector &slownessDebuffs = m_entities.getEntities("SlownessDebuff");
          const EntityVector &speedBoosts     = m_entities.getEntities("SpeedBoost");

          for (auto &slownessDebuff : slownessDebuffs) {
            slownessDebuff->destroy();
          }

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
  const PlayerConfig         &playerConfig         = m_configManager.getPlayerConfig();
  const EnemyConfig          &enemyConfig          = m_configManager.getEnemyConfig();
  const SlownessEffectConfig &slownessEffectConfig = m_configManager.getSlownessEffectConfig();
  const SpeedBoostEffectConfig &speedBoostEffectConfig =
      m_configManager.getSpeedBoostEffectConfig();

  for (std::shared_ptr<Entity> entity : m_entities.getEntities()) {
    MovementHelpers::moveSpeedBoosts(entity, speedBoostEffectConfig, m_deltaTime);
    MovementHelpers::moveEnemies(entity, enemyConfig, m_deltaTime);
    MovementHelpers::movePlayer(entity, playerConfig, m_deltaTime);
    MovementHelpers::moveSlownessDebuffs(entity, slownessEffectConfig, m_deltaTime);
  }
}

void Game::sSpawner() {
  const Uint64 ticks = SDL_GetTicks64();
  if (ticks - m_lastEnemySpawnTime < 2500) {
    return;
  }
  m_lastEnemySpawnTime = ticks;
  spawnEnemy();

  const bool hasSpeedBoost = m_player->cEffects->hasEffect(EffectTypes::Speed);
  const bool hasSlowness   = m_player->cEffects->hasEffect(EffectTypes::Slowness);

  // Spawns a speed boost with a 15% chance and while speed boost and slowness debuff are not
  // active

  std::uniform_int_distribution<int> randomChance(0, 100);
  const bool                         willSpawnSpeedBoost =
      randomChance(m_randomGenerator) < 15 && !hasSpeedBoost && !hasSlowness;

  if (willSpawnSpeedBoost) {
    spawnSpeedBoostEntity();
  }
  // Spawns a slowness debuff with a 30% chance and while slowness debuff and speed boost are
  // not active
  const bool willSpawnSlownessDebuff =
      randomChance(m_randomGenerator) < 30 && !hasSlowness && !hasSpeedBoost;
  if (willSpawnSlownessDebuff) {
    std::cout << "Slowness debuff spawned!" << std::endl;
    spawnSlownessEntity();
  }
}

void Game::sEffects() {
  const auto effects = m_player->cEffects->getEffects();
  if (effects.empty()) {
    return;
  }

  const Uint64 currentTime = SDL_GetTicks64();
  for (auto &effect : effects) {
    const bool effectExpired = currentTime - effect.startTime > effect.duration;
    if (!effectExpired) {
      return;
    }

    m_player->cEffects->removeEffect(effect.type);
    if (effect.type == EffectTypes::Speed) {
      std::cout << "Your speed boost expired. 😔" << std::endl;
    }
  }
}

void Game::sTimer() {
  const Uint64 currentTime = SDL_GetTicks64();
  const Uint64 elapsedTime = currentTime - m_lastFrameTime;

  if (m_timeRemaining < elapsedTime) {
    m_timeRemaining = 0;
    setGameOver();
    return;
  }

  m_timeRemaining -= elapsedTime;
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

    const Uint64 currentTime = SDL_GetTicks();
    const Uint64 elapsedTime = currentTime - entity->cLifespan->birthTime;
    // Calculate the lifespan percentage, ensuring it's clamped between 0 and 1
    const float lifespanPercentage =
        std::min(1.0f, static_cast<float>(elapsedTime) / entity->cLifespan->lifespan);

    const bool entityExpired = elapsedTime > entity->cLifespan->lifespan;
    if (!entityExpired) {
      const float MAX_COLOR_VALUE = 255.0f;
      const Uint8 colorValue      = std::max(
          0.0f, std::min(MAX_COLOR_VALUE, MAX_COLOR_VALUE * (1.0f - lifespanPercentage)));

      SDL_Color &color = entity->cShape->color;
      color            = {color.r, color.g, color.b, colorValue};

      continue;
    }

    if (entity->tag() == EntityTags::Enemy) {
      std::cout << "You missed an enemy! 😢" << std::endl;
      setScore(m_score - 1);
    }

    entity->destroy();
  }
}

void Game::spawnPlayer() {

  const PlayerConfig &playerConfig = m_configManager.getPlayerConfig();
  const GameConfig   &gameConfig   = m_configManager.getGameConfig();

  m_player = m_entities.addEntity(EntityTags::Player);

  std::shared_ptr<CTransform> &playerCTransform = m_player->cTransform;
  std::shared_ptr<CShape>     &playerCShape     = m_player->cShape;
  std::shared_ptr<CInput>     &playerCInput     = m_player->cInput;
  std::shared_ptr<CEffects>   &playerCEffects   = m_player->cEffects;

  // set the player's initial position to the center of the screen
  const Vec2 &windowSize = gameConfig.windowSize;

  playerCShape = std::make_shared<CShape>(m_renderer, playerConfig.shape);

  const int playerHeight = playerCShape->rect.h;
  const int playerWidth  = playerCShape->rect.w;

  const Vec2 playerPosition = windowSize / 2 - Vec2(playerWidth / 2, playerHeight / 2);
  playerCTransform          = std::make_shared<CTransform>(playerPosition, Vec2(0, 0), 0);
  playerCInput              = std::make_shared<CInput>();
  playerCEffects            = std::make_shared<CEffects>();

  std::cout << "Player entity created!" << std::endl;

  m_entities.update();
}

void Game::spawnEnemy() {

  const GameConfig &gameConfig = m_configManager.getGameConfig();
  const Vec2       &windowSize = gameConfig.windowSize;

  std::uniform_int_distribution<int> randomXPos(0, windowSize.x);
  std::uniform_int_distribution<int> randomYPos(0, windowSize.y);

  std::uniform_int_distribution<int> randomXVel(-1, 1);
  std::uniform_int_distribution<int> randomYVel(-1, 1);

  const int xPos = randomXPos(m_randomGenerator);
  const int yPos = randomYPos(m_randomGenerator);

  const int xVel = randomXVel(m_randomGenerator);
  const int yVel = randomYVel(m_randomGenerator);

  std::shared_ptr<Entity>      enemy            = m_entities.addEntity(EntityTags::Enemy);
  std::shared_ptr<CTransform> &entityCTransform = enemy->cTransform;
  std::shared_ptr<CShape>     &entityCShape     = enemy->cShape;
  std::shared_ptr<CLifespan>  &entityLifespan   = enemy->cLifespan;

  entityCTransform = std::make_shared<CTransform>(Vec2(xPos, yPos), Vec2(xVel, yVel), 0);

  const EnemyConfig &enemyConfig = m_configManager.getEnemyConfig();
  entityCShape                   = std::make_shared<CShape>(m_renderer, enemyConfig.shape);
  entityLifespan                 = std::make_shared<CLifespan>(enemyConfig.lifespan);
  bool touchesBoundary      = CollisionHelpers::detectOutOfBounds(enemy, windowSize).any();
  bool touchesOtherEntities = false;

  for (auto &entity : m_entities.getEntities()) {
    if (CollisionHelpers::calculateCollisionBetweenEntities(entity, enemy)) {
      touchesOtherEntities = true;
      break;
    }
  }

  bool isValidVelocity = !(xVel == 0 && yVel == 0);
  while (!isValidVelocity) {
    const int newXVel = randomXVel(m_randomGenerator);
    const int newYVel = randomYVel(m_randomGenerator);

    enemy->cTransform->velocity = Vec2(newXVel, newYVel);
    isValidVelocity             = !(newXVel == 0 && newYVel == 0);
  }

  bool      isValidSpawn       = !touchesBoundary && !touchesOtherEntities;
  const int MAX_SPAWN_ATTEMPTS = 10;
  int       spawnAttempt       = 1;

  while (!isValidSpawn && spawnAttempt < MAX_SPAWN_ATTEMPTS) {
    const int newX = randomXPos(m_randomGenerator);
    const int newY = randomYPos(m_randomGenerator);

    enemy->cTransform->topLeftCornerPos = Vec2(newX, newY);
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

void Game::spawnSpeedBoostEntity() {
  const GameConfig                  &gameConfig = m_configManager.getGameConfig();
  const Vec2                        &windowSize = gameConfig.windowSize;
  std::uniform_int_distribution<int> randomXPos(0, windowSize.x);
  std::uniform_int_distribution<int> randomYPos(0, windowSize.y);

  std::uniform_int_distribution<int> randomXVel(-1, 1);
  std::uniform_int_distribution<int> randomYVel(-1, 1);

  const int xPos = randomXPos(m_randomGenerator);
  const int yPos = randomYPos(m_randomGenerator);

  const int xVel = randomXVel(m_randomGenerator);
  const int yVel = randomYVel(m_randomGenerator);

  std::shared_ptr<Entity>      speedBoost       = m_entities.addEntity(EntityTags::SpeedBoost);
  std::shared_ptr<CTransform> &entityCTransform = speedBoost->cTransform;
  std::shared_ptr<CShape>     &entityCShape     = speedBoost->cShape;
  std::shared_ptr<CLifespan>  &entityLifespan   = speedBoost->cLifespan;

  const SpeedBoostEffectConfig &speedBoostEffectConfig =
      m_configManager.getSpeedBoostEffectConfig();

  entityCTransform = std::make_shared<CTransform>(Vec2(xPos, yPos), Vec2(xVel, yVel), 0);
  entityCShape     = std::make_shared<CShape>(m_renderer, speedBoostEffectConfig.shape);
  entityLifespan   = std::make_shared<CLifespan>(speedBoostEffectConfig.lifespan);

  bool touchesBoundary = CollisionHelpers::detectOutOfBounds(speedBoost, windowSize).any();
  bool touchesOtherEntities = false;

  for (auto &entity : m_entities.getEntities()) {
    if (CollisionHelpers::calculateCollisionBetweenEntities(entity, speedBoost)) {
      touchesOtherEntities = true;
      break;
    }
  }

  bool isValidVelocity = !(xVel == 0 && yVel == 0);

  while (!isValidVelocity) {
    const int newXVel = randomXVel(m_randomGenerator);
    const int newYVel = randomYVel(m_randomGenerator);

    speedBoost->cTransform->velocity = Vec2(newXVel, newYVel);
    isValidVelocity                  = !(newXVel == 0 && newYVel == 0);
  }

  bool      isValidSpawn       = !touchesBoundary && !touchesOtherEntities;
  const int MAX_SPAWN_ATTEMPTS = 10;
  int       spawnAttempt       = 1;

  while (!isValidSpawn && spawnAttempt < MAX_SPAWN_ATTEMPTS) {
    const int newXPos = randomXPos(m_randomGenerator);
    const int newYPos = randomYPos(m_randomGenerator);

    speedBoost->cTransform->topLeftCornerPos = Vec2(newXPos, newYPos);
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

void Game::spawnSlownessEntity() {
  const GameConfig                  &gameConfig = m_configManager.getGameConfig();
  const Vec2                        &windowSize = gameConfig.windowSize;
  std::uniform_int_distribution<int> randomXPos(0, windowSize.x);
  std::uniform_int_distribution<int> randomYPos(0, windowSize.y);

  std::uniform_int_distribution<int> randomXVel(-1, 1);
  std::uniform_int_distribution<int> randomYVel(-1, 1);

  const int xPos = randomXPos(m_randomGenerator);
  const int yPos = randomYPos(m_randomGenerator);

  const int xVel = randomXVel(m_randomGenerator);
  const int yVel = randomYVel(m_randomGenerator);

  std::shared_ptr<Entity> slownessEntity = m_entities.addEntity(EntityTags::SlownessDebuff);
  std::shared_ptr<CTransform> &entityCTransform = slownessEntity->cTransform;
  std::shared_ptr<CShape>     &entityCShape     = slownessEntity->cShape;
  std::shared_ptr<CLifespan>  &entityLifespan   = slownessEntity->cLifespan;

  const SlownessEffectConfig &slownessEffectConfig = m_configManager.getSlownessEffectConfig();
  std::cout << slownessEffectConfig.lifespan << std::endl;

  entityCTransform = std::make_shared<CTransform>(Vec2(xPos, yPos), Vec2(xVel, yVel), 0);
  entityCShape     = std::make_shared<CShape>(m_renderer, slownessEffectConfig.shape);
  entityLifespan   = std::make_shared<CLifespan>(slownessEffectConfig.lifespan);

  bool touchesBoundary = CollisionHelpers::detectOutOfBounds(slownessEntity, windowSize).any();
  bool touchesOtherEntities = false;

  for (auto &entity : m_entities.getEntities()) {
    if (CollisionHelpers::calculateCollisionBetweenEntities(entity, slownessEntity)) {
      touchesOtherEntities = true;
      break;
    }
  }

  bool      isValidSpawn       = !touchesBoundary && !touchesOtherEntities;
  bool      isValidVelocity    = !(xVel == 0 && yVel == 0);
  const int MAX_SPAWN_ATTEMPTS = 10;
  int       spawnAttempt       = 1;

  while (!isValidVelocity) {
    const int newXVel = randomXVel(m_randomGenerator);
    const int newYVel = randomYVel(m_randomGenerator);

    slownessEntity->cTransform->velocity = Vec2(newXVel, newYVel);
    isValidVelocity                      = !(newXVel == 0 && newYVel == 0);
  }

  while (!isValidSpawn && spawnAttempt < MAX_SPAWN_ATTEMPTS) {
    const int newXPos = randomXPos(m_randomGenerator);
    const int newYPos = randomYPos(m_randomGenerator);

    slownessEntity->cTransform->topLeftCornerPos = Vec2(newXPos, newYPos);
    touchesBoundary = CollisionHelpers::detectOutOfBounds(slownessEntity, windowSize).any();
    touchesOtherEntities = false;

    for (auto &entity : m_entities.getEntities()) {
      if (CollisionHelpers::calculateCollisionBetweenEntities(entity, slownessEntity)) {
        touchesOtherEntities = true;
        break;
      }
    }

    spawnAttempt += 1;
    isValidSpawn = !touchesBoundary && !touchesOtherEntities;
  }
  if (!isValidSpawn) {
    std::cout << "Slowness debuff could not be spawned." << std::endl;
    slownessEntity->destroy();
  }

  m_entities.update();
}

void Game::setGameOver() {
  if (m_gameOver) {
    return;
  }
  m_player->cEffects->clearEffects();
  m_gameOver = true;

  std::cout << "Game over! 😭" << std::endl;
}

void Game::setPaused(const bool paused) {
  std::cout << "Game is " << (paused ? "paused" : "unpaused") << std::endl;
  m_paused = paused;
}

void Game::setScore(const int score) {
  const int previousScore = m_score;
  m_score                 = score;

  const int diff = m_score - previousScore;

  if (m_score < 0) {
    std::cout << "Your score dipped below 0! 😬" << std::endl;
    setGameOver();
    return;
  }
  const std::string scoreChange = diff > 0 ? "increased" : "decreased";
  const std::string emoji       = diff > 0 ? "😃" : "😔";

  std::cout << "Score " << scoreChange << " by " << std::abs(diff) << " point"
            << (diff > 1 ? "s" : "") << "! " << emoji << std::endl;

  std::cout << "Your score is now " << m_score << "." << std::endl;
}