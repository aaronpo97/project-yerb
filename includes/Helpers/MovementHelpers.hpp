#pragma once

#include "../Configuration/Config.hpp"
#include "../EntityManagement/Components.hpp"
#include "../EntityManagement/Entity.hpp"
#include "../Helpers/Vec2.hpp"
#include <SDL2/SDL.h>
#include <memory>

namespace MovementHelpers {
  void moveEnemies(std::shared_ptr<Entity> &entity,
                   const EnemyConfig       &enemyConfig,
                   const float             &deltaTime);
  void moveSpeedBoosts(std::shared_ptr<Entity>      &entity,
                       const SpeedBoostEffectConfig &speedBoostEffectConfig,
                       const float                  &deltaTime);
  void movePlayer(std::shared_ptr<Entity> &entity,
                  const PlayerConfig      &m_playerConfig,
                  const float             &deltaTime);

  void moveSlownessDebuffs(std::shared_ptr<Entity>    &entity,
                           const SlownessEffectConfig &slownessEffectConfig,
                           const float                &deltaTime);

  void moveBullets(std::shared_ptr<Entity> &entity, const float &deltaTime);

  void moveItems(std::shared_ptr<Entity> &entity, const float &deltaTime);
} // namespace MovementHelpers
