#include <filesystem>

#ifdef __EMSCRIPTEN__

#include <emscripten/emscripten.h>
#endif

#include "../includes/CollisionHelpers.h"
#include "../includes/EntityManager.h"
#include "../includes/EntityTags.h"
#include "../includes/MainScene.h"
#include "../includes/MovementHelpers.h"
#include "../includes/SpawnHelpers.h"
#include "../includes/TextHelpers.h"

MainScene::MainScene(GameEngine *gameEngine) :
    Scene(gameEngine) {

  m_entities           = EntityManager();
  auto m_renderer      = gameEngine->getRenderer();
  auto m_window        = gameEngine->getWindow();
  auto m_configManager = gameEngine->getConfigManager();
  m_player             = SpawnHelpers::spawnPlayer(m_renderer, m_configManager, m_entities);
  std::cout << "You just spawned in the game! 🎉" << std::endl;
  std::cout << "Press W to move forward, S to move backward, A to move left, D to move right, "
               "and P to pause/unpause the game."
            << std::endl;
}

void MainScene::update() {
  const Uint64 currentTime = SDL_GetTicks64();
  m_deltaTime              = (currentTime - m_lastFrameTime) / 1000.0f;
  std::cout << "Delta time: " << m_deltaTime << std::endl;

  if (!m_paused) {
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

void MainScene::sDoAction() {}

void MainScene::renderText() {

  auto m_renderer   = m_gameEngine->getRenderer();
  auto m_font_big   = m_gameEngine->getFontBig();
  auto m_font_small = m_gameEngine->getFontSmall();

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

void MainScene::sRender() {

  auto m_renderer = m_gameEngine->getRenderer();
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

void MainScene::sCollision() {
  auto        m_configManager = m_gameEngine->getConfigManager();
  const Vec2 &windowSize      = m_configManager.getGameConfig().windowSize;
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

void MainScene::sMovement() {

  auto                        m_configManager      = m_gameEngine->getConfigManager();
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

  auto         m_configManager = m_gameEngine->getConfigManager();
  auto         m_renderer      = m_gameEngine->getRenderer();
  const Uint64 ticks           = SDL_GetTicks64();
  if (ticks - m_lastEnemySpawnTime < 2500) {
    return;
  }
  m_lastEnemySpawnTime = ticks;
  SpawnHelpers::spawnEnemy(m_renderer, m_configManager, m_randomGenerator, m_entities);

  const bool hasSpeedBoost = m_player->cEffects->hasEffect(EffectTypes::Speed);
  const bool hasSlowness   = m_player->cEffects->hasEffect(EffectTypes::Slowness);

  // Spawns a speed boost with a 15% chance and while speed boost and slowness debuff are not
  // active

  std::uniform_int_distribution<int> randomChance(0, 100);
  const bool                         willSpawnSpeedBoost =
      randomChance(m_randomGenerator) < 15 && !hasSpeedBoost && !hasSlowness;

  if (willSpawnSpeedBoost) {
    SpawnHelpers::spawnSpeedBoostEntity(m_renderer, m_configManager, m_randomGenerator,
                                        m_entities);
  }
  // Spawns a slowness debuff with a 30% chance and while slowness debuff and speed boost are
  // not active
  const bool willSpawnSlownessDebuff =
      randomChance(m_randomGenerator) < 30 && !hasSlowness && !hasSpeedBoost;
  if (willSpawnSlownessDebuff) {
    std::cout << "Slowness debuff spawned!" << std::endl;
    SpawnHelpers::spawnSlownessEntity(m_renderer, m_configManager, m_randomGenerator,
                                      m_entities);
  }
}

void MainScene::sEffects() {
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

void MainScene::sTimer() {
  const Uint64 currentTime = SDL_GetTicks64();
  const Uint64 elapsedTime = currentTime - m_lastFrameTime;

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

void MainScene::setGameOver() {
  if (m_gameOver) {
    return;
  }
  m_player->cEffects->clearEffects();
  m_gameOver = true;

  std::cout << "Game over! 😭" << std::endl;
}

void MainScene::setScore(const int score) {
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

void MainScene::onEnd() {
  m_entities = EntityManager();
  m_gameEngine->switchScene("MainMenu");
}