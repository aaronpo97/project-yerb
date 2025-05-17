#include "../../../includes/GameScenes/MainScene/MainSceneSpawner.hpp"
#include "../../../includes/Helpers/CollisionHelpers.hpp"
#include "../../../includes/Helpers/SpawnHelpers.hpp"
MainSceneSpawner::MainSceneSpawner(std::mt19937   &randomGenerator,
                                   ConfigManager  &configManager,
                                   TextureManager &textureManager,
                                   EntityManager  &entityManager,
                                   SDL_Renderer   *renderer) :
    m_randomGenerator(randomGenerator),
    m_configManager(configManager),
    m_textureManager(textureManager),
    m_entityManager(entityManager),
    m_renderer(renderer) {
  std::cout << "spawner created\n";
}

std::shared_ptr<Entity> MainSceneSpawner::spawnPlayer() {
  const PlayerConfig &playerConfig = m_configManager.getPlayerConfig();
  const GameConfig   &gameConfig   = m_configManager.getGameConfig();

  const Vec2 &windowSize     = gameConfig.windowSize;
  const auto  playerHeight   = static_cast<float>(playerConfig.shape.height);
  const auto  playerWidth    = static_cast<float>(playerConfig.shape.width);
  const Vec2  centerPosition = windowSize / 2 - Vec2(playerWidth / 2, playerHeight / 2);

  const Vec2 &playerPosition = centerPosition;
  const Vec2  playerVelocity = {0, 0};

  const auto cShape     = std::make_shared<CShape>(m_renderer, playerConfig.shape);
  const auto cTransform = std::make_shared<CTransform>(playerPosition, playerVelocity);
  const auto cInput     = std::make_shared<CInput>();
  const auto cEffects   = std::make_shared<CEffects>();
  const auto cSprite =
      std::make_shared<CSprite>(m_textureManager.getTexture(TextureName::EXAMPLE));

  std::shared_ptr<Entity> player = m_entityManager.addEntity(EntityTags::Player);
  player->setComponent(cTransform);
  player->setComponent(cShape);
  player->setComponent(cInput);
  player->setComponent(cEffects);
  player->setComponent(cSprite);

  m_entityManager.update();
  return player;
}
void MainSceneSpawner::spawnEnemy(const std::shared_ptr<Entity> &player) {
  constexpr int MAX_SPAWN_ATTEMPTS = 10;

  const GameConfig  &gameConfig  = m_configManager.getGameConfig();
  const EnemyConfig &enemyConfig = m_configManager.getEnemyConfig();
  const Vec2        &windowSize  = gameConfig.windowSize;

  const Vec2 velocity = SpawnHelpers::createValidVelocity(m_randomGenerator);
  const Vec2 position = SpawnHelpers::createRandomPosition(m_randomGenerator, windowSize);

  const auto cTransform = std::make_shared<CTransform>(position, velocity);
  const auto cShape     = std::make_shared<CShape>(m_renderer, enemyConfig.shape);
  const auto cLifespan  = std::make_shared<CLifespan>(enemyConfig.lifespan);

  const auto cSprite =
      std::make_shared<CSprite>(m_textureManager.getTexture(TextureName::EXAMPLE));

  const std::shared_ptr<Entity> &enemy = m_entityManager.addEntity(EntityTags::Enemy);
  enemy->setComponent<CTransform>(cTransform);
  enemy->setComponent<CShape>(cShape);
  enemy->setComponent<CLifespan>(cLifespan);
  enemy->setComponent<CSprite>(cSprite);
  
  if (!player) {
    SDL_Log("Player missing, destroying enemy");
    enemy->destroy();
    return;
  }

  bool isValidSpawn =
      SpawnHelpers::validateSpawnPosition(enemy, player, m_entityManager, windowSize);
  int spawnAttempt = 1;

  while (!isValidSpawn && spawnAttempt < MAX_SPAWN_ATTEMPTS) {
    const auto newPosition = SpawnHelpers::createRandomPosition(m_randomGenerator, windowSize);
    enemy->getComponent<CTransform>()->topLeftCornerPos = newPosition;
    isValidSpawn =
        SpawnHelpers::validateSpawnPosition(enemy, player, m_entityManager, windowSize);
    spawnAttempt += 1;
  }

  if (!isValidSpawn) {
    enemy->destroy();
  }

  m_entityManager.update();
}
void MainSceneSpawner::spawnSpeedBoostEntity(const std::shared_ptr<Entity> &player) {
  constexpr int MAX_SPAWN_ATTEMPTS = 10;

  const GameConfig        &gameConfig        = m_configManager.getGameConfig();
  const SpeedEffectConfig &speedEffectConfig = m_configManager.getSpeedEffectConfig();
  const Vec2              &windowSize        = gameConfig.windowSize;

  const Vec2 velocity = SpawnHelpers::createValidVelocity(m_randomGenerator);
  const Vec2 position = SpawnHelpers::createRandomPosition(m_randomGenerator, windowSize);

  const auto cTransform = std::make_shared<CTransform>(position, velocity);
  const auto cShape     = std::make_shared<CShape>(m_renderer, speedEffectConfig.shape);
  const auto cLifespan  = std::make_shared<CLifespan>(speedEffectConfig.lifespan);

  const auto &speedBoost = m_entityManager.addEntity(EntityTags::SpeedBoost);
  speedBoost->setComponent<CTransform>(cTransform);
  speedBoost->setComponent<CShape>(cShape);
  speedBoost->setComponent<CLifespan>(cLifespan);

  // @todo Check for nullptr player
  if (!player) {
    SDL_Log("Player missing, destroying speed boost");
    speedBoost->destroy();
    return;
  }

  bool isValidSpawn =
      SpawnHelpers::validateSpawnPosition(speedBoost, player, m_entityManager, windowSize);
  int spawnAttempt = 1;

  while (!isValidSpawn && spawnAttempt < MAX_SPAWN_ATTEMPTS) {
    const auto newPosition = SpawnHelpers::createRandomPosition(m_randomGenerator, windowSize);
    speedBoost->getComponent<CTransform>()->topLeftCornerPos = newPosition;
    isValidSpawn =
        SpawnHelpers::validateSpawnPosition(speedBoost, player, m_entityManager, windowSize);
    spawnAttempt += 1;
  }

  if (!isValidSpawn) {
    speedBoost->destroy();
  }

  m_entityManager.update();
}
void MainSceneSpawner::spawnSlownessEntity(const std::shared_ptr<Entity> &player) {
  constexpr int MAX_SPAWN_ATTEMPTS = 10;

  const auto &[windowSize, windowTitle, fontPath, spawnInterval] =
      m_configManager.getGameConfig();

  const SlownessEffectConfig &slownessEffectConfig = m_configManager.getSlownessEffectConfig();

  const auto velocity = SpawnHelpers::createValidVelocity(m_randomGenerator);
  const auto position = SpawnHelpers::createRandomPosition(m_randomGenerator, windowSize);

  const auto cTransform = std::make_shared<CTransform>(position, velocity);
  const auto cShape     = std::make_shared<CShape>(m_renderer, slownessEffectConfig.shape);
  const auto cLifespan  = std::make_shared<CLifespan>(slownessEffectConfig.lifespan);

  const std::shared_ptr<Entity> &slownessEntity =
      m_entityManager.addEntity(EntityTags::SlownessDebuff);

  slownessEntity->setComponent<CTransform>(cTransform);
  slownessEntity->setComponent<CShape>(cShape);
  slownessEntity->setComponent<CLifespan>(cLifespan);

  if (!player) {
    SDL_Log("Player missing destroying slowness debuff");
    slownessEntity->destroy();
    return;
  }

  bool isValidSpawn = SpawnHelpers::validateSpawnPosition(
      slownessEntity, player, m_entityManager, windowSize);
  int spawnAttempt = 1;

  while (!isValidSpawn && spawnAttempt < MAX_SPAWN_ATTEMPTS) {
    const auto newPosition = SpawnHelpers::createRandomPosition(m_randomGenerator, windowSize);
    slownessEntity->getComponent<CTransform>()->topLeftCornerPos = newPosition;
    isValidSpawn = SpawnHelpers::validateSpawnPosition(
        slownessEntity, player, m_entityManager, windowSize);
    spawnAttempt += 1;
  }

  if (!isValidSpawn) {
    slownessEntity->destroy();
  }

  m_entityManager.update();
}

