
#include "../../includes/Helpers/CollisionHelpers.hpp"
#include "../../includes/EntityManagement/Entity.hpp"

#include <bitset>
#include <iostream>

enum Boundaries { TOP, BOTTOM, LEFT, RIGHT };
enum RelativePosition { ABOVE, BELOW, LEFT_OF, RIGHT_OF };

bool hasNullComponentPointers(const std::shared_ptr<Entity> &entity) {
  const bool cTransformIsNullPtr = !entity->cTransform;
  return cTransformIsNullPtr;
}

namespace CollisionHelpers {
  std::bitset<4> detectOutOfBounds(const std::shared_ptr<Entity> &entity,
                                   const Vec2                    &window_size) {
    if (hasNullComponentPointers(entity)) {
      throw std::runtime_error("Entity with ID " + std::to_string(entity->id()) +
                               " lacks a transform or collision component.");
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

  void enforcePlayerBounds(const std::shared_ptr<Entity> &entity,
                           const std::bitset<4>          &collides,
                           const Vec2                    &window_size) {
    if (hasNullComponentPointers(entity)) {
      throw std::runtime_error("Entity with ID " + std::to_string(entity->id()) +
                               " lacks a transform or collision component.");
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
      throw std::runtime_error("Entity with ID " + std::to_string(entity->id()) +
                               " lacks a transform or collision component.");
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

  Vec2 calculateOverlap(const std::shared_ptr<Entity> &entityA,
                        const std::shared_ptr<Entity> &entityB) {

    if (hasNullComponentPointers(entityA)) {
      throw std::runtime_error("Entity with ID " + std::to_string(entityA->id()) +
                               " lacks a transform or collision component.");
    }

    if (hasNullComponentPointers(entityB)) {
      throw std::runtime_error("Entity with ID " + std::to_string(entityB->id()) +
                               " lacks a transform or collision component.");
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
      throw std::runtime_error("Entity with ID " + std::to_string(entityA->id()) +
                               " lacks a transform or collision component.");
    }
    if (hasNullComponentPointers(entityB)) {
      throw std::runtime_error("Entity with ID " + std::to_string(entityB->id()) +
                               " lacks a transform or collision component.");
    }

    const Vec2 overlap = calculateOverlap(entityA, entityB);

    const bool collisionDetected = overlap.x > 0 && overlap.y > 0;

    return collisionDetected;
  }
  std::bitset<4> getPositionRelativeToEntity(const std::shared_ptr<Entity> &entityA,
                                             const std::shared_ptr<Entity> &entityB) {
    if (hasNullComponentPointers(entityA)) {
      throw std::runtime_error("Entity with ID " + std::to_string(entityA->id()) +
                               " lacks a transform or collision component.");
    }

    if (hasNullComponentPointers(entityB)) {
      throw std::runtime_error("Entity with ID " + std::to_string(entityB->id()) +
                               " lacks a transform or collision component.");
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

  void enforceCollisionWithWall(const std::shared_ptr<Entity> &entity,
                                const std::shared_ptr<Entity> &wall) {

    if (hasNullComponentPointers(entity)) {
      throw std::runtime_error("Entity with ID " + std::to_string(entity->id()) +
                               " lacks a transform or collision component.");
    }

    if (hasNullComponentPointers(wall)) {
      throw std::runtime_error("Entity with ID " + std::to_string(wall->id()) +
                               " lacks a transform or collision component.");
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
      throw std::runtime_error("Entity with ID " + std::to_string(entityA->id()) +
                               " lacks a transform or collision component.");
    }

    if (hasNullComponentPointers(entityB)) {
      throw std::runtime_error("Entity with ID " + std::to_string(entityB->id()) +
                               " lacks a transform or collision component.");
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
      throw std::runtime_error("Entity with ID " + std::to_string(bullet->id()) +
                               " lacks a transform or collision component.");
    }

    if (!bulletCollides) {
      return;
    }

    bullet->destroy();
  }
}; // namespace CollisionHelpers
