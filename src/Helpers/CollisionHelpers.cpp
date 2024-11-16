#include "../../includes/Helpers/CollisionHelpers.hpp"
#include "../../includes/EntityManagement/Entity.hpp"
#include "../../includes/GameScenes/MainScene.hpp"
#include "../../includes/Helpers/EntityHelpers.hpp"

#include <bitset>

enum Boundaries : Uint8 { TOP, BOTTOM, LEFT, RIGHT };
enum RelativePosition : Uint8 { ABOVE, BELOW, LEFT_OF, RIGHT_OF };

inline bool hasNullComponentPointers(const std::shared_ptr<Entity> &entity) {
  const bool cTransformIsNullPtr = !entity->cTransform;
  return cTransformIsNullPtr;
}

namespace CollisionHelpers {
  std::bitset<4> detectOutOfBounds(const std::shared_ptr<Entity> &entity,
                                   const Vec2                    &window_size) {
    if (hasNullComponentPointers(entity)) {
      SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                   "Entity with ID %zu and tag %u lacks a transform or collision component.",
                   entity->id(), entity->tag());
      return std::bitset<4>({});
    }

    const std::shared_ptr<CShape> &entityCShape = entity->cShape;

    const Vec2 &topLeftCornerPos = entity->cTransform->topLeftCornerPos;

    const bool collidesWithTop = topLeftCornerPos.y <= 0;
    const bool collidesWithBottom =
        topLeftCornerPos.y + static_cast<float>(entityCShape->rect.h) >= window_size.y;
    const bool collidesWithLeft = topLeftCornerPos.x <= 0;
    const bool collidesWithRight =
        topLeftCornerPos.x + static_cast<float>(entityCShape->rect.w) >= window_size.x;

    std::bitset<4> collidesWithBoundary;
    collidesWithBoundary[TOP]    = collidesWithTop;
    collidesWithBoundary[BOTTOM] = collidesWithBottom;
    collidesWithBoundary[LEFT]   = collidesWithLeft;
    collidesWithBoundary[RIGHT]  = collidesWithRight;

    return collidesWithBoundary;
  }

  Vec2 calculateOverlap(const std::shared_ptr<Entity> &entityA,
                        const std::shared_ptr<Entity> &entityB) {

    if (hasNullComponentPointers(entityA)) {
      SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                   "Entity with ID %zu and tag %u lacks a transform or collision component.",
                   entityA->id(), entityA->tag());
      return {0, 0};
    }

    if (hasNullComponentPointers(entityB)) {
      SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                   "Entity with ID %zu and tag %u lacks a transform or collision component.",
                   entityB->id(), entityB->tag());
      return {0, 0};
    }

    const float halfWidthA  = static_cast<float>(entityA->cShape->rect.w) / 2.0f;
    const float halfHeightA = static_cast<float>(entityA->cShape->rect.h) / 2.0f;

    const float halfWidthB  = static_cast<float>(entityB->cShape->rect.w) / 2.0f;
    const float halfHeightB = static_cast<float>(entityB->cShape->rect.h) / 2.0f;

    const auto halfSizeA = Vec2(halfWidthA, halfHeightA);
    const auto halfSizeB = Vec2(halfWidthB, halfHeightB);

    const Vec2 &centerA = entityA->getCenterPos();
    const Vec2 &centerB = entityB->getCenterPos();

    const auto delta = Vec2(std::abs(centerA.x - centerB.x), std::abs(centerA.y - centerB.y));

    const Vec2 overlap = {
        halfSizeA.x + halfSizeB.x - delta.x,
        halfSizeA.y + halfSizeB.y - delta.y,
    };

    return overlap;
  }
  bool calculateCollisionBetweenEntities(const std::shared_ptr<Entity> &entityA,
                                         const std::shared_ptr<Entity> &entityB) {
    if (hasNullComponentPointers(entityA)) {
      SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                   "Entity with ID %zu and tag %u lacks a transform or collision component.",
                   entityA->id(), entityA->tag());
      return false;
    }

    if (hasNullComponentPointers(entityB)) {
      SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                   "Entity with ID %zu and tag %u lacks a transform or collision component.",
                   entityB->id(), entityB->tag());
      return false;
    }

    const Vec2 overlap = calculateOverlap(entityA, entityB);

    const bool collisionDetected = overlap.x > 0 && overlap.y > 0;

    return collisionDetected;
  }
  std::bitset<4> getPositionRelativeToEntity(const std::shared_ptr<Entity> &entityA,
                                             const std::shared_ptr<Entity> &entityB) {
    if (hasNullComponentPointers(entityA)) {
      SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                   "Entity with ID %zu and tag %u lacks a transform or collision component.",
                   entityA->id(), entityA->tag());
      return std::bitset<4>({});
    }

    if (hasNullComponentPointers(entityB)) {
      SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                   "Entity with ID %zu and tag %u lacks a transform or collision component.",
                   entityB->id(), entityB->tag());
      return std::bitset<4>({});
    }

    const Vec2 &centerA = entityA->getCenterPos();
    const Vec2 &centerB = entityB->getCenterPos();

    std::bitset<4> relativePosition;
    relativePosition[ABOVE]    = centerA.y < centerB.y;
    relativePosition[BELOW]    = centerA.y > centerB.y;
    relativePosition[LEFT_OF]  = centerA.x < centerB.x;
    relativePosition[RIGHT_OF] = centerA.x > centerB.x;

    return relativePosition;
  }

} // namespace CollisionHelpers

