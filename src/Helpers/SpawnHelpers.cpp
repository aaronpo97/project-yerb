#include "../../includes/Helpers/SpawnHelpers.hpp"
#include "../../includes/EntityManagement/Entity.hpp"
#include "../../includes/Helpers/CollisionHelpers.hpp"
#include "../../includes/Helpers/MathHelpers.hpp"

namespace SpawnHelpers {
  Vec2 createRandomPosition(std::mt19937 &randomGenerator, const Vec2 &windowSize) {
    std::uniform_int_distribution<int> randomXPos(0, static_cast<int>(windowSize.x));
    std::uniform_int_distribution<int> randomYPos(0, static_cast<int>(windowSize.y));
    const int                          xPos = randomXPos(randomGenerator);
    const int                          yPos = randomYPos(randomGenerator);
    return {static_cast<float>(xPos), static_cast<float>(yPos)};
  };

  Vec2 createValidVelocity(std::mt19937 &randomGenerator, const int attempts) {
    std::uniform_int_distribution<int> randomVel(-1, 1);

    /*
     * 8/9 chance of generating a valid velocity on the first attempt.
     */
    const auto velocity = Vec2(static_cast<float>(randomVel(randomGenerator)),
                               static_cast<float>(randomVel(randomGenerator)))
                              .normalize();

    /*
     *  Has a (1/9)^n chance of falling back to the default vector after n attempts.
     */
    if (attempts <= 0) {
      return Vec2(1, 0).normalize();
    }

    return (velocity == Vec2(0, 0)) ? createValidVelocity(randomGenerator, attempts - 1)
                                    : velocity;
  };

  bool validateSpawnPosition(const std::shared_ptr<Entity> &entity,
                             const std::shared_ptr<Entity> &player,
                             EntityManager                 &entityManager,
                             const Vec2                    &windowSize) {
    constexpr int MIN_DISTANCE_TO_PLAYER = 40;

    const bool touchesBoundary = CollisionHelpers::detectOutOfBounds(entity, windowSize).any();

    if (touchesBoundary) {
      return false;
    }

    auto calculateDistanceSquared = [](const std::shared_ptr<Entity> &entityA,
                                       const std::shared_ptr<Entity> &entityB) -> float {
      const auto centerA = entityA->getCenterPos();
      const auto centerB = entityB->getCenterPos();
      return MathHelpers::pythagorasSquared(centerA.x - centerB.x, centerA.y - centerB.y);
    };

    const float distanceSquared = calculateDistanceSquared(player, entity);
    if (distanceSquared < MIN_DISTANCE_TO_PLAYER * MIN_DISTANCE_TO_PLAYER) {
      return false;
    }

    auto collisionCheck = [&](const std::shared_ptr<Entity> &entityToCheck) -> bool {
      return CollisionHelpers::calculateCollisionBetweenEntities(entity, entityToCheck);
    };

    const bool isCollidingWithOtherEntities =
        std::ranges::any_of(entityManager.getEntities(), collisionCheck);

    if (isCollidingWithOtherEntities) {
      return false;
    }
    return true;
  };
} // namespace SpawnHelpers

namespace SpawnHelpers::MainScene {
  std::shared_ptr<Entity> spawnPlayer(SDL_Renderer        *renderer,
                                      const ConfigManager &configManager,
                                      EntityManager       &entityManager,
                                      SurfaceManager      &surfaceManager) {

    const PlayerConfig &playerConfig = configManager.getPlayerConfig();
    const GameConfig   &gameConfig   = configManager.getGameConfig();

    const Vec2 &windowSize     = gameConfig.windowSize;
    const auto  playerHeight   = static_cast<float>(playerConfig.shape.height);
    const auto  playerWidth    = static_cast<float>(playerConfig.shape.width);
    const Vec2  centerPosition = windowSize / 2 - Vec2(playerWidth / 2, playerHeight / 2);

    const Vec2 &playerPosition = centerPosition;
    const Vec2  playerVelocity = {0, 0};

    const auto cShape     = std::make_shared<CShape>(renderer, playerConfig.shape);
    const auto cTransform = std::make_shared<CTransform>(playerPosition, playerVelocity);
    const auto cInput     = std::make_shared<CInput>();
    const auto cEffects   = std::make_shared<CEffects>();
    const auto cSprite =
        std::make_shared<CSprite>(surfaceManager.getSurface(SurfaceName::EXAMPLE));

    std::shared_ptr<Entity> player = entityManager.addEntity(EntityTags::Player);
    player->setComponent(cTransform);
    player->setComponent(cShape);
    player->setComponent(cInput);
    player->setComponent(cEffects);
    player->setComponent(cSprite);

    entityManager.update();

    return player;
  }

