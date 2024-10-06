
#include "../includes/CollisionHelpers.h"
#include "../includes/MathHelpers.h"
#include <bitset>

enum Boundaries { TOP, BOTTOM, LEFT, RIGHT };

bool hasNullComponentPointers(const std::shared_ptr<Entity> &entity) {
  const bool cTransformIsNullPtr = !entity->cTransform;
  const bool cCollisionIsNullPtr = !entity->cCollision;
  return cTransformIsNullPtr || cCollisionIsNullPtr;
}

namespace CollisionHelpers {

  std::bitset<4> detectOutOfBounds(const std::shared_ptr<Entity> &entity,
                                   const Vec2                    &window_size) {
    if (hasNullComponentPointers(entity)) {
      throw std::runtime_error("Entity " + entity->tag() + ", with ID " +
                               std::to_string(entity->id()) +
                               " lacks a transform or collision component.");
    }

    const float                    radius          = entity->cCollision->radius;
    const std::shared_ptr<CShape> &entityCShape    = entity->cShape;
    Vec2                          &entityCenterPos = entity->cTransform->centerPos;

    entityCenterPos.x = entity->cTransform->topLeftCornerPos.x + entityCShape->rect.w / 2;
    entityCenterPos.y = entity->cTransform->topLeftCornerPos.y + entityCShape->rect.h / 2;

    const bool collidesWithTop    = entityCenterPos.y - radius < 0;
    const bool collidesWithBottom = entityCenterPos.y + radius > window_size.y;
    const bool collidesWithLeft   = entityCenterPos.x - radius < 0;
    const bool collidesWithRight  = entityCenterPos.x + radius > window_size.x;

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
      throw std::runtime_error("Entity " + entity->tag() + ", with ID " +
                               std::to_string(entity->id()) +
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

    Vec2 &centerPos = entity->cTransform->centerPos;
    centerPos.x     = leftCornerPosition.x + cShape->rect.w / 2;
    centerPos.y     = leftCornerPosition.y + cShape->rect.h / 2;
  }

  bool calculateCollisionBetweenEntities(const std::shared_ptr<Entity> &entityA,
                                         const std::shared_ptr<Entity> &entityB) {
    if (hasNullComponentPointers(entityA)) {
      throw std::runtime_error("Entity " + entityA->tag() + ", with ID " +
                               std::to_string(entityA->id()) +
                               " lacks a transform or collision component.");
    }
    if (hasNullComponentPointers(entityB)) {
      throw std::runtime_error("Entity " + entityB->tag() + ", with ID " +
                               std::to_string(entityB->id()) +
                               " lacks a transform or collision component.");
    }

    const Vec2 &centerPosA = entityA->cTransform->centerPos;
    const Vec2 &centerPosB = entityB->cTransform->centerPos;

    const float distance =
        MathHelpers::pythagoras(centerPosA.x - centerPosB.x, centerPosA.y - centerPosB.y);

    const float radiusA = entityA->cCollision->radius;
    const float radiusB = entityB->cCollision->radius;

    return distance < radiusA + radiusB;
  }

} // namespace CollisionHelpers