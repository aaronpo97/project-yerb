#include <filesystem>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include "../../includes/EntityManagement/EntityTags.hpp"
#include "../../includes/GameScenes/MainScene.hpp"
#include "../../includes/GameScenes/MenuScene.hpp"
#include "../../includes/GameScenes/ScoreScene.hpp"
#include "../../includes/Helpers/CollisionHelpers.hpp"
#include "../../includes/Helpers/MovementHelpers.hpp"
#include "../../includes/Helpers/SpawnHelpers.hpp"
#include "../../includes/Helpers/TextHelpers.hpp"
#include "../../includes/Helpers/Vec2.hpp"
#include <memory>

MainScene::MainScene(GameEngine *gameEngine) :
    Scene(gameEngine) {
  SDL_Renderer  *renderer      = gameEngine->getRenderer();
  ConfigManager &configManager = gameEngine->getConfigManager();

  m_entities = EntityManager();
  m_player   = SpawnHelpers::spawnPlayer(renderer, configManager, m_entities);

  // WASD
  registerAction(SDLK_w, "FORWARD");
  registerAction(SDLK_s, "BACKWARD");
  registerAction(SDLK_a, "LEFT");
  registerAction(SDLK_d, "RIGHT");

  // Pause
  registerAction(SDLK_p, "PAUSE");

  // Go to menu
  registerAction(SDLK_BACKSPACE, "GO_BACK");
};

void MainScene::update() {
  const Uint64 currentTime = SDL_GetTicks64();
  m_deltaTime              = (currentTime - m_lastFrameTime) / 1000.0f;

  if (!m_paused && !m_gameOver) {
    sCollision();
    sMovement();
    sSpawner();
    sLifespan();
    sEffects();
    sTimer();
  }
  sRender();

  m_lastFrameTime = currentTime;
}

