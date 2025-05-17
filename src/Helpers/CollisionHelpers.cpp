#include "../../includes/Helpers/CollisionHelpers.hpp"
#include "../../includes/EntityManagement/Entity.hpp"
#include "../../includes/GameScenes/MainScene/MainScene.hpp"
#include "../../includes/Helpers/EntityHelpers.hpp"

#include <bitset>

enum Boundaries : Uint8 { TOP, BOTTOM, LEFT, RIGHT };
enum RelativePosition : Uint8 { ABOVE, BELOW, LEFT_OF, RIGHT_OF };

namespace CollisionHelpers {
  std::bitset<4> detectOutOfBounds(const std::shared_ptr<Entity> &entity,
                                   const Vec2                    &window_size) {

    const std::shared_ptr<CTransform> &cTransform = entity->getComponent<CTransform>();
    const std::shared_ptr<CShape>     &cShape     = entity->getComponent<CShape>();

    if (!cTransform || !cShape) {
      SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                   "Entity with ID %zu and tag %u lacks a transform or shape component.",
                   entity->id(),
                   entity->tag());

      return {};
    }

    const Vec2 &topLeftCorner = cTransform->topLeftCornerPos;
    const auto  rectHeight    = static_cast<float>(cShape->rect.h);
    const auto  rectWidth     = static_cast<float>(cShape->rect.w);

    const bool collidesWithTop    = topLeftCorner.y <= 0;
    const bool collidesWithBottom = topLeftCorner.y + rectHeight >= window_size.y;
    const bool collidesWithLeft   = topLeftCorner.x <= 0;
    const bool collidesWithRight  = topLeftCorner.x + rectWidth >= window_size.x;

    std::bitset<4> collidesWithBoundary;
    collidesWithBoundary[TOP]    = collidesWithTop;
    collidesWithBoundary[BOTTOM] = collidesWithBottom;
    collidesWithBoundary[LEFT]   = collidesWithLeft;
    collidesWithBoundary[RIGHT]  = collidesWithRight;