namespace CollisionHelpers::MainScene::Enforce {
  void enforcePlayerBounds(const std::shared_ptr<Entity> &entity,
                           const std::bitset<4>          &collides,
                           const Vec2                    &window_size) {
    if (hasNullComponentPointers(entity)) {
      SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                   "Entity with ID %zu and tag %u lacks a transform or collision component.",
                   entity->id(), entity->tag());
      return;
    }
    const std::shared_ptr<CShape> &cShape             = entity->cShape;
    Vec2                          &leftCornerPosition = entity->cTransform->topLeftCornerPos;

    if (collides[TOP]) {
      leftCornerPosition.y = 0;
    }
    if (collides[BOTTOM]) {
      leftCornerPosition.y = window_size.y - static_cast<float>(cShape->rect.h);
    }
    if (collides[LEFT]) {
      leftCornerPosition.x = 0;
    }
    if (collides[RIGHT]) {
      leftCornerPosition.x = window_size.x - static_cast<float>(cShape->rect.w);
    }
  }

  void enforceNonPlayerBounds(const std::shared_ptr<Entity> &entity,
                              const std::bitset<4>          &collides,
                              const Vec2                    &window_size) {

    if (hasNullComponentPointers(entity)) {
      SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                   "Entity with ID %zu and tag %u lacks a transform or collision component.",
                   entity->id(), entity->tag());
    }
    if (entity->tag() == EntityTags::Player) {
      return;
    }
    const std::shared_ptr<CShape> &cShape             = entity->cShape;
    Vec2                          &leftCornerPosition = entity->cTransform->topLeftCornerPos;
    Vec2                          &velocity           = entity->cTransform->velocity;
    // Bounce the entity off the boundaries
    if (collides[TOP]) {
      leftCornerPosition.y = 0;
      velocity.y           = -velocity.y;
    }
    if (collides[BOTTOM]) {
      leftCornerPosition.y = window_size.y - static_cast<float>(cShape->rect.h);
      velocity.y           = -velocity.y;
    }
    if (collides[LEFT]) {
      leftCornerPosition.x = 0;
      velocity.x           = -velocity.x;
    }
    if (collides[RIGHT]) {
      leftCornerPosition.x = window_size.x - static_cast<float>(cShape->rect.w);
      velocity.x           = -velocity.x;
    }
  }

  void enforceCollisionWithWall(const std::shared_ptr<Entity> &entity,
                                const std::shared_ptr<Entity> &wall) {

    if (hasNullComponentPointers(entity)) {
      SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                   "Entity with ID %zu and tag %u lacks a transform or collision component.",
                   entity->id(), entity->tag());
      return;
    }

    if (hasNullComponentPointers(wall)) {
      SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                   "Entity with ID %zu and tag %u lacks a transform or collision component.",
                   wall->id(), wall->tag());
      return;
    }

    const Vec2 &overlap = calculateOverlap(entity, wall);

    const bool           mustResolveCollisionVertically   = overlap.x > overlap.y;
    const bool           mustResolveCollisionHorizontally = overlap.x < overlap.y;
    const std::bitset<4> positionRelativeToWall = getPositionRelativeToEntity(entity, wall);

    const bool playerAboveWall   = positionRelativeToWall[ABOVE];
    const bool playerBelowWall   = positionRelativeToWall[BELOW];
    const bool playerLeftOfWall  = positionRelativeToWall[LEFT_OF];
    const bool playerRightOfWall = positionRelativeToWall[RIGHT_OF];

    if (mustResolveCollisionVertically && playerAboveWall) {
      entity->cTransform->topLeftCornerPos.y -= overlap.y;
      entity->cTransform->velocity.y = -entity->cTransform->velocity.y;
    }

    if (mustResolveCollisionVertically && playerBelowWall) {
      entity->cTransform->topLeftCornerPos.y += overlap.y;
      entity->cTransform->velocity.y = -entity->cTransform->velocity.y;
    }

    if (mustResolveCollisionHorizontally && playerLeftOfWall) {
      entity->cTransform->topLeftCornerPos.x -= overlap.x;
      entity->cTransform->velocity.x = -entity->cTransform->velocity.x;
    }

    if (mustResolveCollisionHorizontally && playerRightOfWall) {
      entity->cTransform->topLeftCornerPos.x += overlap.x;
      entity->cTransform->velocity.x = -entity->cTransform->velocity.x;
    }
  }

  void enforceEntityEntityCollision(const std::shared_ptr<Entity> &entityA,
                                    const std::shared_ptr<Entity> &entityB) {
    if (hasNullComponentPointers(entityA)) {
      SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                   "Entity with ID %zu and tag %u lacks a transform or collision component.",
                   entityA->id(), entityA->tag());

      return;
    }

    if (hasNullComponentPointers(entityB)) {
      SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                   "Entity with ID %zu and tag %u lacks a transform or collision component.",
                   entityB->id(), entityB->tag());

      return;
    }

    const Vec2 &overlap = calculateOverlap(entityA, entityB);

    const bool           mustResolveCollisionVertically   = overlap.x > overlap.y;
    const bool           mustResolveCollisionHorizontally = overlap.x < overlap.y;
    const std::bitset<4> entityARelativePosition =
        getPositionRelativeToEntity(entityA, entityB);
    const std::bitset<4> entityBRelativePosition =
        getPositionRelativeToEntity(entityB, entityA);

    const bool entityAAboveEntityB   = entityARelativePosition[ABOVE];
    const bool entityABelowEntityB   = entityARelativePosition[BELOW];
    const bool entityALeftOfEntityB  = entityARelativePosition[LEFT_OF];
    const bool entityARightOfEntityB = entityARelativePosition[RIGHT_OF];

    const bool entityBAboveEntityA   = entityBRelativePosition[ABOVE];
    const bool entityBBelowEntityA   = entityBRelativePosition[BELOW];
    const bool entityBLeftOfEntityA  = entityBRelativePosition[LEFT_OF];
    const bool entityBRightOfEntityA = entityBRelativePosition[RIGHT_OF];

    if (mustResolveCollisionVertically && entityAAboveEntityB) {
      entityA->cTransform->topLeftCornerPos.y -= overlap.y;
      entityA->cTransform->velocity.y = -entityA->cTransform->velocity.y;
    }

    if (mustResolveCollisionVertically && entityABelowEntityB) {
      entityA->cTransform->topLeftCornerPos.y += overlap.y;
      entityA->cTransform->velocity.y = -entityA->cTransform->velocity.y;
    }

    if (mustResolveCollisionHorizontally && entityALeftOfEntityB) {
      entityA->cTransform->topLeftCornerPos.x -= overlap.x;
      entityA->cTransform->velocity.x = -entityA->cTransform->velocity.x;
    }

    if (mustResolveCollisionHorizontally && entityARightOfEntityB) {
      entityA->cTransform->topLeftCornerPos.x += overlap.x;
      entityA->cTransform->velocity.x = -entityA->cTransform->velocity.x;
    }

    if (mustResolveCollisionVertically && entityBAboveEntityA) {
      entityB->cTransform->topLeftCornerPos.y -= overlap.y;
      entityB->cTransform->velocity.y = -entityB->cTransform->velocity.y;
    }

    if (mustResolveCollisionVertically && entityBBelowEntityA) {
      entityB->cTransform->topLeftCornerPos.y += overlap.y;
      entityB->cTransform->velocity.y = -entityB->cTransform->velocity.y;
    }

    if (mustResolveCollisionHorizontally && entityBLeftOfEntityA) {
      entityB->cTransform->topLeftCornerPos.x -= overlap.x;
      entityB->cTransform->velocity.x = -entityB->cTransform->velocity.x;
    }

    if (mustResolveCollisionHorizontally && entityBRightOfEntityA) {
      entityB->cTransform->topLeftCornerPos.x += overlap.x;
      entityB->cTransform->velocity.x = -entityB->cTransform->velocity.x;
    }
  }

  void enforceBulletCollision(const std::shared_ptr<Entity> &bullet,
                              const bool                     bulletCollides) {
    if (hasNullComponentPointers(bullet)) {
      SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                   "Entity with ID %zu and tag %u lacks a transform or collision component.",
                   bullet->id(), bullet->tag());
      return;
    }

    if (!bulletCollides) {
      return;
    }

    bullet->destroy();
  }

} // namespace CollisionHelpers::MainScene::Enforce

