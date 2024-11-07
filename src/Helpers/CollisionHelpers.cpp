
#include "../../includes/Helpers/CollisionHelpers.hpp"
#include "../../includes/EntityManagement/Entity.hpp"
#include "../../includes/GameScenes/MainScene.hpp"
#include "../../includes/Helpers/EntityHelpers.hpp"

#include <bitset>
#include <iostream>

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

    const bool collidesWithTop    = topLeftCornerPos.y <= 0;
    const bool collidesWithBottom = topLeftCornerPos.y + entityCShape->rect.h >= window_size.y;
    const bool collidesWithLeft   = topLeftCornerPos.x <= 0;
    const bool collidesWithRight  = topLeftCornerPos.x + entityCShape->rect.w >= window_size.x;

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
    const Vec2 &playerPos     = entityA->cTransform->topLeftCornerPos;
    const Vec2 &wallPos       = entityB->cTransform->topLeftCornerPos;
    const Vec2 halfSizePlayer = Vec2(entityA->cShape->rect.w / 2, entityA->cShape->rect.h / 2);
    const Vec2 halfSizeWall   = Vec2(entityB->cShape->rect.w / 2, entityB->cShape->rect.h / 2);

    const Vec2 &playerCenter =
        playerPos + Vec2(entityA->cShape->rect.w / 2, entityA->cShape->rect.h / 2);
    const Vec2 &wallCenter =
        wallPos + Vec2(entityB->cShape->rect.w / 2, entityB->cShape->rect.h / 2);

    const Vec2 delta =
        Vec2(std::abs(playerCenter.x - wallCenter.x), std::abs(playerCenter.y - wallCenter.y));

    const Vec2 overlap = {
        halfSizePlayer.x + halfSizeWall.x - delta.x,
        halfSizePlayer.y + halfSizeWall.y - delta.y,
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
    const Vec2 &entityAPos = entityA->cTransform->topLeftCornerPos;
    const Vec2 &entityBPos = entityB->cTransform->topLeftCornerPos;

    const Vec2 &entityACenter =
        entityAPos + Vec2(entityA->cShape->rect.w / 2, entityA->cShape->rect.h / 2);
    const Vec2 &entityBCenter =
        entityBPos + Vec2(entityB->cShape->rect.w / 2, entityB->cShape->rect.h / 2);

    std::bitset<4> relativePosition;
    relativePosition[ABOVE]    = entityACenter.y < entityBCenter.y;
    relativePosition[BELOW]    = entityACenter.y > entityBCenter.y;
    relativePosition[LEFT_OF]  = entityACenter.x < entityBCenter.x;
    relativePosition[RIGHT_OF] = entityACenter.x > entityBCenter.x;

    return relativePosition;
  }

}; // namespace CollisionHelpers

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
      leftCornerPosition.y = window_size.y - cShape->rect.h;
    }
    if (collides[LEFT]) {
      leftCornerPosition.x = 0;
    }
    if (collides[RIGHT]) {
      leftCornerPosition.x = window_size.x - cShape->rect.w;
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
      leftCornerPosition.y = window_size.y - cShape->rect.h;
      velocity.y           = -velocity.y;
    }
    if (collides[LEFT]) {
      leftCornerPosition.x = 0;
      velocity.x           = -velocity.x;
    }
    if (collides[RIGHT]) {
      leftCornerPosition.x = window_size.x - cShape->rect.w;
      velocity.x           = -velocity.x;
    }
  };

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

}; // namespace CollisionHelpers::MainScene::Enforce

namespace CollisionHelpers::MainScene {
  void handleEntityBounds(const std::shared_ptr<Entity> &entity, const Vec2 &windowSize) {
    const auto tag = entity->tag();
    if (tag == EntityTags::SpeedBoost) {
      std::bitset<4> speedBoostCollides = detectOutOfBounds(entity, windowSize);
      Enforce::enforceNonPlayerBounds(entity, speedBoostCollides, windowSize);
    }

    if (tag == EntityTags::Player) {
      std::bitset<4> playerCollides = detectOutOfBounds(entity, windowSize);
      Enforce::enforcePlayerBounds(entity, playerCollides, windowSize);
    }

    if (tag == EntityTags::Enemy) {
      std::bitset<4> enemyCollides = detectOutOfBounds(entity, windowSize);
      Enforce::enforceNonPlayerBounds(entity, enemyCollides, windowSize);
    }

    if (tag == EntityTags::SlownessDebuff) {
      std::bitset<4> slownessCollides = detectOutOfBounds(entity, windowSize);
      Enforce::enforceNonPlayerBounds(entity, slownessCollides, windowSize);
    }

    if (tag == EntityTags::Bullet) {
      std::bitset<4> bulletCollides = detectOutOfBounds(entity, windowSize);
      Enforce::enforceBulletCollision(entity, bulletCollides.any());
    }
    if (tag == EntityTags::Item) {
      std::bitset<4> itemCollides = detectOutOfBounds(entity, windowSize);
      Enforce::enforceNonPlayerBounds(entity, itemCollides, windowSize);
    }
  };

