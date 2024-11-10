#pragma once

#include "../Configuration/Config.hpp"
#include "../EntityManagement/Components.hpp"
#include "../EntityManagement/Entity.hpp"
#include "../Helpers/Vec2.hpp"
#include <SDL2/SDL.h>
#include <memory>

namespace MovementHelpers {
  void moveEnemies(const std::shared_ptr<Entity> &entity,
                   const EnemyConfig             &enemyConfig,
                   const float                   &deltaTime);
  void moveSpeedBoosts(const std::shared_ptr<Entity> &entity,
                       const SpeedBoostEffectConfig  &speedBoostEffectConfig,
                       const float                   &deltaTime);
  void movePlayer(const std::shared_ptr<Entity> &entity,
                  const PlayerConfig            &playerConfig,
                  const float                   &deltaTime);

  void moveSlownessDebuffs(const std::shared_ptr<Entity> &entity,
                           const SlownessEffectConfig    &slownessEffectConfig,
                           const float                   &deltaTime);

  void moveBullets(const std::shared_ptr<Entity> &entity, const float &deltaTime);

  void moveItems(const std::shared_ptr<Entity> &entity, const float &deltaTime);
} // namespace MovementHelpers