namespace CollisionHelpers::MainScene {
  void handleEntityBounds(const std::shared_ptr<Entity> &entity, const Vec2 &windowSize) {
    const auto tag = entity->tag();
    if (tag == EntityTags::SpeedBoost) {
      const std::bitset<4> speedBoostCollides = detectOutOfBounds(entity, windowSize);
      Enforce::enforceNonPlayerBounds(entity, speedBoostCollides, windowSize);
    }

    if (tag == EntityTags::Player) {
      const std::bitset<4> playerCollides = detectOutOfBounds(entity, windowSize);
      Enforce::enforcePlayerBounds(entity, playerCollides, windowSize);
    }

    if (tag == EntityTags::Enemy) {
      const std::bitset<4> enemyCollides = detectOutOfBounds(entity, windowSize);
      Enforce::enforceNonPlayerBounds(entity, enemyCollides, windowSize);
    }

    if (tag == EntityTags::SlownessDebuff) {
      const std::bitset<4> slownessCollides = detectOutOfBounds(entity, windowSize);
      Enforce::enforceNonPlayerBounds(entity, slownessCollides, windowSize);
    }

    if (tag == EntityTags::Bullet) {
      const std::bitset<4> bulletCollides = detectOutOfBounds(entity, windowSize);
      Enforce::enforceBulletCollision(entity, bulletCollides.any());
    }
    if (tag == EntityTags::Item) {
      const std::bitset<4> itemCollides = detectOutOfBounds(entity, windowSize);
      Enforce::enforceNonPlayerBounds(entity, itemCollides, windowSize);
    }
  }

