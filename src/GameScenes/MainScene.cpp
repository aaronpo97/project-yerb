#include <filesystem>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include "../../includes/GameScenes/MainScene.hpp"
#include "../../includes/GameScenes/MenuScene.hpp"
#include "../../includes/GameScenes/ScoreScene.hpp"
#include "../../includes/Helpers/CollisionHelpers.hpp"
#include "../../includes/Helpers/MovementHelpers.hpp"
#include "../../includes/Helpers/SpawnHelpers.hpp"
#include "../../includes/Helpers/TextHelpers.hpp"
#include "../../includes/Helpers/Vec2.hpp"

MainScene::MainScene(GameEngine *gameEngine) :
    Scene(gameEngine) {
  SDL_Renderer        *renderer      = gameEngine->getRenderer();
  const ConfigManager &configManager = gameEngine->getConfigManager();

  m_entities = EntityManager();
  m_player   = SpawnHelpers::MainScene::spawnPlayer(renderer, configManager, m_entities);

  SpawnHelpers::MainScene::spawnWalls(renderer, configManager, m_entities);

  // WASD
  registerAction(SDLK_w, "FORWARD");
  registerAction(SDLK_s, "BACKWARD");
  registerAction(SDLK_a, "LEFT");
  registerAction(SDLK_d, "RIGHT");

  // Mouse click
  registerAction(SDL_BUTTON_LEFT, "SHOOT");
  // Pause
  registerAction(SDLK_p, "PAUSE");

  // Go to menu
  registerAction(SDLK_BACKSPACE, "GO_BACK");
}

void MainScene::update() {
  const Uint64 currentTime = SDL_GetTicks64();
  m_deltaTime              = static_cast<float>(currentTime - m_lastFrameTime) / 1000.0f;

  if (!m_paused && !m_gameOver) {
    sMovement();
    sCollision();
    sSpawner();
    sLifespan();
    sEffects();
    sTimer();
  }

  sAudio();
  sRender();
  m_lastFrameTime = currentTime;

  if (m_endTriggered) {
    onEnd();
  }
}

void MainScene::sDoAction(Action &action) {
  const ActionState  &actionState        = action.getState();
  AudioSampleManager &audioSampleManager = m_gameEngine->getAudioSampleManager();

  if (m_player == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Player entity is null, cannot process action.");
    return;
  }

  if (m_player->cInput == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Player entity lacks an input component.");
    return;
  }

  bool &forward  = m_player->cInput->forward;
  bool &backward = m_player->cInput->backward;
  bool &left     = m_player->cInput->left;
  bool &right    = m_player->cInput->right;

  const bool actionStateStart = actionState == ActionState::START;

  if (action.getName() == "FORWARD") {
    forward = actionStateStart;
  }
  if (action.getName() == "BACKWARD") {
    backward = actionStateStart;
  }
  if (action.getName() == "LEFT") {
    left = actionStateStart;
  }
  if (action.getName() == "RIGHT") {
    right = actionStateStart;
  }

  if (!actionStateStart) {
    return;
  }
  if (action.getName() == "SHOOT") {
    const std::optional<Vec2> position = action.getPos();

    if (!position.has_value()) {
      SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "A mouse event was called without a position.");
      return;
    }

    const Vec2 mousePosition = *position;

    audioSampleManager.queueSample(AudioSample::Shoot, AudioPriority::STANDARD);
    SpawnHelpers::MainScene::spawnBullets(m_gameEngine->getRenderer(),
                                          m_gameEngine->getConfigManager(), m_entities,
                                          m_player, mousePosition);
  }

  if (action.getName() == "PAUSE") {
    audioSampleManager.queueSample(AudioSample::MenuSelect, AudioPriority::CRITICAL);
    m_paused = !m_paused;
  }

  if (action.getName() == "GO_BACK") {
    audioSampleManager.queueSample(AudioSample::MenuSelect, AudioPriority::CRITICAL);
    m_endTriggered = true;
  }
}

