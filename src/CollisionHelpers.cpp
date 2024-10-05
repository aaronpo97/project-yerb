
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

    const Vec2                    &entityPosition = entity->cTransform->position;
    const std::shared_ptr<CShape> &entityCShape   = entity->cShape;

    const bool collidesWithTop    = entityPosition.y < 0;
    const bool collidesWithLeft   = entityPosition.x < 0;
    const bool collidesWithBottom = entityPosition.y > window_size.y - entityCShape->rect.h;
    const bool collidesWithRight  = entityPosition.x > window_size.x - entityCShape->rect.w;

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
    const std::shared_ptr<CShape> &cShape   = entity->cShape;
    Vec2                          &position = entity->cTransform->position;

    if (collides[TOP]) {
      position.y = 0;
    }
    if (collides[BOTTOM]) {
      position.y = window_size.y - cShape->rect.h;
    }
    if (collides[LEFT]) {
      position.x = 0;
    }
    if (collides[RIGHT]) {
      position.x = window_size.x - cShape->rect.w;
    }
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

    return true;
  }

} // namespace CollisionHelpers