  void handleEntityEntityCollision(const CollisionPair &collisionPair, const GameState &args) {
    const std::shared_ptr<Entity> &entity      = collisionPair.entityA;
    const std::shared_ptr<Entity> &otherEntity = collisionPair.entityB;

    const EntityTags tag      = entity->tag();
    const EntityTags otherTag = otherEntity->tag();

    constexpr Uint64 minSlownessDuration   = 5000;
    constexpr Uint64 maxSlownessDuration   = 10000;
    constexpr Uint64 minSpeedBoostDuration = 9000;
    constexpr Uint64 maxSpeedBoostDuration = 15000;

    std::uniform_int_distribution<Uint64> randomSlownessDuration(minSlownessDuration,
                                                                 maxSlownessDuration);
    std::uniform_int_distribution<Uint64> randomSpeedBoostDuration(minSpeedBoostDuration,
                                                                   maxSpeedBoostDuration);

    const int                      m_score           = args.score;
    EntityManager                 &m_entities        = args.entityManager;
    std::mt19937                  &m_randomGenerator = args.randomGenerator;
    const std::function<void()>    decrementLives    = args.decrementLives;
    const std::function<void(int)> setScore          = args.setScore;
    const Vec2                    &windowSize        = args.windowSize;

    if (entity == otherEntity) {
      return;
    }

    const bool entitiesCollided =
        CollisionHelpers::calculateCollisionBetweenEntities(entity, otherEntity);

    if (!entitiesCollided) {
      return;
    }

    if (otherTag == EntityTags::Wall) {
      Enforce::enforceCollisionWithWall(entity, otherEntity);
    }

    if (tag == EntityTags::Enemy && otherTag == EntityTags::Enemy) {
      Enforce::enforceEntityEntityCollision(entity, otherEntity);
    }

    if (tag == EntityTags::Enemy && otherTag == EntityTags::SpeedBoost) {
      Enforce::enforceEntityEntityCollision(entity, otherEntity);
    }

    if (tag == EntityTags::Enemy && otherTag == EntityTags::SlownessDebuff) {
      Enforce::enforceEntityEntityCollision(entity, otherEntity);
    }

    if (tag == EntityTags::Bullet && otherTag == EntityTags::Enemy) {
      AudioSample nextSample = AudioSample::BulletHit02;
      args.audioSampleManager.queueSample(nextSample, AudioPriority::STANDARD);

      setScore(m_score + 5);
      otherEntity->destroy();
      entity->destroy();
    }

    if (tag == EntityTags::Bullet && otherTag == EntityTags::Wall) {
      args.audioSampleManager.queueSample(AudioSample::BulletHit01, AudioPriority::BACKGROUND);
    }

    if (tag == EntityTags::Bullet &&
        (otherTag == EntityTags::SlownessDebuff || otherTag == EntityTags::SpeedBoost ||
         otherTag == EntityTags::Item)) {
      otherEntity->destroy();
      entity->destroy();
    }

    if (tag == EntityTags::Player && otherTag == EntityTags::Enemy) {
      args.audioSampleManager.queueSample(AudioSample::EnemyCollides, AudioPriority::STANDARD);
      setScore(m_score > 10 ? m_score - 10 : 0);
      otherEntity->destroy();
      decrementLives();

      const std::shared_ptr<CTransform> &cTransform = entity->cTransform;
      const std::shared_ptr<CEffects>   &cEffects   = entity->cEffects;
      cTransform->topLeftCornerPos                  = {windowSize.x / 2, windowSize.y / 2};

      constexpr float    REMOVAL_RADIUS   = 150.0f;
      const EntityVector entitiesToRemove = EntityHelpers::getEntitiesInRadius(
          entity, m_entities.getEntities(EntityTags::Enemy), REMOVAL_RADIUS);

      for (const std::shared_ptr<Entity> &entityToRemove : entitiesToRemove) {
        entityToRemove->destroy();
      }

      cEffects->clearEffects();
    }

    if (tag == EntityTags::Player && otherTag == EntityTags::SlownessDebuff) {
      const Uint64 startTime = SDL_GetTicks64();
      const Uint64 duration  = randomSlownessDuration(m_randomGenerator);
      entity->cEffects->addEffect(
          {.startTime = startTime, .duration = duration, .type = EffectTypes::Slowness});

      EntityVector        effectsToCheck;
      const EntityVector &slownessDebuffs = m_entities.getEntities(EntityTags::SlownessDebuff);
      const EntityVector &speedBoosts     = m_entities.getEntities(EntityTags::SpeedBoost);

      effectsToCheck.insert(effectsToCheck.end(), slownessDebuffs.begin(),
                            slownessDebuffs.end());
      effectsToCheck.insert(effectsToCheck.end(), speedBoosts.begin(), speedBoosts.end());

      const AudioSample nextSample = AudioSample::SlownessDebuffAcquired;
      args.audioSampleManager.queueSample(nextSample, AudioPriority::STANDARD);

      constexpr float    REMOVAL_RADIUS = 150.0f;
      const EntityVector entitiesToRemove =
          EntityHelpers::getEntitiesInRadius(entity, effectsToCheck, REMOVAL_RADIUS);

      for (const auto &entityToRemove : entitiesToRemove) {
        entityToRemove->destroy();
      }

      for (const auto &speedBoost : speedBoosts) {
        speedBoost->destroy();
      }
    }

    if (tag == EntityTags::Player && otherTag == EntityTags::SpeedBoost) {
      const Uint64 startTime = SDL_GetTicks64();
      const Uint64 duration  = randomSpeedBoostDuration(m_randomGenerator);
      entity->cEffects->addEffect(
          {.startTime = startTime, .duration = duration, .type = EffectTypes::Speed});

      const AudioSample nextSample = AudioSample::SpeedBoostAcquired;
      args.audioSampleManager.queueSample(nextSample, AudioPriority::STANDARD);

      const EntityVector &slownessDebuffs = m_entities.getEntities(EntityTags::SlownessDebuff);
      const EntityVector &speedBoosts     = m_entities.getEntities(EntityTags::SpeedBoost);

      constexpr float    REMOVAL_RADIUS = 150.0f;
      const EntityVector entitiesToRemove =
          EntityHelpers::getEntitiesInRadius(entity, speedBoosts, REMOVAL_RADIUS);

      for (const auto &entityToRemove : entitiesToRemove) {
        entityToRemove->destroy();
      }

      // set the lifespan of the speed boost to 10% of previous value
      for (const auto &speedBoost : speedBoosts) {
        constexpr float MULTIPLIER = 0.1f;
        Uint64         &lifespan   = speedBoost->cLifespan->lifespan;

        lifespan = static_cast<Uint64>(std::round(static_cast<float>(lifespan) * MULTIPLIER));
      }
      for (const auto &slowDebuff : slownessDebuffs) {
        slowDebuff->destroy();
      }
    }

    if (tag == EntityTags::Player && otherTag == EntityTags::Item) {
      args.audioSampleManager.queueSample(AudioSample::ItemAcquired, AudioPriority::STANDARD);
      setScore(m_score + 60);
      otherEntity->destroy();
    }

    if (tag == EntityTags::Item && otherTag == EntityTags::Enemy) {
      Enforce::enforceEntityEntityCollision(entity, otherEntity);
    }

    if (tag == EntityTags::Item && otherTag == EntityTags::SpeedBoost) {
      Enforce::enforceEntityEntityCollision(entity, otherEntity);
    }

    if (tag == EntityTags::Item && otherTag == EntityTags::SlownessDebuff) {
      Enforce::enforceEntityEntityCollision(entity, otherEntity);
    }
  }

} // namespace CollisionHelpers::MainScene