void MainScene::renderText() const {
  SDL_Renderer *renderer = m_gameEngine->getRenderer();
  TTF_Font     *fontSm   = m_gameEngine->getFontManager().getFontSm();
  TTF_Font     *fontMd   = m_gameEngine->getFontManager().getFontMd();

  constexpr SDL_Color scoreColor = {255, 255, 255, 255};
  const std::string   scoreText  = "Score: " + std::to_string(m_score);
  const Vec2          scorePos   = {10, 10};
  TextHelpers::renderLineOfText(renderer, fontMd, scoreText, scoreColor, scorePos);

  constexpr SDL_Color livesColor = {255, 255, 255, 255};
  const std::string   livesText  = "Lives: " + std::to_string(m_lives);
  const Vec2          livesPos   = {10, 40};
  TextHelpers::renderLineOfText(renderer, fontMd, livesText, livesColor, livesPos);

  const Uint64        timeRemaining = m_timeRemaining;
  const Uint64        minutes       = timeRemaining / 60000;
  const Uint64        seconds       = timeRemaining % 60000 / 1000;
  constexpr SDL_Color timeColor     = {255, 255, 255, 255};
  const std::string   timeText      = "Time: " + std::to_string(minutes) + ":" +
                               (seconds < 10 ? "0" : "") + std::to_string(seconds);
  const Vec2 timePos = {10, 70};

  TextHelpers::renderLineOfText(renderer, fontMd, timeText, timeColor, timePos);

  if (m_player->cEffects->hasEffect(EffectTypes::Speed)) {
    constexpr SDL_Color speedBoostColor = {0, 255, 0, 255};
    const std::string   speedBoostText  = "Speed Boost Active!";
    const Vec2          speedBoostPos   = {10, 120};
    TextHelpers::renderLineOfText(renderer, fontSm, speedBoostText, speedBoostColor,
                                  speedBoostPos);
  }

  if (m_player->cEffects->hasEffect(EffectTypes::Slowness)) {
    constexpr SDL_Color slownessColor = {255, 0, 0, 255};
    const std::string   slownessText  = "Slowness Active!";
    const Vec2          slownessPos   = {10, 120};
    TextHelpers::renderLineOfText(renderer, fontSm, slownessText, slownessColor, slownessPos);
  }
}

void MainScene::sRender() {
  SDL_Renderer *renderer = m_gameEngine->getRenderer();
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  for (const auto &entity : m_entities.getEntities()) {
    if (entity->cShape == nullptr) {
      continue;
    }

    SDL_Rect   &rect = entity->cShape->rect;
    const Vec2 &pos  = entity->cTransform->topLeftCornerPos;

    rect.x = static_cast<int>(pos.x);
    rect.y = static_cast<int>(pos.y);

    const auto &[r, g, b, a] = entity->cShape->color;
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, &rect);
  }

  renderText();
  // Update the screen
  SDL_RenderPresent(renderer);
}

void MainScene::sCollision() {
  using namespace CollisionHelpers::MainScene;

  const ConfigManager &configManager = m_gameEngine->getConfigManager();
  const Vec2          &windowSize    = configManager.getGameConfig().windowSize;

  AudioSampleManager &audioSampleManager = m_gameEngine->getAudioSampleManager();
  const GameState     gameState          = {.entityManager   = m_entities,
                                            .randomGenerator = m_randomGenerator,
                                            .score           = m_score,
                                            .setScore = [this](const int score) { setScore(score); },
                                            .decrementLives     = [this]() { decrementLives(); },
                                            .windowSize         = windowSize,
                                            .audioSampleManager = audioSampleManager};

  for (auto &entity : m_entities.getEntities()) {
    handleEntityBounds(entity, windowSize);
    for (auto &otherEntity : m_entities.getEntities()) {
      const CollisionPair collisionPair = {.entityA = entity, .entityB = otherEntity};
      handleEntityEntityCollision(collisionPair, gameState);
    }
  }

  m_entities.update();
}