void MainScene::sDoAction(Action &action) {
  const std::string &actionName  = action.getName();
  const ActionState &actionState = action.getState();

  if (m_player == nullptr) {
    std::cerr << "Player entity is null." << std::endl;
    return;
  }

  if (m_player->cInput == nullptr) {
    std::cerr << "Player entity lacks an input component." << std::endl;
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

  if (action.getName() == "PAUSE") {
    m_paused = !m_paused;
  }

  if (action.getName() == "GO_BACK") {
    onEnd();
  }
}

void MainScene::renderText() {
  SDL_Renderer *renderer = m_gameEngine->getRenderer();
  TTF_Font     *fontMd   = m_gameEngine->getFontMd();
  TTF_Font     *fontSm   = m_gameEngine->getFontSm();

  const SDL_Color   scoreColor = {255, 255, 255, 255};
  const std::string scoreText  = "Score: " + std::to_string(m_score);
  const Vec2        scorePos   = {10, 10};
  TextHelpers::renderLineOfText(renderer, fontMd, scoreText, scoreColor, scorePos);

  const Uint64      timeRemaining = m_timeRemaining;
  const Uint64      minutes       = timeRemaining / 60000;
  const Uint64      seconds       = (timeRemaining % 60000) / 1000;
  const SDL_Color   timeColor     = {255, 255, 255, 255};
  const std::string timeText      = "Time: " + std::to_string(minutes) + ":" +
                               (seconds < 10 ? "0" : "") + std::to_string(seconds);
  const Vec2 timePos = {10, 40};
  TextHelpers::renderLineOfText(renderer, fontMd, timeText, timeColor, timePos);

  if (m_player->cEffects->hasEffect(EffectTypes::Speed)) {
    const SDL_Color   speedBoostColor = {0, 255, 0, 255};
    const std::string speedBoostText  = "Speed Boost Active!";
    const Vec2        speedBoostPos   = {10, 90};
    TextHelpers::renderLineOfText(renderer, fontSm, speedBoostText, speedBoostColor,
                                  speedBoostPos);
  };

  if (m_player->cEffects->hasEffect(EffectTypes::Slowness)) {
    const SDL_Color   slownessColor = {255, 0, 0, 255};
    const std::string slownessText  = "Slowness Active!";
    const Vec2        slownessPos   = {10, 90};
    TextHelpers::renderLineOfText(renderer, fontSm, slownessText, slownessColor, slownessPos);
  };
}

void MainScene::sRender() {
  SDL_Renderer *renderer = m_gameEngine->getRenderer();
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  for (auto &entity : m_entities.getEntities()) {
    if (entity->cShape == nullptr) {
      continue;
    }

    SDL_Rect &rect = entity->cShape->rect;
    Vec2     &pos  = entity->cTransform->topLeftCornerPos;

    rect.x = static_cast<int>(pos.x);
    rect.y = static_cast<int>(pos.y);

    const SDL_Color &color = entity->cShape->color;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
  }

  renderText();
  // Update the screen
  SDL_RenderPresent(renderer);
}

void MainScene::sCollision() {
  const ConfigManager &m_configManager = m_gameEngine->getConfigManager();
  const Vec2          &windowSize      = m_configManager.getGameConfig().windowSize;
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
          setScore(m_score + 1);
          otherEntity->destroy();
        }

        if (entity->tag() == EntityTags::Player &&
            otherEntity->tag() == EntityTags::SlownessDebuff) {

          const Uint64 startTime = SDL_GetTicks64();
          const Uint64 duration  = randomSlownessDuration(m_randomGenerator);
          entity->cEffects->addEffect(
              {.startTime = startTime, .duration = duration, .type = EffectTypes::Slowness});

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

          const Uint64 startTime = SDL_GetTicks64();
          const Uint64 duration  = randomSpeedBoostDuration(m_randomGenerator);
          entity->cEffects->addEffect(
              {.startTime = startTime, .duration = duration, .type = EffectTypes::Speed});

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

void MainScene::sMovement() {
  const ConfigManager        &m_configManager      = m_gameEngine->getConfigManager();
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

void MainScene::sSpawner() {
  ConfigManager m_configManager = m_gameEngine->getConfigManager();
  SDL_Renderer *renderer        = m_gameEngine->getRenderer();
  const Uint64  ticks           = SDL_GetTicks64();
  if (ticks - m_lastEnemySpawnTime < 2500) {
    return;
  }
  m_lastEnemySpawnTime = ticks;
  SpawnHelpers::spawnEnemy(renderer, m_configManager, m_randomGenerator, m_entities);

  const bool hasSpeedBoost = m_player->cEffects->hasEffect(EffectTypes::Speed);
  const bool hasSlowness   = m_player->cEffects->hasEffect(EffectTypes::Slowness);

  // Spawns a speed boost with a 15% chance and while speed boost and slowness debuff are not
  // active

  std::uniform_int_distribution<int> randomChance(0, 100);
  const bool                         willSpawnSpeedBoost =
      randomChance(m_randomGenerator) < 15 && !hasSpeedBoost && !hasSlowness;

  if (willSpawnSpeedBoost) {
    SpawnHelpers::spawnSpeedBoostEntity(renderer, m_configManager, m_randomGenerator,
                                        m_entities);
  }
  // Spawns a slowness debuff with a 30% chance and while slowness debuff and speed boost are
  // not active
  const bool willSpawnSlownessDebuff =
      randomChance(m_randomGenerator) < 30 && !hasSlowness && !hasSpeedBoost;
  if (willSpawnSlownessDebuff) {
    SpawnHelpers::spawnSlownessEntity(renderer, m_configManager, m_randomGenerator,
                                      m_entities);
  }
}

void MainScene::sEffects() {
  const std::vector<Effect> effects = m_player->cEffects->getEffects();
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
      const Uint8 alpha           = std::max(
          0.0f, std::min(MAX_COLOR_VALUE, MAX_COLOR_VALUE * (1.0f - lifespanPercentage)));

      SDL_Color &color = entity->cShape->color;
      color            = {.r = color.r, .g = color.g, .b = color.b, .a = alpha};

      continue;
    }

    if (entity->tag() == EntityTags::Enemy) {
      setScore(m_score - 1);
    }

    entity->destroy();
  }
}

void MainScene::setGameOver() {
  if (m_gameOver) {
    return;
  }
  m_gameOver = true;
  onEnd();
}

void MainScene::setScore(const int score) {
  m_score = score;
  if (m_score < 0) {
    m_score = 0;
    setGameOver();
    return;
  }
}

void MainScene::onEnd() {
  if (!m_gameOver) {
    m_gameEngine->loadScene("Menu", std::make_shared<MenuScene>(m_gameEngine));
    return;
  }
  m_gameEngine->loadScene("ScoreScene", std::make_shared<ScoreScene>(m_gameEngine, m_score));
}