#include "../../includes/Helpers/SpawnHelpers.hpp"
#include "../../includes/EntityManagement/Entity.hpp"
#include "../../includes/Helpers/CollisionHelpers.hpp"
#include "../../includes/Helpers/MathHelpers.hpp"

namespace SpawnHelpers {
  std::shared_ptr<Entity> spawnPlayer(SDL_Renderer        *renderer,
                                      const ConfigManager &configManager,
                                      EntityManager       &entityManager) {

    const PlayerConfig &playerConfig = configManager.getPlayerConfig();
    const GameConfig   &gameConfig   = configManager.getGameConfig();

    std::shared_ptr<Entity> player = entityManager.addEntity(EntityTags::Player);

    std::shared_ptr<CTransform> &playerCTransform = player->cTransform;
    std::shared_ptr<CShape>     &playerCShape     = player->cShape;
    std::shared_ptr<CInput>     &playerCInput     = player->cInput;
    std::shared_ptr<CEffects>   &playerCEffects   = player->cEffects;

    // set the player's initial position to the center of the screen
    const Vec2 &windowSize = gameConfig.windowSize;

    playerCShape = std::make_shared<CShape>(renderer, playerConfig.shape);

    const auto playerHeight = static_cast<float>(playerCShape->rect.h);
    const auto playerWidth  = static_cast<float>(playerCShape->rect.w);

    const Vec2 playerPosition = windowSize / 2 - Vec2(playerWidth / 2, playerHeight / 2);
    playerCTransform          = std::make_shared<CTransform>(playerPosition, Vec2(0, 0), 0);
    playerCInput              = std::make_shared<CInput>();
    playerCEffects            = std::make_shared<CEffects>();

    entityManager.update();

    return player;
  }

