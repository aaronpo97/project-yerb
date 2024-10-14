#include "../includes/Entity.h"
#include <../includes/EntityTags.h>
#include <../includes/Game.h>
#include <memory>

namespace MovementHelpers {
  void moveEnemies(std::shared_ptr<Entity> &entity, const float &deltaTime) {
    if (entity == nullptr) {
      throw std::runtime_error("Entity is null");
    }

    const auto &entityTag = entity->tag();
    if (entityTag != EntityTags::Enemy) {
      return;
    }

    const auto &entityCTransform = entity->cTransform;
    if (entityCTransform == nullptr) {
      throw std::runtime_error("Entity " + entity->tag() + ", with ID " +
                               std::to_string(entity->id()) + " lacks a transform component.");
    }

    auto &position = entityCTransform->topLeftCornerPos;
    auto &velocity = entityCTransform->velocity;

    while (velocity == Vec2{0, 0}) {
      velocity.x = static_cast<float>(rand() % 3 - 1);
      velocity.y = static_cast<float>(rand() % 3 - 1);
    }

    position += velocity * deltaTime * 50;
  }
  void moveSpeedBoosts(std::shared_ptr<Entity> &entity, const float &deltaTime) {
    if (entity == nullptr) {
      throw std::runtime_error("Entity is null");
    }

    const auto &entityTag = entity->tag();
    if (entityTag != EntityTags::SpeedBoost) {
      return;
    }

    const auto &entityCTransform = entity->cTransform;
    if (entityCTransform == nullptr) {
      throw std::runtime_error("Entity " + entity->tag() + ", with ID " +
                               std::to_string(entity->id()) + " lacks a transform component.");
    }

    Vec2 &position = entityCTransform->topLeftCornerPos;
    Vec2 &velocity = entityCTransform->velocity;

    while (velocity == Vec2{0, 0}) {
      // Randomize velocity x and velocity y with any value between -1 and 1, if it's still {0,
      // 0}, then keep randomizing

      velocity.x = static_cast<float>(rand() % 3 - 1);
      velocity.y = static_cast<float>(rand() % 3 - 1);
    }

    position += velocity * deltaTime * 50;
  }

  void movePlayer(std::shared_ptr<Entity> &entity,
                  const PlayerConfig      &m_playerConfig,
                  const float             &deltaTime) {
    if (entity == nullptr) {
      throw std::runtime_error("Entity is null");
    }

    const auto &entityTag = entity->tag();
    if (entityTag != EntityTags::Player) {
      return;
    }

    const auto &entityCTransform = entity->cTransform;
    if (entityCTransform == nullptr) {
      throw std::runtime_error("Entity " + entity->tag() + ", with ID " +
                               std::to_string(entity->id()) + " lacks a transform component.");
    }

    const std::shared_ptr<CInput> &entityCInput = entity->cInput;
    if (entityCInput == nullptr) {
      throw std::runtime_error("Player entity lacks an input component.");
    }

    Vec2 &position = entityCTransform->topLeftCornerPos;
    Vec2 &velocity = entityCTransform->velocity;
    // reset the velocity
    velocity = {0, 0};

    if (entityCInput->forward) {
      velocity.y = -1;
    }
    if (entityCInput->backward) {
      velocity.y = 1;
    }
    if (entityCInput->left) {
      velocity.x = -1;
    }
    if (entityCInput->right) {
      velocity.x = 1;
    }

    position += velocity * m_playerConfig.speed * deltaTime * 50;
  }
} // namespace MovementHelpers