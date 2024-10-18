#include "../includes/SpawnHelpers.h"
#include "../includes/CollisionHelpers.h"
#include "../includes/ConfigManager.h"
#include "../includes/EntityManager.h"
#include "../includes/EntityTags.h"
#include <iostream>

namespace SpawnHelpers {
  std::shared_ptr<Entity> spawnPlayer(SDL_Renderer  *m_renderer,
                                      ConfigManager &m_configManager,
                                      EntityManager &m_entities) {

    const PlayerConfig &playerConfig = m_configManager.getPlayerConfig();
    const GameConfig   &gameConfig   = m_configManager.getGameConfig();

    std::shared_ptr<Entity> m_player = m_entities.addEntity(EntityTags::Player);

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

    return m_player;
  }

  void spawnEnemy(SDL_Renderer  *m_renderer,
                  ConfigManager &m_configManager,
                  std::mt19937  &m_randomGenerator,
                  EntityManager &m_entities) {

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

  void spawnSpeedBoostEntity(SDL_Renderer  *m_renderer,
                             ConfigManager &m_configManager,
                             std::mt19937  &m_randomGenerator,
                             EntityManager &m_entities) {
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

    std::shared_ptr<Entity>      speedBoost = m_entities.addEntity(EntityTags::SpeedBoost);
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

  void spawnSlownessEntity(SDL_Renderer        *m_renderer,
                           const ConfigManager &m_configManager,
                           std::mt19937        &m_randomGenerator,
                           EntityManager       &m_entities) {
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

    const SlownessEffectConfig &slownessEffectConfig =
        m_configManager.getSlownessEffectConfig();
    std::cout << slownessEffectConfig.lifespan << std::endl;

    entityCTransform = std::make_shared<CTransform>(Vec2(xPos, yPos), Vec2(xVel, yVel), 0);
    entityCShape     = std::make_shared<CShape>(m_renderer, slownessEffectConfig.shape);
    entityLifespan   = std::make_shared<CLifespan>(slownessEffectConfig.lifespan);

    bool touchesBoundary =
        CollisionHelpers::detectOutOfBounds(slownessEntity, windowSize).any();
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
} // namespace SpawnHelpers