  void spawnEnemy(SDL_Renderer                  *renderer,
                  const ConfigManager           &configManager,
                  std::mt19937                  &randomGenerator,
                  EntityManager                 &entityManager,
                  const std::shared_ptr<Entity> &player,
                  SurfaceManager                &surfaceManager) {
    constexpr int MAX_SPAWN_ATTEMPTS = 10;

    const GameConfig  &gameConfig  = configManager.getGameConfig();
    const EnemyConfig &enemyConfig = configManager.getEnemyConfig();
    const Vec2        &windowSize  = gameConfig.windowSize;

    const Vec2 velocity = createValidVelocity(randomGenerator);
    const Vec2 position = createRandomPosition(randomGenerator, windowSize);

    const auto cTransform = std::make_shared<CTransform>(position, velocity);
    const auto cShape     = std::make_shared<CShape>(renderer, enemyConfig.shape);
    const auto cLifespan  = std::make_shared<CLifespan>(enemyConfig.lifespan);

    const auto cSprite =
        std::make_shared<CSprite>(surfaceManager.getSurface(SurfaceName::EXAMPLE));

    const std::shared_ptr<Entity> &enemy = entityManager.addEntity(EntityTags::Enemy);
    enemy->setComponent<CTransform>(cTransform);
    enemy->setComponent<CShape>(cShape);
    enemy->setComponent<CLifespan>(cLifespan);
    enemy->setComponent<CSprite>(cSprite);

    bool isValidSpawn = validateSpawnPosition(enemy, player, entityManager, windowSize);
    int  spawnAttempt = 1;

    while (!isValidSpawn && spawnAttempt < MAX_SPAWN_ATTEMPTS) {
      const auto newPosition = createRandomPosition(randomGenerator, windowSize);
      enemy->getComponent<CTransform>()->topLeftCornerPos = newPosition;
      isValidSpawn = validateSpawnPosition(enemy, player, entityManager, windowSize);
      spawnAttempt += 1;
    }

    if (!isValidSpawn) {
      enemy->destroy();
    }

    entityManager.update();
  }

  void spawnSpeedBoostEntity(SDL_Renderer                  *renderer,
                             const ConfigManager           &configManager,
                             std::mt19937                  &randomGenerator,
                             EntityManager                 &entityManager,
                             const std::shared_ptr<Entity> &player) {
    constexpr int MAX_SPAWN_ATTEMPTS = 10;

    const GameConfig        &gameConfig        = configManager.getGameConfig();
    const SpeedEffectConfig &speedEffectConfig = configManager.getSpeedEffectConfig();
    const Vec2              &windowSize        = gameConfig.windowSize;

    const Vec2 velocity = createValidVelocity(randomGenerator);
    const Vec2 position = createRandomPosition(randomGenerator, windowSize);

    const auto cTransform = std::make_shared<CTransform>(position, velocity);
    const auto cShape     = std::make_shared<CShape>(renderer, speedEffectConfig.shape);
    const auto cLifespan  = std::make_shared<CLifespan>(speedEffectConfig.lifespan);

    const auto &speedBoost = entityManager.addEntity(EntityTags::SpeedBoost);
    speedBoost->setComponent<CTransform>(cTransform);
    speedBoost->setComponent<CShape>(cShape);
    speedBoost->setComponent<CLifespan>(cLifespan);

    bool isValidSpawn = validateSpawnPosition(speedBoost, player, entityManager, windowSize);
    int  spawnAttempt = 1;

    while (!isValidSpawn && spawnAttempt < MAX_SPAWN_ATTEMPTS) {
      const auto newPosition = createRandomPosition(randomGenerator, windowSize);
      speedBoost->getComponent<CTransform>()->topLeftCornerPos = newPosition;
      isValidSpawn = validateSpawnPosition(speedBoost, player, entityManager, windowSize);
      spawnAttempt += 1;
    }

    if (!isValidSpawn) {
      speedBoost->destroy();
    }

    entityManager.update();
  }

