#include "../../includes/Helpers/MovementHelpers.hpp"

#include <memory>
const float BASE_MOVEMENT_MULTIPLIER = 50.0f;

namespace MovementHelpers {
  void moveEnemies(std::shared_ptr<Entity> &entity,
                   const EnemyConfig       &enemyConfig,
                   const float             &deltaTime) {

    std::uniform_int_distribution<int> randomVelocityX(-1, 1);

    if (entity == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Entity is null");
      return;
    }

    const EntityTags entityTag = entity->tag();
    if (entityTag != EntityTags::Enemy) {
      return;
    }

    const std::shared_ptr<CTransform> &entityCTransform = entity->cTransform;
    if (entityCTransform == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                   "Entity with ID %zu lacks a transform component.", entity->id());
      return;
    }

    Vec2 &position = entityCTransform->topLeftCornerPos;
    Vec2 &velocity = entityCTransform->velocity;

    position += velocity * ((enemyConfig.speed) * (deltaTime * BASE_MOVEMENT_MULTIPLIER));
  }
  void moveSpeedBoosts(std::shared_ptr<Entity>      &entity,
                       const SpeedBoostEffectConfig &speedBoostEffectConfig,
                       const float                  &deltaTime) {
    if (entity == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Entity is null");
      return;
    }

    const EntityTags entityTag = entity->tag();
    if (entityTag != EntityTags::SpeedBoost) {
      return;
    }

    const std::shared_ptr<CTransform> &entityCTransform = entity->cTransform;
    if (entityCTransform == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                   "Entity with ID %zu lacks a transform component.", entity->id());
      return;
    }

    Vec2 &position = entityCTransform->topLeftCornerPos;
    Vec2 &velocity = entityCTransform->velocity;

    position += velocity * deltaTime * speedBoostEffectConfig.speed * BASE_MOVEMENT_MULTIPLIER;
  }

  void movePlayer(std::shared_ptr<Entity> &entity,
                  const PlayerConfig      &playerConfig,
                  const float             &deltaTime) {
    if (entity == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Entity is null");
      return;
    }

    const EntityTags entityTag = entity->tag();
    if (entityTag != EntityTags::Player) {
      return;
    }

    const std::shared_ptr<CTransform> &entityCTransform = entity->cTransform;
    if (entityCTransform == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                   "Entity with ID %zu lacks a transform component.", entity->id());
      return;
    }

    const std::shared_ptr<CInput> &entityCInput = entity->cInput;
    if (entityCInput == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                   "Entity with ID %zu lacks an input component.", entity->id());
      return;
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
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Entity is null");
      return;
    }

    const EntityTags entityTag = entity->tag();
    if (entityTag != EntityTags::SlownessDebuff) {
      return;
    }

    const std::shared_ptr<CTransform> &entityCTransform = entity->cTransform;
    const std::shared_ptr<CShape>     &entityCShape     = entity->cShape;

    if (entityCTransform == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                   "Entity with ID %zu lacks a transform component.", entity->id());

      return;
    }

    if (entityCShape == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Entity with ID %zu lacks a shape component.",
                   entity->id());

      return;
    }

    Vec2 &position = entityCTransform->topLeftCornerPos;
    Vec2 &velocity = entityCTransform->velocity;

    position += velocity * deltaTime * slownessEffectConfig.speed * BASE_MOVEMENT_MULTIPLIER;
  };

  void moveBullets(std::shared_ptr<Entity> &entity, const float &deltaTime) {
    if (entity == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Entity is null");
      return;
    }

    const EntityTags entityTag = entity->tag();
    if (entityTag != EntityTags::Bullet) {
      return;
    }

    const std::shared_ptr<CTransform> &entityCTransform = entity->cTransform;
    if (entityCTransform == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                   "Entity with ID %zu lacks a transform component.", entity->id());
      return;
    }

    Vec2 &position = entityCTransform->topLeftCornerPos;
    Vec2 &velocity = entityCTransform->velocity;

    const float BULLET_MOVEMENT_MULTIPLIER = 3.0f;
    position += velocity * (deltaTime * BULLET_MOVEMENT_MULTIPLIER * BASE_MOVEMENT_MULTIPLIER);
  }
} // namespace MovementHelpers