  void spawnEnemy(SDL_Renderer                  *renderer,
                  const ConfigManager           &configManager,
                  std::mt19937                  &randomGenerator,
                  EntityManager                 &entityManager,
                  const std::shared_ptr<Entity> &player) {

    const GameConfig &gameConfig = configManager.getGameConfig();
    const Vec2       &windowSize = gameConfig.windowSize;

    std::uniform_int_distribution<int> randomXPos(0, static_cast<int>(windowSize.x));
    std::uniform_int_distribution<int> randomYPos(0, static_cast<int>(windowSize.y));

    std::uniform_int_distribution<int> randomXVel(-1, 1);
    std::uniform_int_distribution<int> randomYVel(-1, 1);

    const int xPos = randomXPos(randomGenerator);
    const int yPos = randomYPos(randomGenerator);

    const int xVel = randomXVel(randomGenerator);
    const int yVel = randomYVel(randomGenerator);

    const std::shared_ptr<Entity> enemy           = entityManager.addEntity(EntityTags::Enemy);
    std::shared_ptr<CTransform>  &enemyCTransform = enemy->cTransform;
    std::shared_ptr<CShape>      &enemyCShape     = enemy->cShape;
    std::shared_ptr<CLifespan>   &enemyCLifespan  = enemy->cLifespan;

    const auto enemyPosition = Vec2(static_cast<float>(xPos), static_cast<float>(yPos));
    const auto enemyVelocity = Vec2(static_cast<float>(xVel), static_cast<float>(yVel));
    enemyCTransform          = std::make_shared<CTransform>(enemyPosition, enemyVelocity, 0);

    const EnemyConfig &enemyConfig = configManager.getEnemyConfig();
    enemyCShape                    = std::make_shared<CShape>(renderer, enemyConfig.shape);
    enemyCLifespan                 = std::make_shared<CLifespan>(enemyConfig.lifespan);
    bool touchesBoundary      = CollisionHelpers::detectOutOfBounds(enemy, windowSize).any();
    bool touchesOtherEntities = false;

    for (auto &entity : entityManager.getEntities()) {
      if (CollisionHelpers::calculateCollisionBetweenEntities(entity, enemy)) {
        touchesOtherEntities = true;
        break;
      }
    }

    bool isValidVelocity = enemyVelocity != Vec2(0, 0);

    while (!isValidVelocity) {
      const int newXVel = randomXVel(randomGenerator);
      const int newYVel = randomYVel(randomGenerator);

      const auto newVelocity = Vec2(static_cast<float>(newXVel), static_cast<float>(newYVel));
      enemy->cTransform->velocity = newVelocity;
      isValidVelocity             = newVelocity != Vec2(0, 0);
    }

    auto calculateDistanceSquared = [&](const std::shared_ptr<Entity> &entityA,
                                        const std::shared_ptr<Entity> &entityB) -> float {
      const auto centerA = entityA->getCenterPos();
      const auto centerB = entityB->getCenterPos();
      const auto distanceSquared =
          MathHelpers::pythagorasSquared(centerA.x - centerB.x, centerA.y - centerB.y);

      return distanceSquared;
    };

    constexpr int MIN_DISTANCE_TO_PLAYER = 40;
    // Calculate squared distance between enemy and player

    const float distanceSquared = calculateDistanceSquared(player, enemy);

    bool tooCloseToPlayer = distanceSquared < MIN_DISTANCE_TO_PLAYER * MIN_DISTANCE_TO_PLAYER;

    bool isValidSpawn = !touchesBoundary && !touchesOtherEntities && !tooCloseToPlayer;
    constexpr int MAX_SPAWN_ATTEMPTS = 10;
    int           spawnAttempt       = 1;

    while (!isValidSpawn && spawnAttempt < MAX_SPAWN_ATTEMPTS) {
      const int newX = randomXPos(randomGenerator);
      const int newY = randomYPos(randomGenerator);

      enemy->cTransform->topLeftCornerPos =
          Vec2(static_cast<float>(newX), static_cast<float>(newY));
      const float newDistanceSquared = calculateDistanceSquared(enemy, player);

      touchesBoundary      = CollisionHelpers::detectOutOfBounds(enemy, windowSize).any();
      touchesOtherEntities = false;
      tooCloseToPlayer = newDistanceSquared < MIN_DISTANCE_TO_PLAYER * MIN_DISTANCE_TO_PLAYER;

      for (auto &entity : entityManager.getEntities()) {
        if (CollisionHelpers::calculateCollisionBetweenEntities(entity, enemy)) {
          touchesOtherEntities = true;
          break;
        }
      }

      spawnAttempt += 1;
      isValidSpawn = !touchesBoundary && !touchesOtherEntities && !tooCloseToPlayer;
    }

    if (!isValidSpawn) {
      enemy->destroy();
    }

    entityManager.update();
  }

