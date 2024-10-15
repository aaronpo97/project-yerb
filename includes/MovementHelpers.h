#ifndef MOVEMENT_HELPERS_H
#define MOVEMENT_HELPERS_H

#include "../includes/Config.h"
#include "../includes/Entity.h"
#include "../includes/EntityTags.h"
#include "../includes/Game.h"
#include <memory>

namespace MovementHelpers {
  void moveEnemies(std::shared_ptr<Entity> &entity,
                   const EnemyConfig       &enemyConfig,
                   const float             &deltaTime);
  void moveSpeedBoosts(std::shared_ptr<Entity> &entity, const float &deltaTime);
  void movePlayer(std::shared_ptr<Entity> &entity,
                  const PlayerConfig      &m_playerConfig,
                  const float             &deltaTime);
} // namespace MovementHelpers

#endif // MOVEMENT_HELPERS_H