void MainSceneSpawner::spawnWalls() {
  const GameConfig &gameConfig = m_configManager.getGameConfig();

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
    const auto shapeComponent     = std::make_shared<CShape>(m_renderer, wallConfig);
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

    const std::shared_ptr<Entity> wall = m_entityManager.addEntity(EntityTags::Wall);
    wall->setComponent(shapeComponent);
    wall->setComponent(transformComponent);
  }

  m_entityManager.update();
}
void MainSceneSpawner::spawnBullets(const std::shared_ptr<Entity> &player,
                                    const Vec2                    &mousePosition) {

  const EntityVector walls = m_entityManager.getEntities(EntityTags::Wall);

  const auto &[lifespan, speed, shape] = m_configManager.getBulletConfig();

  if (!player) {
    SDL_Log("player missing, not creating bullet");
    return;
  }
  const Vec2 &playerCenter = player->getCenterPos();
  const float playerHalfWidth =
      static_cast<float>(player->getComponent<CShape>()->rect.w) / 2;

  Vec2 direction;
  direction.x = mousePosition.x - playerCenter.x;
  direction.y = mousePosition.y - playerCenter.y;
  direction.normalize();

  const float                   bulletSpeed    = speed;
  Vec2                          bulletVelocity = direction * bulletSpeed;
  const std::shared_ptr<Entity> bullet         = m_entityManager.addEntity(EntityTags::Bullet);

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
      m_renderer, ShapeConfig(shape.height, shape.width, shape.color));

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

  m_entityManager.update();
}