  void handleEntityEntityCollision(const CollisionPair &collisionPair, const GameState &args) {
    const std::shared_ptr<Entity> &entity      = collisionPair.entityA;
    const std::shared_ptr<Entity> &otherEntity = collisionPair.entityB;

    const EntityTags tag      = entity->tag();
    const EntityTags otherTag = otherEntity->tag();

    const Uint64 minSlownessDuration   = 5000;
    const Uint64 maxSlownessDuration   = 10000;
    const Uint64 minSpeedBoostDuration = 9000;
    const Uint64 maxSpeedBoostDuration = 15000;

    std::uniform_int_distribution<Uint64> randomSlownessDuration(minSlownessDuration,
                                                                 maxSlownessDuration);
    std::uniform_int_distribution<Uint64> randomSpeedBoostDuration(minSpeedBoostDuration,
                                                                   maxSpeedBoostDuration);

    const int                m_score           = args.score;
    std::function<void(int)> setScore          = args.setScore;
    EntityManager           &m_entities        = args.entityManager;
    std::mt19937            &m_randomGenerator = args.randomGenerator;
    std::function<void()>    decrementLives    = args.decrementLives;
    const Vec2              &windowSize        = args.windowSize;

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
      setScore(m_score + 5);
      otherEntity->destroy();
      entity->destroy();
    }

    if (tag == EntityTags::Player && otherTag == EntityTags::Enemy) {
      setScore(m_score > 10 ? m_score - 10 : 0);
      otherEntity->destroy();
      decrementLives();

      const std::shared_ptr<CTransform> &cTransform = entity->cTransform;
      cTransform->topLeftCornerPos                  = {windowSize.x / 2, windowSize.y / 2};

      const float        REMOVAL_RADIUS   = 150.0f;
      const EntityVector entitiesToRemove = EntityHelpers::getEntitiesInRadius(
          entity, m_entities.getEntities(EntityTags::Enemy), REMOVAL_RADIUS);

      for (const std::shared_ptr<Entity> &entityToRemove : entitiesToRemove) {
        entityToRemove->destroy();
      }
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

      const float        REMOVAL_RADIUS = 150.0f;
      const EntityVector entitiesToRemove =
          EntityHelpers::getEntitiesInRadius(entity, effectsToCheck, REMOVAL_RADIUS);

      for (const auto &entityToRemove : entitiesToRemove) {
        entityToRemove->destroy();
      }
    }

    if (tag == EntityTags::Player && otherTag == EntityTags::SpeedBoost) {
      const Uint64 startTime = SDL_GetTicks64();
      const Uint64 duration  = randomSpeedBoostDuration(m_randomGenerator);
      entity->cEffects->addEffect(
          {.startTime = startTime, .duration = duration, .type = EffectTypes::Speed});

      const EntityVector &slownessDebuffs = m_entities.getEntities(EntityTags::SlownessDebuff);
      const EntityVector &speedBoosts     = m_entities.getEntities(EntityTags::SpeedBoost);

      const float        REMOVAL_RADIUS = 150.0f;
      const EntityVector entitiesToRemove =
          EntityHelpers::getEntitiesInRadius(entity, speedBoosts, REMOVAL_RADIUS);

      for (const auto &entityToRemove : entitiesToRemove) {
        entityToRemove->destroy();
      }

      // set the lifespan of the speed boost to 10% of previous value
      const float MULTIPLIER = 0.1f;
      for (const auto &speedBoost : speedBoosts) {
        speedBoost->cLifespan->lifespan *= MULTIPLIER;
      }
      for (const auto &slowDebuff : slownessDebuffs) {
        slowDebuff->destroy();
      }
    }

    if (tag == EntityTags::Player && otherTag == EntityTags::Item) {
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
  };

} // namespace CollisionHelpers::MainScene