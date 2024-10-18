#include "../includes/MovementHelpers.h"
#include "../includes/Entity.h"
#include "../includes/EntityTags.h"
#include "../includes/Game.h"
#include <memory>
const float BASE_MOVEMENT_MULTIPLIER = 50.0f;

namespace MovementHelpers {

  void moveEnemies(std::shared_ptr<Entity> &entity,
                   const EnemyConfig       &enemyConfig,
                   const float             &deltaTime) {

    std::uniform_int_distribution<int> randomVelocityX(-1, 1);

    if (entity == nullptr) {
      throw std::runtime_error("Entity is null");
    }

    const std::string &entityTag = entity->tag();
    if (entityTag != EntityTags::Enemy) {
      return;
    }

    const std::shared_ptr<CTransform> &entityCTransform = entity->cTransform;
    if (entityCTransform == nullptr) {
      throw std::runtime_error("Entity " + entity->tag() + ", with ID " +
                               std::to_string(entity->id()) + " lacks a transform component.");
    }

    Vec2 &position = entityCTransform->topLeftCornerPos;
    Vec2 &velocity = entityCTransform->velocity;

    position += velocity * ((enemyConfig.speed) * (deltaTime * BASE_MOVEMENT_MULTIPLIER));
  }
  void moveSpeedBoosts(std::shared_ptr<Entity>      &entity,
                       const SpeedBoostEffectConfig &speedBoostEffectConfig,
                       const float                  &deltaTime) {
    if (entity == nullptr) {
      throw std::runtime_error("Entity is null");
    }

    const std::string &entityTag = entity->tag();
    if (entityTag != EntityTags::SpeedBoost) {
      return;
    }

    const std::shared_ptr<CTransform> &entityCTransform = entity->cTransform;
    if (entityCTransform == nullptr) {
      throw std::runtime_error("Entity " + entity->tag() + ", with ID " +
                               std::to_string(entity->id()) + " lacks a transform component.");
    }

    Vec2 &position = entityCTransform->topLeftCornerPos;
    Vec2 &velocity = entityCTransform->velocity;

    position += velocity * deltaTime * speedBoostEffectConfig.speed * BASE_MOVEMENT_MULTIPLIER;
  }

  void movePlayer(std::shared_ptr<Entity> &entity,
                  const PlayerConfig      &playerConfig,
                  const float             &deltaTime) {
    if (entity == nullptr) {
      throw std::runtime_error("Entity is null");
    }

    const std::string &entityTag = entity->tag();
    if (entityTag != EntityTags::Player) {
      return;
    }

    const std::shared_ptr<CTransform> &entityCTransform = entity->cTransform;
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

    float effectMultiplier = 1;

    if (entity->cEffects->hasEffect(EffectTypes::Speed)) {
      effectMultiplier = playerConfig.speedBoostMultiplier;
    }

    if (entity->cEffects->hasEffect(EffectTypes::Slowness)) {
      effectMultiplier = playerConfig.slownessMultiplier;
    }

    position += velocity * ((playerConfig.baseSpeed * effectMultiplier) *
                            (deltaTime * BASE_MOVEMENT_MULTIPLIER));
  }

  void moveSlownessDebuffs(std::shared_ptr<Entity>    &entity,
                           const SlownessEffectConfig &slownessEffectConfig,
                           const float                &deltaTime) {

    if (entity == nullptr) {
      throw std::runtime_error("Entity is null");
    }

    const std::string &entityTag = entity->tag();
    if (entityTag != EntityTags::SlownessDebuff) {
      return;
    }

    const std::shared_ptr<CTransform> &entityCTransform = entity->cTransform;
    const std::shared_ptr<CShape>     &entityCShape     = entity->cShape;

    if (entityCTransform == nullptr) {
      throw std::runtime_error("Entity " + entity->tag() + ", with ID " +
                               std::to_string(entity->id()) + " lacks a transform component.");
    }

    if (entityCShape == nullptr) {
      throw std::runtime_error("Entity " + entity->tag() + ", with ID " +
                               std::to_string(entity->id()) + " lacks a shape component.");
    }

    Vec2 &position = entityCTransform->topLeftCornerPos;
    Vec2 &velocity = entityCTransform->velocity;

    position += velocity * deltaTime * slownessEffectConfig.speed * BASE_MOVEMENT_MULTIPLIER;
  };
} // namespace MovementHelpers