void MainScene::sMovement() {
  const ConfigManager          &configManager        = m_gameEngine->getConfigManager();
  const PlayerConfig           &playerConfig         = configManager.getPlayerConfig();
  const EnemyConfig            &enemyConfig          = configManager.getEnemyConfig();
  const SlownessEffectConfig   &slownessEffectConfig = configManager.getSlownessEffectConfig();
  const SpeedBoostEffectConfig &speedBoostEffectConfig =
      configManager.getSpeedBoostEffectConfig();

  for (const std::shared_ptr<Entity> &entity : m_entities.getEntities()) {
    MovementHelpers::moveSpeedBoosts(entity, speedBoostEffectConfig, m_deltaTime);
    MovementHelpers::moveEnemies(entity, enemyConfig, m_deltaTime);
    MovementHelpers::movePlayer(entity, playerConfig, m_deltaTime);
    MovementHelpers::moveSlownessDebuffs(entity, slownessEffectConfig, m_deltaTime);
    MovementHelpers::moveBullets(entity, m_deltaTime);
    MovementHelpers::moveItems(entity, m_deltaTime);
  }
}

void MainScene::sSpawner() {
  const ConfigManager &configManager  = m_gameEngine->getConfigManager();
  SDL_Renderer        *renderer       = m_gameEngine->getRenderer();
  const Uint64         ticks          = SDL_GetTicks64();
  const Uint64         SPAWN_INTERVAL = configManager.getGameConfig().spawnInterval;

  if (ticks - m_lastEnemySpawnTime < SPAWN_INTERVAL) {
    return;
  }
  m_lastEnemySpawnTime = ticks;

  const EnemyConfig            &enemyConfig = configManager.getEnemyConfig();
  const SpeedBoostEffectConfig &speedBoostEffectConfig =
      configManager.getSpeedBoostEffectConfig();
  const SlownessEffectConfig &slownessEffectConfig = configManager.getSlownessEffectConfig();
  const ItemConfig           &itemConfig           = configManager.getItemConfig();

  const bool hasSpeedBasedEffect = m_player->cEffects->hasEffect(EffectTypes::Speed) ||
                                   m_player->cEffects->hasEffect(EffectTypes::Slowness);

  std::uniform_int_distribution<unsigned int> distribution(0, 100);

  std::mt19937 &randomGenerator      = m_randomGenerator;
  auto          meetsSpawnPercentage = [&randomGenerator,
                               &distribution](const unsigned int chance) -> bool {
    return distribution(randomGenerator) < chance;
  };

  const struct SpawnDecisions {
    bool enemy;
    bool speedBoost;
    bool slowness;
    bool item;
  } decisions = {.enemy      = meetsSpawnPercentage(enemyConfig.spawnPercentage),
                 .speedBoost = !hasSpeedBasedEffect &&
                               meetsSpawnPercentage(speedBoostEffectConfig.spawnPercentage),
                 .slowness = !hasSpeedBasedEffect &&
                             meetsSpawnPercentage(slownessEffectConfig.spawnPercentage),
                 .item = meetsSpawnPercentage(itemConfig.spawnPercentage)};

  if (decisions.enemy) {
    SpawnHelpers::MainScene::spawnEnemy(renderer, configManager, m_randomGenerator, m_entities,
                                        m_player);
  }

  if (decisions.speedBoost) {
    SpawnHelpers::MainScene::spawnSpeedBoostEntity(renderer, configManager, m_randomGenerator,
                                                   m_entities, m_player);
  }

  if (decisions.slowness) {
    SpawnHelpers::MainScene::spawnSlownessEntity(renderer, configManager, m_randomGenerator,
                                                 m_entities, m_player);
  }

  if (decisions.item) {
    SpawnHelpers::MainScene::spawnItem(renderer, configManager, m_randomGenerator, m_entities,
                                       m_player);
  }
}