    return collidesWithBoundary;
  }

  Vec2 calculateOverlap(const std::shared_ptr<Entity> &entityA,
                        const std::shared_ptr<Entity> &entityB) {

    const auto &cShapeA = entityA->getComponent<CShape>();
    const auto &cShapeB = entityB->getComponent<CShape>();

    if (!cShapeA) {
      SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                   "Entity with ID %zu and tag %u lacks a collision component.",
                   entityA->id(),
                   entityA->tag());
      return {0, 0};
    }

    if (!cShapeB) {
      SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                   "Entity with ID %zu and tag %u lacks a collision component.",
                   entityB->id(),
                   entityB->tag());
      return {0, 0};
    }

    const float halfWidthA  = static_cast<float>(cShapeA->rect.w) / 2.0f;
    const float halfHeightA = static_cast<float>(cShapeA->rect.h) / 2.0f;

    const float halfWidthB  = static_cast<float>(cShapeB->rect.w) / 2.0f;
    const float halfHeightB = static_cast<float>(cShapeB->rect.h) / 2.0f;

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
    const Vec2 overlap           = calculateOverlap(entityA, entityB);
    const bool collisionDetected = overlap.x > 0 && overlap.y > 0;
    return collisionDetected;
  }

  std::bitset<4> getPositionRelativeToEntity(const std::shared_ptr<Entity> &entityA,
                                             const std::shared_ptr<Entity> &entityB) {
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

    const std::shared_ptr<CShape>     &cShape     = entity->getComponent<CShape>();
    const std::shared_ptr<CTransform> &cTransform = entity->getComponent<CTransform>();

    if (!cShape || !cTransform) {
      SDL_LogError(SDL_LOG_CATEGORY_SYSTEM,
                   "Entity with ID %zu and tag %u lacks a transform or shape component.",
                   entity->id(),
                   entity->tag());
    };

    Vec2 &leftCornerPosition = cTransform->topLeftCornerPos;

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
                              const std::bitset<4>          &collides) {
    if (entity->tag() == EntityTags::Player) {
      return;
    }

    if (collides.any()) {
      entity->destroy();
    }
  }

  void enforceCollisionWithWall(const std::shared_ptr<Entity> &entity,
                                const std::shared_ptr<Entity> &wall) {

    const auto &cTransform     = entity->getComponent<CTransform>();
    const auto &cBounceTracker = entity->getComponent<CBounceTracker>();

    const Vec2 &overlap = calculateOverlap(entity, wall);

    const bool           mustResolveCollisionVertically   = overlap.x > overlap.y;
    const bool           mustResolveCollisionHorizontally = overlap.x < overlap.y;
    const std::bitset<4> positionRelativeToWall = getPositionRelativeToEntity(entity, wall);

    const bool playerAboveWall   = positionRelativeToWall[ABOVE];
    const bool playerBelowWall   = positionRelativeToWall[BELOW];
    const bool playerLeftOfWall  = positionRelativeToWall[LEFT_OF];
    const bool playerRightOfWall = positionRelativeToWall[RIGHT_OF];

    if (mustResolveCollisionVertically && playerAboveWall) {
      cTransform->topLeftCornerPos.y -= overlap.y;
      cTransform->velocity.y = -cTransform->velocity.y;
    }

    if (mustResolveCollisionVertically && playerBelowWall) {
      cTransform->topLeftCornerPos.y += overlap.y;
      cTransform->velocity.y = -cTransform->velocity.y;
    }

    if (mustResolveCollisionHorizontally && playerLeftOfWall) {
      cTransform->topLeftCornerPos.x -= overlap.x;
      cTransform->velocity.x = -cTransform->velocity.x;
    }

    if (mustResolveCollisionHorizontally && playerRightOfWall) {
      cTransform->topLeftCornerPos.x += overlap.x;
      cTransform->velocity.x = -cTransform->velocity.x;
    }

    if (cBounceTracker) {
      cBounceTracker->addBounce();
    }
  }

  void enforceEntityEntityCollision(const std::shared_ptr<Entity> &entityA,
                                    const std::shared_ptr<Entity> &entityB) {
    const auto &cTransformA = entityA->getComponent<CTransform>();
    const auto &cTransformB = entityB->getComponent<CTransform>();

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
      cTransformA->topLeftCornerPos.y -= overlap.y;
      cTransformA->velocity.y = -cTransformA->velocity.y;
    }

    if (mustResolveCollisionVertically && entityABelowEntityB) {
      cTransformA->topLeftCornerPos.y += overlap.y;
      cTransformA->velocity.y = -cTransformA->velocity.y;
    }

    if (mustResolveCollisionHorizontally && entityALeftOfEntityB) {
      cTransformA->topLeftCornerPos.x -= overlap.x;
      cTransformA->velocity.x = -cTransformA->velocity.x;
    }

    if (mustResolveCollisionHorizontally && entityARightOfEntityB) {
      cTransformA->topLeftCornerPos.x += overlap.x;
      cTransformA->velocity.x = -cTransformA->velocity.x;
    }

    if (mustResolveCollisionVertically && entityBAboveEntityA) {
      cTransformB->topLeftCornerPos.y -= overlap.y;
      cTransformB->velocity.y = -cTransformB->velocity.y;
    }

    if (mustResolveCollisionVertically && entityBBelowEntityA) {
      cTransformB->topLeftCornerPos.y += overlap.y;
      cTransformB->velocity.y = -cTransformB->velocity.y;
    }

    if (mustResolveCollisionHorizontally && entityBLeftOfEntityA) {
      cTransformB->topLeftCornerPos.x -= overlap.x;
      cTransformB->velocity.x = -cTransformB->velocity.x;
    }

    if (mustResolveCollisionHorizontally && entityBRightOfEntityA) {
      cTransformB->topLeftCornerPos.x += overlap.x;
      cTransformB->velocity.x = -cTransformB->velocity.x;
    }
  }

} // namespace CollisionHelpers::MainScene::Enforce