  void spawnSlownessEntity(SDL_Renderer                  *renderer,
                           const ConfigManager           &configManager,
                           std::mt19937                  &randomGenerator,
                           EntityManager                 &entityManager,
                           const std::shared_ptr<Entity> &player) {
    constexpr int MAX_SPAWN_ATTEMPTS = 10;

    const auto &[windowSize, windowTitle, fontPath, spawnInterval] =
        configManager.getGameConfig();

    const SlownessEffectConfig &slownessEffectConfig = configManager.getSlownessEffectConfig();

    const auto velocity = createValidVelocity(randomGenerator);
    const auto position = createRandomPosition(randomGenerator, windowSize);

    const auto cTransform = std::make_shared<CTransform>(position, velocity);
    const auto cShape     = std::make_shared<CShape>(renderer, slownessEffectConfig.shape);
    const auto cLifespan  = std::make_shared<CLifespan>(slownessEffectConfig.lifespan);

    const std::shared_ptr<Entity> &slownessEntity =
        entityManager.addEntity(EntityTags::SlownessDebuff);

    slownessEntity->setComponent<CTransform>(cTransform);
    slownessEntity->setComponent<CShape>(cShape);
    slownessEntity->setComponent<CLifespan>(cLifespan);

    bool isValidSpawn =
        validateSpawnPosition(slownessEntity, player, entityManager, windowSize);
    int spawnAttempt = 1;

    while (!isValidSpawn && spawnAttempt < MAX_SPAWN_ATTEMPTS) {
      const auto newPosition = createRandomPosition(randomGenerator, windowSize);
      slownessEntity->getComponent<CTransform>()->topLeftCornerPos = newPosition;
      isValidSpawn = validateSpawnPosition(slownessEntity, player, entityManager, windowSize);
      spawnAttempt += 1;
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
    const float         wallWidth  = gameConfig.windowSize.x * 0.025f;

    const auto wallConfig = ShapeConfig(wallHeight, wallWidth, wallColor);

    constexpr size_t WALL_COUNT = 8;

    const float innerWidth  = gameConfig.windowSize.x * 0.6f;
    const float innerHeight = gameConfig.windowSize.y * 0.6f;
    const float innerStartX = (gameConfig.windowSize.x - innerWidth) / 2;
    const float innerStartY = (gameConfig.windowSize.y - innerHeight) / 2;

    const float outerWidth  = gameConfig.windowSize.x;
    const float outerHeight = gameConfig.windowSize.y;
    const float outerStartX = (gameConfig.windowSize.x - outerWidth) / 2;
    const float outerStartY = (gameConfig.windowSize.y - outerHeight) / 2;

    // Gap sizes proportional to respective rectangles
    const float innerGapSize = innerWidth * 0.15f;
    const float outerGapSize = outerWidth * 0.18f;

    for (int i = 0; i < WALL_COUNT; i++) {
      const auto shapeComponent     = std::make_shared<CShape>(renderer, wallConfig);
      const auto transformComponent = std::make_shared<CTransform>();

      Vec2 &topLeftCornerPos = transformComponent->topLeftCornerPos;

      const bool isOuterWall       = i >= 4;
      const bool isHorizontal      = (i % 2 == 0);
      const bool isOuterHorizontal = isOuterWall && isHorizontal;
      const bool isOuterVertical   = isOuterWall && !isHorizontal;
      const bool isInnerHorizontal = !isOuterWall && isHorizontal;
      const bool isInnerVertical   = !isOuterWall && !isHorizontal;

      if (isOuterHorizontal) {
        shapeComponent->rect.h = static_cast<int>(wallWidth);
        shapeComponent->rect.w = static_cast<int>(outerWidth - (2 * outerGapSize));

        topLeftCornerPos.x = outerStartX + outerGapSize;
        topLeftCornerPos.y = (i == 4) ? outerStartY : outerStartY + outerHeight - wallWidth;
      }
      if (isOuterVertical) {
        shapeComponent->rect.h = static_cast<int>(outerHeight - (2 * outerGapSize));
        shapeComponent->rect.w = static_cast<int>(wallWidth);

        topLeftCornerPos.x = (i == 5) ? outerStartX : outerStartX + outerWidth - wallWidth;
        topLeftCornerPos.y = outerStartY + outerGapSize;
      }
      if (isInnerHorizontal) {
        shapeComponent->rect.h = static_cast<int>(wallWidth);
        shapeComponent->rect.w = static_cast<int>(innerWidth - (2 * innerGapSize));

        topLeftCornerPos.x = innerStartX + innerGapSize;
        topLeftCornerPos.y = (i == 0) ? innerStartY : innerStartY + innerHeight - wallWidth;
      }
      if (isInnerVertical) {
        shapeComponent->rect.h = static_cast<int>(innerHeight - (2 * innerGapSize));
        shapeComponent->rect.w = static_cast<int>(wallWidth);

        topLeftCornerPos.x = (i == 1) ? innerStartX : innerStartX + innerWidth - wallWidth;
        topLeftCornerPos.y = innerStartY + innerGapSize;
      }

      const std::shared_ptr<Entity> wall = entityManager.addEntity(EntityTags::Wall);
      wall->setComponent(shapeComponent);
      wall->setComponent(transformComponent);
    }

    entityManager.update();
  }
  void spawnBullets(SDL_Renderer                  *renderer,
                    const ConfigManager           &configManager,
                    EntityManager                 &entityManager,
                    const std::shared_ptr<Entity> &player,
                    const Vec2                    &mousePosition) {

    const EntityVector walls = entityManager.getEntities(EntityTags::Wall);

    const auto &[lifespan, speed, shape] = configManager.getBulletConfig();

    const Vec2 &playerCenter = player->getCenterPos();
    const float playerHalfWidth =
        static_cast<float>(player->getComponent<CShape>()->rect.w) / 2;

    Vec2 direction;
    direction.x = mousePosition.x - playerCenter.x;
    direction.y = mousePosition.y - playerCenter.y;
    direction.normalize();

    const float                   bulletSpeed    = speed;
    Vec2                          bulletVelocity = direction * bulletSpeed;
    const std::shared_ptr<Entity> bullet         = entityManager.addEntity(EntityTags::Bullet);

    const float bulletHalfWidth  = shape.width / 2;
    const float bulletHalfHeight = shape.height / 2;

    const auto spawnOffset = bulletHalfWidth + static_cast<float>(playerHalfWidth);

    Vec2 bulletPos;
    // Set bullet position slightly offset from player center in the direction of travel
    bulletPos.x = playerCenter.x + direction.x * spawnOffset - bulletHalfWidth;
    bulletPos.y = playerCenter.y + direction.y * spawnOffset - bulletHalfHeight;

    const auto cTransform     = std::make_shared<CTransform>(bulletPos, bulletVelocity);
    const auto cLifespan      = std::make_shared<CLifespan>(lifespan);
    const auto cBounceTracker = std::make_shared<CBounceTracker>();
    const auto cShape         = std::make_shared<CShape>(
        renderer, ShapeConfig(shape.height, shape.width, shape.color));

    bullet->setComponent<CShape>(cShape);
    bullet->setComponent<CTransform>(cTransform);
    bullet->setComponent<CLifespan>(cLifespan);
    bullet->setComponent<CBounceTracker>(cBounceTracker);

    for (const std::shared_ptr<Entity> &wall : walls) {
      if (CollisionHelpers::calculateCollisionBetweenEntities(bullet, wall)) {
        bullet->destroy();
        break;
      }
    }

    entityManager.update();
  }

  void spawnItem(SDL_Renderer                  *renderer,
                 const ConfigManager           &configManager,
                 std::mt19937                  &randomGenerator,
                 EntityManager                 &entityManager,
                 const std::shared_ptr<Entity> &player) {
    constexpr int MAX_SPAWN_ATTEMPTS = 10;

    const GameConfig &gameConfig                          = configManager.getGameConfig();
    const auto &[spawnPercentage, lifespan, speed, shape] = configManager.getItemConfig();
    const Vec2 &windowSize                                = gameConfig.windowSize;

    const auto position   = createRandomPosition(randomGenerator, windowSize);
    const auto velocity   = Vec2(0, 0);
    const auto cTransform = std::make_shared<CTransform>(position, velocity);
    const auto cShape     = std::make_shared<CShape>(renderer, shape);
    const auto cLifespan  = std::make_shared<CLifespan>(lifespan);

    const auto &item = entityManager.addEntity(EntityTags::Item);
    item->setComponent<CTransform>(cTransform);
    item->setComponent<CShape>(cShape);
    item->setComponent<CLifespan>(cLifespan);

    bool isValidSpawn = validateSpawnPosition(item, player, entityManager, windowSize);
    int  spawnAttempt = 1;

    while (!isValidSpawn && spawnAttempt < MAX_SPAWN_ATTEMPTS) {
      const auto newPosition = createRandomPosition(randomGenerator, windowSize);
      item->getComponent<CTransform>()->topLeftCornerPos = newPosition;

      isValidSpawn = validateSpawnPosition(item, player, entityManager, windowSize);
      spawnAttempt += 1;
    }

    if (!isValidSpawn) {
      item->destroy();
    }

    entityManager.update();
  }
} // namespace SpawnHelpers::MainScene