void MainScene::sEffects() const {
  const std::vector<Effect> effects = m_player->cEffects->getEffects();
  if (effects.empty()) {
    return;
  }

  const Uint64 currentTime = SDL_GetTicks64();
  for (const auto &[startTime, duration, type] : effects) {
    const bool effectExpired = currentTime - startTime > duration;
    if (!effectExpired) {
      return;
    }

    m_player->cEffects->removeEffect(type);
  }
}

void MainScene::sTimer() {
  const Uint64 currentTime = SDL_GetTicks64();

  // Check if the timer was recently operated by comparing the current time
  // with the last frame time and the scene's start time.
  const bool   wasTimerRecentlyOperated = currentTime - m_lastFrameTime < m_SceneStartTime;
  const Uint64 elapsedTime = !wasTimerRecentlyOperated ? 0 : currentTime - m_lastFrameTime;

  if (m_timeRemaining < elapsedTime) {
    m_timeRemaining = 0;
    setGameOver();
    return;
  }

  m_timeRemaining -= elapsedTime;
}

void MainScene::sLifespan() {
  for (const auto &entity : m_entities.getEntities()) {
    const auto tag = entity->tag();
    if (tag == EntityTags::Player) {
      continue;
    }
    if (tag == EntityTags::Wall) {
      continue;
    }
    if (entity->cLifespan == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                   "Entity with ID %zu and tag %d lacks a lifespan component.", entity->id(),
                   tag);
      continue;
    }

    const Uint64 currentTime = SDL_GetTicks();
    const Uint64 elapsedTime = currentTime - entity->cLifespan->birthTime;
    // Calculate the lifespan percentage, ensuring it's clamped between 0 and 1
    const float lifespanPercentage =
        std::min(1.0f, static_cast<float>(elapsedTime) /
                           static_cast<float>(entity->cLifespan->lifespan));

    const bool entityExpired = elapsedTime > entity->cLifespan->lifespan;
    if (!entityExpired) {
      constexpr float MAX_COLOR_VALUE = 255.0f;
      const Uint8     alpha           = static_cast<Uint8>(std::max(
          0.0f, std::min(MAX_COLOR_VALUE, MAX_COLOR_VALUE * (1.0f - lifespanPercentage))));

      SDL_Color &color = entity->cShape->color;
      color            = {.r = color.r, .g = color.g, .b = color.b, .a = alpha};

      continue;
    }

    if (tag == EntityTags::Enemy) {
      setScore(m_score - 1);
    }

    entity->destroy();
  }
}

void MainScene::setGameOver() {
  if (m_gameOver) {
    return;
  }
  m_gameOver     = true;
  m_endTriggered = true;
}

void MainScene::setScore(const int score) {
  m_score = score;
  if (m_score < 0) {
    m_score = 0;
    setGameOver();
  }
}

int MainScene::getScore() const {
  return m_score;
}

void MainScene::decrementLives() {
  if (m_lives > 0) {
    m_lives--;
    return;
  }
  setGameOver();
}

void MainScene::onEnd() {
  if (!m_gameOver) {
    m_gameEngine->loadScene("Menu", std::make_shared<MenuScene>(m_gameEngine));
    return;
  }
  m_gameEngine->loadScene("ScoreScene", std::make_shared<ScoreScene>(m_gameEngine, m_score));
}

void MainScene::sAudio() {

  AudioManager       &audioManager       = m_gameEngine->getAudioManager();
  AudioSampleManager &audioSampleManager = m_gameEngine->getAudioSampleManager();

  if (audioManager.getCurrentAudioTrack() != AudioTrack::Play) {
    audioManager.playTrack(AudioTrack::Play, -1);
  }

  audioSampleManager.update(audioManager);
}