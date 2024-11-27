#include "../../includes/Helpers/MovementHelpers.hpp"

constexpr float BASE_MOVEMENT_MULTIPLIER = 50.0f;

namespace MovementHelpers {

  void moveEnemies(const std::shared_ptr<Entity> &entity,
                   const EnemyConfig             &enemyConfig,
                   const float                   &deltaTime) {

    if (entity == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Entity is null");
      return;
    }

    const EntityTags entityTag = entity->tag();
    if (entityTag != EntityTags::Enemy) {
      return;
    }

    const std::shared_ptr<CTransform> &entityCTransform = entity->getComponent<CTransform>();
    if (entityCTransform == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                   "Entity with ID %zu lacks a transform component.", entity->id());
      return;
    }

    Vec2       &position = entityCTransform->topLeftCornerPos;
    const Vec2 &velocity = entityCTransform->velocity;

    position += velocity * (enemyConfig.speed * (deltaTime * BASE_MOVEMENT_MULTIPLIER));
  }

  void moveSpeedBoosts(const std::shared_ptr<Entity> &entity,
                       const SpeedEffectConfig       &speedBoostEffectConfig,
                       const float                   &deltaTime) {
    if (entity == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Entity is null");
      return;
    }

    const EntityTags entityTag = entity->tag();
    if (entityTag != EntityTags::SpeedBoost) {
      return;
    }

    const std::shared_ptr<CTransform> &entityCTransform = entity->getComponent<CTransform>();
    if (entityCTransform == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                   "Entity with ID %zu lacks a transform component.", entity->id());
      return;
    }

    Vec2       &position = entityCTransform->topLeftCornerPos;
    const Vec2 &velocity = entityCTransform->velocity;

    position += velocity * deltaTime * speedBoostEffectConfig.speed * BASE_MOVEMENT_MULTIPLIER;
  }

  void movePlayer(const std::shared_ptr<Entity> &entity,
                  const PlayerConfig            &playerConfig,
                  const float                   &deltaTime) {
    if (entity == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Entity is null");
      return;
    }

    const EntityTags entityTag = entity->tag();
    if (entityTag != EntityTags::Player) {
      return;
    }

    const std::shared_ptr<CTransform> &entityCTransform = entity->getComponent<CTransform>();
    if (entityCTransform == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                   "Entity with ID %zu lacks a transform component.", entity->id());
      return;
    }

    const std::shared_ptr<CInput> &entityCInput = entity->getComponent<CInput>();
    if (entityCInput == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                   "Entity with ID %zu lacks an input component.", entity->id());
      return;
    }

    Vec2 &position = entityCTransform->topLeftCornerPos;
    Vec2 &velocity = entityCTransform->velocity;

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

    velocity.normalize();

    const std::shared_ptr<CEffects> &entityEffects = entity->getComponent<CEffects>();

    float effectMultiplier = 1;
    if (entityEffects->hasEffect(EffectTypes::Speed)) {
      effectMultiplier = playerConfig.speedBoostMultiplier;
    }

    if (entityEffects->hasEffect(EffectTypes::Slowness)) {
      effectMultiplier = playerConfig.slownessMultiplier;
    }

    velocity *=
        effectMultiplier * deltaTime * BASE_MOVEMENT_MULTIPLIER * playerConfig.baseSpeed;
    position += velocity;
  }

  void moveSlownessDebuffs(const std::shared_ptr<Entity> &entity,
                           const SlownessEffectConfig    &slownessEffectConfig,
                           const float                   &deltaTime) {

    if (entity == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Entity is null");
      return;
    }

    const EntityTags entityTag = entity->tag();
    if (entityTag != EntityTags::SlownessDebuff) {
      return;
    }

    const std::shared_ptr<CTransform> &entityCTransform = entity->getComponent<CTransform>();
    const std::shared_ptr<CShape>     &entityCShape     = entity->getComponent<CShape>();

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

    Vec2       &position = entityCTransform->topLeftCornerPos;
    const Vec2 &velocity = entityCTransform->velocity;

    position += velocity * deltaTime * slownessEffectConfig.speed * BASE_MOVEMENT_MULTIPLIER;
  }

  void moveBullets(const std::shared_ptr<Entity> &entity, const float &deltaTime) {
    if (entity == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Entity is null");
      return;
    }

    const EntityTags entityTag = entity->tag();
    if (entityTag != EntityTags::Bullet) {
      return;
    }

    const std::shared_ptr<CTransform> &entityCTransform = entity->getComponent<CTransform>();
    if (entityCTransform == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                   "Entity with ID %zu lacks a transform component.", entity->id());
      return;
    }

    Vec2       &position = entityCTransform->topLeftCornerPos;
    const Vec2 &velocity = entityCTransform->velocity;

    constexpr float BULLET_MOVEMENT_MULTIPLIER = 3.0f;
    position += velocity * (deltaTime * BULLET_MOVEMENT_MULTIPLIER * BASE_MOVEMENT_MULTIPLIER);
  }
  void moveItems(const std::shared_ptr<Entity> &entity, const float &deltaTime) {
    if (entity == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Entity is null");
      return;
    }

    const EntityTags entityTag = entity->tag();

    if (entityTag != EntityTags::Item) {
      return;
    }

    const std::shared_ptr<CTransform> &entityCTransform = entity->getComponent<CTransform>();

    if (entityCTransform == nullptr) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                   "Entity with ID %zu lacks a transform component.", entity->id());
      return;
    }

    Vec2 &position = entityCTransform->topLeftCornerPos;

    // Use deltaTime to maintain consistent movement speed
    constexpr float ITEM_MOVEMENT_MULTIPLIER = .9f;
    const float     time                     = static_cast<float>(SDL_GetTicks64()) / 1000.0f;
    // Entity id will be odd when the last bit is 1
    const bool ENTITY_ID_ODD = entity->id() & 1;

    if (ENTITY_ID_ODD) {
      position.x +=
          std::cos(time) * (ITEM_MOVEMENT_MULTIPLIER * deltaTime * BASE_MOVEMENT_MULTIPLIER);
      position.y +=
          std::sin(time) * (ITEM_MOVEMENT_MULTIPLIER * deltaTime * BASE_MOVEMENT_MULTIPLIER);

    } else {
      position.x +=
          std::sin(time) * (ITEM_MOVEMENT_MULTIPLIER * deltaTime * BASE_MOVEMENT_MULTIPLIER);
      position.y +=
          std::cos(time) * (ITEM_MOVEMENT_MULTIPLIER * deltaTime * BASE_MOVEMENT_MULTIPLIER);
    }
  }
} // namespace MovementHelpers