  void spawnSpeedBoostEntity(SDL_Renderer        *renderer,
                             const ConfigManager &configManager,
                             std::mt19937        &randomGenerator,
                             EntityManager       &entityManager) {
    const GameConfig                  &gameConfig = configManager.getGameConfig();
    const Vec2                        &windowSize = gameConfig.windowSize;
    std::uniform_int_distribution<int> randomXPos(0, static_cast<int>(windowSize.x));
    std::uniform_int_distribution<int> randomYPos(0, static_cast<int>(windowSize.y));

    std::uniform_int_distribution<int> randomXVel(-1, 1);
    std::uniform_int_distribution<int> randomYVel(-1, 1);

    const int  positionX = randomXPos(randomGenerator);
    const int  positionY = randomYPos(randomGenerator);
    const auto position  = Vec2(static_cast<float>(positionX), static_cast<float>(positionY));

    const int  velocityX = randomXVel(randomGenerator);
    const int  velocityY = randomYVel(randomGenerator);
    const auto velocity  = Vec2(static_cast<float>(velocityX), static_cast<float>(velocityY));

    const std::shared_ptr<Entity> speedBoost = entityManager.addEntity(EntityTags::SpeedBoost);
    std::shared_ptr<CTransform>  &entityCTransform = speedBoost->cTransform;
    std::shared_ptr<CShape>      &entityCShape     = speedBoost->cShape;
    std::shared_ptr<CLifespan>   &entityLifespan   = speedBoost->cLifespan;

    const auto &[spawnPercentage, lifespan, speed, shape] =
        configManager.getSpeedBoostEffectConfig();

    entityCTransform = std::make_shared<CTransform>(position, velocity, 0);
    entityCShape     = std::make_shared<CShape>(renderer, shape);
    entityLifespan   = std::make_shared<CLifespan>(lifespan);

    bool touchesBoundary = CollisionHelpers::detectOutOfBounds(speedBoost, windowSize).any();
    bool touchesOtherEntities = false;

    for (auto &entity : entityManager.getEntities()) {
      if (CollisionHelpers::calculateCollisionBetweenEntities(entity, speedBoost)) {
        touchesOtherEntities = true;
        break;
      }
    }

    bool isValidVelocity = velocity != Vec2(0, 0);

    while (!isValidVelocity) {
      const int  newVelocityX = randomXVel(randomGenerator);
      const int  newVelocityY = randomYVel(randomGenerator);
      const auto newVelocity =
          Vec2(static_cast<float>(newVelocityX), static_cast<float>(newVelocityY));

      speedBoost->cTransform->velocity = newVelocity;
      isValidVelocity                  = newVelocity != Vec2(0, 0);
    }

    bool          isValidSpawn       = !touchesBoundary && !touchesOtherEntities;
    constexpr int MAX_SPAWN_ATTEMPTS = 10;
    int           spawnAttempt       = 1;

    while (!isValidSpawn && spawnAttempt < MAX_SPAWN_ATTEMPTS) {
      const int  newPositionX = randomXPos(randomGenerator);
      const int  newPositionY = randomYPos(randomGenerator);
      const auto newPosition =
          Vec2(static_cast<float>(newPositionX), static_cast<float>(newPositionY));

      speedBoost->cTransform->topLeftCornerPos = newPosition;
      touchesBoundary      = CollisionHelpers::detectOutOfBounds(speedBoost, windowSize).any();
      touchesOtherEntities = false;

      for (auto &entity : entityManager.getEntities()) {
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
    entityManager.update();
  }

  void spawnSlownessEntity(SDL_Renderer        *renderer,
                           const ConfigManager &configManager,
                           std::mt19937        &randomGenerator,
                           EntityManager       &entityManager) {
    const GameConfig &gameConfig = configManager.getGameConfig();
    const Vec2       &windowSize = gameConfig.windowSize;

    std::uniform_int_distribution<int> randomXPos(0, static_cast<int>(windowSize.x));
    std::uniform_int_distribution<int> randomYPos(0, static_cast<int>(windowSize.y));

    std::uniform_int_distribution<int> randomXVel(-1, 1);
    std::uniform_int_distribution<int> randomYVel(-1, 1);

    const int  positionX = randomXPos(randomGenerator);
    const int  positionY = randomYPos(randomGenerator);
    const auto position  = Vec2(static_cast<float>(positionX), static_cast<float>(positionY));

    const int  velocityX = randomXVel(randomGenerator);
    const int  velocityY = randomYVel(randomGenerator);
    const auto velocity  = Vec2(static_cast<float>(velocityX), static_cast<float>(velocityY));

    const std::shared_ptr<Entity> slownessEntity =
        entityManager.addEntity(EntityTags::SlownessDebuff);
    std::shared_ptr<CTransform> &entityCTransform = slownessEntity->cTransform;
    std::shared_ptr<CShape>     &entityCShape     = slownessEntity->cShape;
    std::shared_ptr<CLifespan>  &entityLifespan   = slownessEntity->cLifespan;

    const auto &[spawnPercentage, lifespan, speed, shape] =
        configManager.getSlownessEffectConfig();

    entityCTransform = std::make_shared<CTransform>(position, velocity, 0);
    entityCShape     = std::make_shared<CShape>(renderer, shape);
    entityLifespan   = std::make_shared<CLifespan>(lifespan);

    bool touchesBoundary =
        CollisionHelpers::detectOutOfBounds(slownessEntity, windowSize).any();
    bool touchesOtherEntities = false;

    for (auto &entity : entityManager.getEntities()) {
      if (CollisionHelpers::calculateCollisionBetweenEntities(entity, slownessEntity)) {
        touchesOtherEntities = true;
        break;
      }
    }

    bool isValidSpawn    = !touchesBoundary && !touchesOtherEntities;
    bool isValidVelocity = velocity != Vec2(0, 0);
    int  spawnAttempt    = 1;

    constexpr int MAX_SPAWN_ATTEMPTS = 10;
    while (!isValidVelocity) {
      const int  newXVel     = randomXVel(randomGenerator);
      const int  newYVel     = randomYVel(randomGenerator);
      const auto newVelocity = Vec2(static_cast<float>(newXVel), static_cast<float>(newYVel));

      slownessEntity->cTransform->velocity = newVelocity;
      isValidVelocity                      = newVelocity != Vec2(0, 0);
    }

    while (!isValidSpawn && spawnAttempt < MAX_SPAWN_ATTEMPTS) {
      const int  newXPos = randomXPos(randomGenerator);
      const int  newYPos = randomYPos(randomGenerator);
      const auto newPos  = Vec2(static_cast<float>(newXPos), static_cast<float>(newYPos));

      slownessEntity->cTransform->topLeftCornerPos = newPos;
      touchesBoundary = CollisionHelpers::detectOutOfBounds(slownessEntity, windowSize).any();
      touchesOtherEntities = false;

      for (auto &entity : entityManager.getEntities()) {
        if (CollisionHelpers::calculateCollisionBetweenEntities(entity, slownessEntity)) {
          touchesOtherEntities = true;
          break;
        }
      }

      spawnAttempt += 1;
      isValidSpawn = !touchesBoundary && !touchesOtherEntities;
    }
    if (!isValidSpawn) {
      slownessEntity->destroy();
    }

    entityManager.update();
  }

  void spawnWalls(SDL_Renderer        *renderer,
                  const ConfigManager &configManager,
                  EntityManager       &entityManager) {

    const GameConfig &gameConfig = configManager.getGameConfig();

    constexpr SDL_Color wallColor  = {.r = 176, .g = 196, .b = 222, .a = 255};
    const float         wallHeight = gameConfig.windowSize.y * 0.6f;
    const float         wallWidth  = gameConfig.windowSize.x * 0.02f;

    const auto wallConfig = ShapeConfig(wallHeight, wallWidth, wallColor);

    auto wallVelocity     = Vec2(0, 0);
    auto wallLeftPosition = Vec2(400, 0);
    auto wallRightPosition =
        Vec2(gameConfig.windowSize.x * 0.7f, gameConfig.windowSize.y - wallHeight);

    const std::shared_ptr<Entity> wallLeft  = entityManager.addEntity(EntityTags::Wall);
    const std::shared_ptr<Entity> wallRight = entityManager.addEntity(EntityTags::Wall);

    wallLeft->cShape     = std::make_shared<CShape>(renderer, wallConfig);
    wallLeft->cTransform = std::make_shared<CTransform>(wallLeftPosition, wallVelocity, 0);

    wallRight->cShape     = std::make_shared<CShape>(renderer, wallConfig);
    wallRight->cTransform = std::make_shared<CTransform>(wallRightPosition, wallVelocity, 0);
  }

  void spawnBullets(SDL_Renderer                  *renderer,
                    const ConfigManager           &configManager,
                    EntityManager                 &entityManager,
                    const std::shared_ptr<Entity> &player,
                    const Vec2                    &mousePosition) {

    const EntityVector walls = entityManager.getEntities(EntityTags::Wall);

    const auto &[lifespan, speed, shape] = configManager.getBulletConfig();

    const Vec2 &playerCenter = player->getCenterPos();

    Vec2 direction;
    direction.x = mousePosition.x - playerCenter.x;
    direction.y = mousePosition.y - playerCenter.y;
    direction.normalize();

    const float                   bulletSpeed    = speed;
    Vec2                          bulletVelocity = direction * bulletSpeed;
    const std::shared_ptr<Entity> bullet         = entityManager.addEntity(EntityTags::Bullet);

    bullet->cShape = std::make_shared<CShape>(
        renderer, ShapeConfig(shape.height, shape.width, shape.color));

    const int  playerHalfWidth = player->cShape->rect.w / 2;
    const int  bulletHalfWidth = bullet->cShape->rect.w / 2;
    const auto spawnOffset     = static_cast<float>(bulletHalfWidth + playerHalfWidth);
    Vec2       bulletPos;
    // Set bullet position slightly offset from player center in the direction of travel
    bulletPos.x = playerCenter.x + direction.x * spawnOffset -
                  static_cast<float>(bullet->cShape->rect.w) / 2;
    bulletPos.y = playerCenter.y + direction.y * spawnOffset -
                  static_cast<float>(bullet->cShape->rect.h) / 2;

    bullet->cTransform = std::make_shared<CTransform>(bulletPos, bulletVelocity, 0);
    bullet->cLifespan  = std::make_shared<CLifespan>(lifespan);

    for (const std::shared_ptr<Entity> &wall : walls) {
      if (CollisionHelpers::calculateCollisionBetweenEntities(bullet, wall)) {
        bullet->destroy();
        break;
      }
    }

    entityManager.update();
  }

  void spawnItem(SDL_Renderer        *renderer,
                 const ConfigManager &configManager,
                 std::mt19937        &randomGenerator,
                 EntityManager       &entityManager) {
    const GameConfig &gameConfig                          = configManager.getGameConfig();
    const auto &[spawnPercentage, lifespan, speed, shape] = configManager.getItemConfig();
    const Vec2 &windowSize                                = gameConfig.windowSize;

    std::uniform_int_distribution<int> randomXPos(0, static_cast<int>(windowSize.x));
    std::uniform_int_distribution<int> randomYPos(0, static_cast<int>(windowSize.y));

    const int  positionX = randomXPos(randomGenerator);
    const int  positionY = randomYPos(randomGenerator);
    const auto position  = Vec2(static_cast<float>(positionX), static_cast<float>(positionY));
    const auto velocity  = Vec2(0, 0);

    const std::shared_ptr<Entity> item             = entityManager.addEntity(EntityTags::Item);
    std::shared_ptr<CTransform>  &entityCTransform = item->cTransform;
    std::shared_ptr<CShape>      &entityCShape     = item->cShape;
    std::shared_ptr<CLifespan>   &entityLifespan   = item->cLifespan;

    entityCTransform = std::make_shared<CTransform>(position, velocity, 0);
    entityLifespan   = std::make_shared<CLifespan>(lifespan);
    entityCShape     = std::make_shared<CShape>(renderer, shape);

    bool touchesBoundary      = CollisionHelpers::detectOutOfBounds(item, windowSize).any();
    bool touchesOtherEntities = false;

    for (auto &entity : entityManager.getEntities()) {
      if (CollisionHelpers::calculateCollisionBetweenEntities(entity, item)) {
        touchesOtherEntities = true;
        break;
      }
    }

    bool          isValidSpawn       = !touchesBoundary && !touchesOtherEntities;
    constexpr int MAX_SPAWN_ATTEMPTS = 10;
    int           spawnAttempt       = 1;

    while (!isValidSpawn && spawnAttempt < MAX_SPAWN_ATTEMPTS) {
      const int newXPos = randomXPos(randomGenerator);
      const int newYPos = randomYPos(randomGenerator);

      const auto newPosition = Vec2(static_cast<float>(newXPos), static_cast<float>(newYPos));

      item->cTransform->topLeftCornerPos = newPosition;
      touchesBoundary      = CollisionHelpers::detectOutOfBounds(item, windowSize).any();
      touchesOtherEntities = false;

      for (auto &entity : entityManager.getEntities()) {
        if (CollisionHelpers::calculateCollisionBetweenEntities(entity, item)) {
          touchesOtherEntities = true;
          break;
        }
      }

      spawnAttempt += 1;
      isValidSpawn = !touchesBoundary && !touchesOtherEntities;
    }

    if (!isValidSpawn) {
      item->destroy();
    }

    entityManager.update();
  }
} // namespace SpawnHelpers