void MainSceneSpawner::spawnItem(const std::shared_ptr<Entity> &player) {
  constexpr int MAX_SPAWN_ATTEMPTS = 10;

  const GameConfig &gameConfig                          = m_configManager.getGameConfig();
  const auto &[spawnPercentage, lifespan, speed, shape] = m_configManager.getItemConfig();
  const Vec2 &windowSize                                = gameConfig.windowSize;

  const auto position   = SpawnHelpers::createRandomPosition(m_randomGenerator, windowSize);
  const auto velocity   = Vec2(0, 0);
  const auto cTransform = std::make_shared<CTransform>(position, velocity);
  const auto cShape     = std::make_shared<CShape>(m_renderer, shape);
  const auto cLifespan  = std::make_shared<CLifespan>(lifespan);

  const auto &item = m_entityManager.addEntity(EntityTags::Item);
  item->setComponent<CTransform>(cTransform);
  item->setComponent<CShape>(cShape);
  item->setComponent<CLifespan>(cLifespan);

  if (!player) {
    SDL_Log("Player missing, destroying item entity");
    item->destroy();
    return;
  }

  bool isValidSpawn =
      SpawnHelpers::validateSpawnPosition(item, player, m_entityManager, windowSize);
  int spawnAttempt = 1;

  while (!isValidSpawn && spawnAttempt < MAX_SPAWN_ATTEMPTS) {
    const auto newPosition = SpawnHelpers::createRandomPosition(m_randomGenerator, windowSize);
    item->getComponent<CTransform>()->topLeftCornerPos = newPosition;

    isValidSpawn =
        SpawnHelpers::validateSpawnPosition(item, player, m_entityManager, windowSize);
    spawnAttempt += 1;
  }

  if (!isValidSpawn) {
    item->destroy();
  }

  m_entityManager.update();
}