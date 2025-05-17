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