namespace CollisionHelpers::MainScene {
  void handleEntityBounds(const std::shared_ptr<Entity> &entity, const Vec2 &windowSize) {
    const auto tag = entity->tag();
    if (tag == EntityTags::SpeedBoost) {
      const std::bitset<4> speedBoostCollides = detectOutOfBounds(entity, windowSize);
      Enforce::enforceNonPlayerBounds(entity, speedBoostCollides);
    }

    if (tag == EntityTags::Player) {
      const std::bitset<4> playerCollides = detectOutOfBounds(entity, windowSize);
      Enforce::enforcePlayerBounds(entity, playerCollides, windowSize);
    }

    if (tag == EntityTags::Enemy) {
      const std::bitset<4> enemyCollides = detectOutOfBounds(entity, windowSize);
      Enforce::enforceNonPlayerBounds(entity, enemyCollides);
    }

    if (tag == EntityTags::SlownessDebuff) {
      const std::bitset<4> slownessCollides = detectOutOfBounds(entity, windowSize);
      Enforce::enforceNonPlayerBounds(entity, slownessCollides);
    }

    if (tag == EntityTags::Bullet) {
      const std::bitset<4> bulletCollides = detectOutOfBounds(entity, windowSize);
      Enforce::enforceNonPlayerBounds(entity, bulletCollides);
    }
    if (tag == EntityTags::Item) {
      const std::bitset<4> itemCollides = detectOutOfBounds(entity, windowSize);
      Enforce::enforceNonPlayerBounds(entity, itemCollides);
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
      AudioSample nextSample = AudioSample::BULLET_HIT_02;
      args.audioSampleManager.queueSample(nextSample, AudioSamplePriority::STANDARD);

      const auto &cBounceTracker = entity->getComponent<CBounceTracker>();

      if (!cBounceTracker) {
        entity->destroy();
        return;
      }
      const int bounces = cBounceTracker->getBounces();
      setScore(5 * (bounces + 1) + m_score);
      otherEntity->destroy();
      entity->destroy();
    }

    if (tag == EntityTags::Bullet && otherTag == EntityTags::Wall) {
      args.audioSampleManager.queueSample(AudioSample::BULLET_HIT_01,
                                          AudioSamplePriority::BACKGROUND);
    }

    if (tag == EntityTags::Bullet &&
        (otherTag == EntityTags::SlownessDebuff || otherTag == EntityTags::SpeedBoost ||
         otherTag == EntityTags::Item)) {
      otherEntity->destroy();
      entity->destroy();

      if (m_score > 15) {
        const auto updatedScore =
            otherTag == EntityTags::SlownessDebuff ? m_score + 15 : m_score - 15;
        setScore(updatedScore);
      }
    }

    if (tag == EntityTags::Player && otherTag == EntityTags::Enemy) {
      args.audioSampleManager.queueSample(AudioSample::ENEMY_COLLISION,
                                          AudioSamplePriority::STANDARD);
      setScore(m_score > 10 ? m_score - 10 : 0);
      otherEntity->destroy();
      decrementLives();

      const std::shared_ptr<CTransform> &cTransform = entity->getComponent<CTransform>();
      const std::shared_ptr<CEffects>   &cEffects   = entity->getComponent<CEffects>();
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

      const auto &cEffects = entity->getComponent<CEffects>();
      cEffects->addEffect(
          {.startTime = startTime, .duration = duration, .type = EffectTypes::Slowness});

      EntityVector        effectsToCheck;
      const EntityVector &slownessDebuffs = m_entities.getEntities(EntityTags::SlownessDebuff);
      const EntityVector &speedBoosts     = m_entities.getEntities(EntityTags::SpeedBoost);

      effectsToCheck.insert(
          effectsToCheck.end(), slownessDebuffs.begin(), slownessDebuffs.end());
      effectsToCheck.insert(effectsToCheck.end(), speedBoosts.begin(), speedBoosts.end());

      const AudioSample nextSample = AudioSample::SLOWNESS_DEBUFF;
      args.audioSampleManager.queueSample(nextSample, AudioSamplePriority::STANDARD);

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
      const auto  &cEffects  = entity->getComponent<CEffects>();

      cEffects->addEffect(
          {.startTime = startTime, .duration = duration, .type = EffectTypes::Speed});

      const AudioSample nextSample = AudioSample::SPEED_BOOST;
      args.audioSampleManager.queueSample(nextSample, AudioSamplePriority::STANDARD);

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
        const auto     &cLifespan  = speedBoost->getComponent<CLifespan>();
        Uint64         &lifespan   = cLifespan->lifespan;

        lifespan = static_cast<Uint64>(std::round(static_cast<float>(lifespan) * MULTIPLIER));
      }
      for (const auto &slowDebuff : slownessDebuffs) {
        slowDebuff->destroy();
      }
    }

    if (tag == EntityTags::Player && otherTag == EntityTags::Item) {
      args.audioSampleManager.queueSample(AudioSample::ITEM_ACQUIRED,
                                          AudioSamplePriority::STANDARD);
      setScore(m_score + 90);
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